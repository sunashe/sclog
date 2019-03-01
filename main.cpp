#include <iostream>
#include "sclog.h"
#include <stdio.h>
#include "vs_log.h"
#include <iostream>

using std::cout;
using std::endl;

using my_log::sclog;

int main() {

  init_error_log(3);
  char* log_file = "./test.log";
  if(open_error_log(log_file))
  {
    cout << "open err log failed" << endl;
  }

  int a=10;
  for(int i=0;i<1000000;i++)
  {
    sql_print_error("init semisync_slave plugin error %d 你是我的",a);
  }

  return 0;
}