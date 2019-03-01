//
// Created by Ashe on 2019-02-01.
//

/* Copyright (c) 2000, 2016, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */


/**
  @file

  @brief
  logging of commands

  @todo
    Abort logging when we get an error in reading or writing log files
*/

#include "vs_log.h"
#include "mysqld_error.h"


#include "pfs_file_provider.h"
#include "mysql/psi/mysql_file.h"
#include "mysql/service_my_snprintf.h"

#include <string>
#include <sstream>
#ifdef _WIN32
#include <message.h>
#else
#include <syslog.h>
#endif

#include <pthread.h>
#include <sys/time.h>


using std::min;
using std::max;

/* max size of log messages (error log, plugins' logging, general log) */
static const uint MAX_LOG_BUFFER_SIZE= 1024;
int log_error_level;


/* 26 for regular timestamp, plus 7 (".123456") when using micro-seconds */
static const int iso8601_size= 33;
pthread_mutex_t mutex_error_log;


static void ull2timeval(ulonglong utime, struct timeval *tv)
{
  DBUG_ASSERT(tv != NULL);
  DBUG_ASSERT(utime > 0);      /* should hold true in this context */
  tv->tv_sec= static_cast<long>(utime / 1000000);
  tv->tv_usec=utime % 1000000;
}


/**
  Return time in microseconds.

  @remark This function is to be used to measure performance in
          micro seconds. As it's not defined whats the start time
          for the clock, this function us only useful to measure
          time between two moments.

  @retval Value in microseconds from some undefined point in time.
*/

ulonglong my_micro_time()
{
#ifdef _WIN32
  ulonglong newtime;
  GetSystemTimeAsFileTime((FILETIME*)&newtime);
  newtime-= OFFSET_TO_EPOCH;
  return (newtime/10);
#else
  ulonglong newtime;
  struct timeval t;
  /*
    The following loop is here because gettimeofday may fail on some systems
  */
  while (gettimeofday(&t, NULL) != 0)
  {}
  newtime= (ulonglong)t.tv_sec * 1000000 + t.tv_usec;
  return newtime;
#endif
}


/**
  Make and return an ISO 8601 / RFC 3339 compliant timestamp.
  Heeds log_timestamps.

  @param buf       A buffer of at least 26 bytes to store the timestamp in
                   (19 + tzinfo tail + \0)
  @param seconds   Seconds since the epoch, or 0 for "now"

  @return          length of timestamp (excluding \0)
*/

static int make_iso8601_timestamp(char *buf, ulonglong utime= 0)
{
  struct tm  my_tm;
  char       tzinfo[7]="Z";  // max 6 chars plus \0
  size_t     len;
  time_t     seconds;

  if (utime == 0)
    utime= my_micro_time();

  seconds= utime / 1000000;
  utime = utime % 1000000;

  {
    localtime_r(&seconds, &my_tm);

#ifdef __FreeBSD__
    /*
      The field tm_gmtoff is the offset (in seconds) of the time represented
      from UTC, with positive values indicating east of the Prime Meridian.
    */
    long tim= -my_tm.tm_gmtoff;
#elif _WIN32
    long tim = _timezone;
#else
    long tim= timezone; // seconds West of UTC.
#endif
    char dir= '-';

    if (tim < 0)
    {
      dir= '+';
      tim= -tim;
    }
    my_snprintf(tzinfo, sizeof(tzinfo), "%c%02d:%02d",
                dir, (int) (tim / (60 * 60)), (int) ((tim / 60) % 60));
  }

  len= my_snprintf(buf, iso8601_size, "%04d-%02d-%02dT%02d:%02d:%02d.%06lu%s",
                   my_tm.tm_year + 1900,
                   my_tm.tm_mon  + 1,
                   my_tm.tm_mday,
                   my_tm.tm_hour,
                   my_tm.tm_min,
                   my_tm.tm_sec,
                   (unsigned long) utime,
                   tzinfo);

  return min<int>(len, iso8601_size - 1);
}




////////////////////////////////////////////////////////////
//
// Error Log
//
////////////////////////////////////////////////////////////

static bool error_log_initialized= false;
// This mutex prevents fprintf from different threads from being interleaved.
// It also prevents reopen while we are in the process of logging.
// This variable is different from log_error_dest.
// E.g. log_error_dest is "stderr" if we are not logging to file.
static const char *error_log_file= NULL;
static bool error_log_buffering= true;
static std::string *buffered_messages= NULL;


void flush_error_log_messages()
{
  if (buffered_messages && !buffered_messages->empty())
  {
    fprintf(stderr, "%s", buffered_messages->c_str());
    fflush(stderr);
    delete buffered_messages;
    buffered_messages= NULL;
  }
  error_log_buffering= false;
}


void init_error_log(int log_level)
{
  DBUG_ASSERT(!error_log_initialized);
  pthread_mutex_init(&mutex_error_log,NULL);
  error_log_initialized= true;
  log_error_level = log_level;
}


/**
  Change the file associated with a file stream.

  @param path   Path to file.
  @param mode   Mode of the stream.
  @param stream File stream.

  @note
    This function is used to redirect stdout and stderr to a file and
    subsequently to close and reopen that file for log rotation.

  @retval A FILE pointer on success. Otherwise, NULL.
*/

FILE *my_freopen(const char *path, const char *mode, FILE *stream)
{
  FILE *result;

#if defined(_WIN32)
  result= my_win_freopen(path, mode, stream);
#else
  result= freopen(path, mode, stream);
#endif

  return result;
}

bool open_error_log(const char *filename)
{
  DBUG_ASSERT(filename);
  int retries= 2, errors= 0;

  do
  {
    errors= 0;
    if (!my_freopen(filename, "a", stderr))
      errors++;
#ifndef EMBEDDED_LIBRARY
    if (!my_freopen(filename, "a", stdout))
      errors++;
#endif
  }
  while (retries-- && errors);

  if (errors)
    return true;

   setbuf(stderr,NULL);

  error_log_file= filename; // Remember name for later reopen

  // Write any messages buffered while we were figuring out the filename
  flush_error_log_messages();
  return false;
}


void destroy_error_log()
{
  // We should have flushed before this...
  DBUG_ASSERT(!error_log_buffering);
  // ... but play it safe on release builds
  flush_error_log_messages();
  if (error_log_initialized)
  {
    error_log_initialized= false;
    error_log_file= NULL;
    pthread_mutex_destroy(&mutex_error_log);
  }
}


bool reopen_error_log()
{
  if (!error_log_file)
    return false;
  pthread_mutex_lock(&mutex_error_log);
  bool result= open_error_log(error_log_file);
  pthread_mutex_unlock(&mutex_error_log);
  if (result)
  {
    //todo  my_error(ER_UNKNOWN_ERROR, MYF(0));
  }

  return result;
}


static void print_buffer_to_file(enum loglevel level, const char *buffer,
                                 size_t length)
{

  char my_timestamp[iso8601_size];

  my_thread_id thread_id= 0;

  /*
    If the thread system is up and running and we're in a connection,
    add the connection ID to the log-line, otherwise 0.
  */
  make_iso8601_timestamp(my_timestamp);

  /*
    This must work even if the mutex has not been initialized yet.
    At that point we should still be single threaded so that it is
    safe to write without mutex.
  */
  if(error_log_initialized)
  { pthread_mutex_lock(&mutex_error_log);}


  if (error_log_buffering)
  {
    // Logfile not open yet, buffer messages for now.
    if (buffered_messages == NULL)
      buffered_messages= new (std::nothrow) std::string();
    std::ostringstream s;
    s << my_timestamp << " " << thread_id;
    if (level == ERROR_LEVEL)
      s << " [ERROR] ";
    else if (level == WARNING_LEVEL)
      s << " [Warning] ";
    else
      s << " [Note] ";
    s << buffer << std::endl;
    buffered_messages->append(s.str());
  }
  else
  {
    fprintf(stderr, "%s %u [%s] %.*s\n",
            my_timestamp,
            thread_id,
            (level == ERROR_LEVEL ? "ERROR" : level == WARNING_LEVEL ?
                                              "Warning" : "Note"),
            (int) length, buffer);

    //fflush(stderr); // noneed to flush
  }

  if (error_log_initialized)
    pthread_mutex_unlock(&mutex_error_log);
}


void error_log_print(enum loglevel level, const char *format, va_list args)
{
  char   buff[MAX_LOG_BUFFER_SIZE];
  size_t length;



  if (static_cast<int>(level) < log_error_level) {
    length = my_vsnprintf(buff, sizeof(buff), format, args);
    print_buffer_to_file(level, buff, length);
  }
}

void sql_print_error(const char *format, ...)
{
  va_list args;


  va_start(args, format);
  error_log_print(ERROR_LEVEL, format, args);
  va_end(args);

}


void sql_print_warning(const char *format, ...)
{
  va_list args;


  va_start(args, format);
  error_log_print(WARNING_LEVEL, format, args);
  va_end(args);

}


void sql_print_information(const char *format, ...)
{
  va_list args;


  va_start(args, format);
  error_log_print(INFORMATION_LEVEL, format, args);
  va_end(args);

}
