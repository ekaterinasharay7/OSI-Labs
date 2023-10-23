#pragma once
#ifndef Lab1_os
#define Lab1_os
#include <cstring>

struct employee
{
public:
    int num;
    char name[10];
    double hours;

    employee() {
        num = 0;
        strcpy_s(name, "Noname");
        hours = 0.;
    }
    employee(int num_, const char name_[], double hours_) {
        num = num_;
        strcpy_s(name, name_);
        hours = hours_;
    }
};

#endif //Lab1_os