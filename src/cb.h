#pragma once
#include <memory>
#include <string>
#include <cstdio>
#include <functional>
#include "rust/cxx.h"

template<class Err, class... Args>
class CallBack {
public:
    void(*ok)(Args..., size_t);
    void(*err)(Err, size_t);
    void(*drop)(size_t);
    size_t data;
    
    CallBack(size_t ok_, size_t err_, size_t drop_, size_t data_) {
        ok = (void(*)(Args..., size_t)) ok_;
        err = (void(*)(Err, size_t)) err_;
        drop = (void(*)(size_t)) drop_;
        data = data_;
    }
    void Success(Args... args) {
        (*ok)(args..., data);
    };
    void Error(Err e) {
        (*err)(e, data);
    };
    void Drop() {
        (*drop)(data);
    }
};

using simple_cb = CallBack<std::string, std::string, std::string>;
std::unique_ptr<simple_cb> create_simple(size_t ok_, size_t err_, size_t drop_, size_t data_);
void call_ok(simple_cb& cb, const std::string& a, const std::string& b);
void call_err(simple_cb& cb, const std::string& a);
void call_drop(simple_cb& cb);