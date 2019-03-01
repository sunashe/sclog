//
// Created by Ashe·Sun on 2019-03-01.
//

#ifndef SCLOG_CSLOG_H
#define SCLOG_CSLOG_H
#include <pthread.h>

typedef unsigned int uint;

namespace  my_log
{
    class sclog {
    public:
        int init_cslog();


    private:
        uint log_level_;

    };

}
#endif //SCLOG_CSLOG_H
