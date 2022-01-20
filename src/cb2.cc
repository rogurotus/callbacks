
#include "callbacks/src/cb2.h"

void fn_ok(std::string a, std::string b, size_t data) {
    auto f = (Flutter*) data;
    printf("OK %s%s%s\n", a,b,f->data);
}
void fn_err(std::string a, size_t data) {
    auto f = (Flutter*) data;
    printf("ERR %s%s\n", a,f->data);
}
void fn_drop(size_t data) {
    auto f = (Flutter*) data;
    delete f;
}

void tst() {
    auto data = new Flutter();
    data->data = "Flutter";
    auto cb = create_my_cb(
        (size_t)fn_ok, 
        (size_t)fn_err, 
        (size_t)fn_drop, 
        (size_t)data
    );

    cb->call_my_cb_ok("test", "test");
    cb->call_my_cb_drop();

}