#include <iostream>
#include <stdio.h>
#include "vs_log.h"
#include <iostream>
using std::cout;
using std::endl;

int main() {

  init_error_log(3);
  char* log_file = "./test.log";
  if(open_error_log(log_file))
  {
    cout << "open err log failed" << endl;
  }

  for(int i=0;i<1000000;i++)
  {
    sql_print_error("sclog 测试 %d",i);
  }

  destroy_error_log();
  return 0;
}