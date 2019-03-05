#include <iostream>
#include <stdio.h>
#include "sc_log.h"
#include <iostream>
using std::cout;
using std::endl;

int main(int argc,char** argv)
{
    unsigned int times = atoi(argv[1]);
    init_error_log(3);
    char* log_file = "./test.log";
    if(open_error_log(log_file))
    {
        cout << "open err log failed" << endl;
    }
    close_log_buffer();

    for(int i=0;i<times;i++)
    {
        sql_print_information("sclog 测试 %d",i);
    }

    destroy_error_log();
    sql_print_error("关闭后");
//    init_error_log(3);
//    if(open_error_log(log_file))
//    {
//        cout << "open err log failed" << endl;
//    }

    return 0;
}