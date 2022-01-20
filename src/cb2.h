#pragma once
#include <string>
#include <cstdio>
#include "callbacks\src\main.rs.h"
#include "rust/cxx.h"

class Flutter {
    public:
    std::string data;
};

void fn_ok(std::string a, std::string b, size_t data);
void fn_err(std::string a, size_t data);
void fn_drop(size_t data);

void tst();