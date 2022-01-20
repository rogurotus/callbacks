#include "callbacks/src/cb.h"

std::unique_ptr<simple_cb> create_simple(size_t ok_, size_t err_, size_t drop_, size_t data_) {
    return std::make_unique<simple_cb>(simple_cb(ok_, err_, drop_, data_));
}

void call_ok(simple_cb& cb, const std::string& a, const std::string& b) {
    cb.Success(a,b);
}
void call_err(simple_cb& cb, const std::string& a) {
    cb.Error(a);
}
void call_drop(simple_cb& cb) {
    cb.Drop();
}