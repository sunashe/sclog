//
// Created by Ashe·Sun on 2019-03-01.
//

#include "spdlog/spdlog.h"
int main()
{

  int a=10;
  auto my_logger = spdlog::basic_logger_mt("basic_logger", "basic-log.txt");
  my_logger->flush_on(spdlog::level::info);
  for(int i=0;i<1000000;i++)
  {
    my_logger->error("spdlog 测试 %d",a);
  }
  return 0;
}