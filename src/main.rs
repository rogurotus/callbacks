use cxx::let_cxx_string;
use cxx::{UniquePtr, CxxString};

#[cxx::bridge]
mod ffi {
    extern  "Rust" {
        type my_cb;
        fn create_my_cb(            
            ok_   :usize,
            err_  :usize,
            drop_ :usize,
            data_ :usize,
        ) -> Box<my_cb>;

        fn call_my_cb_ok(
            self: &mut my_cb, a: &CxxString, b:&CxxString
        );

        fn call_my_cb_err(
            self: &mut my_cb, a: &CxxString
        );

        fn call_my_cb_drop(
            self: &mut my_cb
        );
    }
    // C++ types and signatures exposed to Rust.
    unsafe extern "C++" {   
        include!("callbacks/src/cb.h");  
   
        type simple_cb;
        fn create_simple(
            ok_   :usize,
            err_  :usize,
            drop_ :usize,
            data_ :usize,
        ) -> UniquePtr<simple_cb>;   

        fn call_ok(cb: Pin<&mut simple_cb>, a: &CxxString, b:&CxxString);
        fn call_err(cb: Pin<&mut simple_cb>, a: &CxxString);
        fn call_drop(cb: Pin<&mut simple_cb>);
    }

    unsafe extern "C++" {   
        include!("callbacks/src/cb2.h");  
        fn tst();
    }
}

pub struct my_cb(Box<UniquePtr<ffi::simple_cb>>);

impl my_cb {
    pub fn call_my_cb_ok(&mut self, a: &CxxString, b:&CxxString) {
        ffi::call_ok(self.0.pin_mut(), a, b);
    }
    pub fn call_my_cb_err(&mut self, a: &CxxString) {
        ffi::call_err(self.0.pin_mut(), a);
    }
    pub fn call_my_cb_drop(&mut self) {
        ffi::call_drop(self.0.pin_mut());
    }
}

pub fn create_my_cb(
    ok_   :usize,
    err_  :usize,
    drop_ :usize,
    data_ :usize,
) -> Box<my_cb>
{
    Box::new(my_cb(Box::new(ffi::create_simple(ok_, err_, drop_, data_))))
}

fn main() {
    loop {
        ffi::tst(); 
    }
     
}