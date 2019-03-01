# Install script for directory: /data/mysql-server/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/data/mysql/base")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "/data/mysql-server/include/../libbinlogevents/export/binary_log_types.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/data/mysql-server/include/mysql.h"
    "/data/mysql-server/include/mysql_com.h"
    "/data/mysql-server/include/my_command.h"
    "/data/mysql-server/include/mysql_time.h"
    "/data/mysql-server/include/my_list.h"
    "/data/mysql-server/include/my_alloc.h"
    "/data/mysql-server/include/typelib.h"
    "/data/mysql-server/include/mysql/plugin.h"
    "/data/mysql-server/include/mysql/plugin_audit.h"
    "/data/mysql-server/include/mysql/plugin_ftparser.h"
    "/data/mysql-server/include/mysql/plugin_validate_password.h"
    "/data/mysql-server/include/mysql/plugin_keyring.h"
    "/data/mysql-server/include/mysql/plugin_group_replication.h"
    "/data/mysql-server/include/my_dbug.h"
    "/data/mysql-server/include/m_string.h"
    "/data/mysql-server/include/my_sys.h"
    "/data/mysql-server/include/my_xml.h"
    "/data/mysql-server/include/mysql_embed.h"
    "/data/mysql-server/include/my_thread.h"
    "/data/mysql-server/include/my_thread_local.h"
    "/data/mysql-server/include/decimal.h"
    "/data/mysql-server/include/errmsg.h"
    "/data/mysql-server/include/my_global.h"
    "/data/mysql-server/include/my_getopt.h"
    "/data/mysql-server/include/sslopt-longopts.h"
    "/data/mysql-server/include/my_dir.h"
    "/data/mysql-server/include/sslopt-vars.h"
    "/data/mysql-server/include/sslopt-case.h"
    "/data/mysql-server/include/sql_common.h"
    "/data/mysql-server/include/keycache.h"
    "/data/mysql-server/include/m_ctype.h"
    "/data/mysql-server/include/my_compiler.h"
    "/data/mysql-server/include/mysql_com_server.h"
    "/data/mysql-server/include/my_byteorder.h"
    "/data/mysql-server/include/byte_order_generic.h"
    "/data/mysql-server/include/byte_order_generic_x86.h"
    "/data/mysql-server/include/little_endian.h"
    "/data/mysql-server/include/big_endian.h"
    "/data/mysql-server/include/thr_cond.h"
    "/data/mysql-server/include/thr_mutex.h"
    "/data/mysql-server/include/thr_rwlock.h"
    "/data/mysql-server/include/mysql_version.h"
    "/data/mysql-server/include/my_config.h"
    "/data/mysql-server/include/mysqld_ername.h"
    "/data/mysql-server/include/mysqld_error.h"
    "/data/mysql-server/include/sql_state.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mysql" TYPE DIRECTORY FILES "/data/mysql-server/include/mysql/" REGEX "/[^/]*\\.h$" REGEX "/psi\\_abi[^/]*$" EXCLUDE)
endif()

