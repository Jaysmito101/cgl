#![allow(dead_code)]

use std::ffi::CString;
use std::result;
use libc::{c_void, c_int, c_char};

mod window_manager;
pub use window_manager::MouseButton;
pub use window_manager::Action;
pub use window_manager::Key;
pub use window_manager::Window;
pub use window_manager::Event;


extern {
    fn CGL_init() -> c_int;
    fn CGL_shutdown() -> c_void;
    fn CGL_logger_log(level: c_int, log_format: *const c_char, ...) -> c_void;
}

pub fn init() -> result::Result<(), ()> {
    unsafe {
        let result = CGL_init();
        if result == 0 {
            Err(())
        } else {
            Ok(())
        }   
    }
}

pub fn shutdown() -> () {
    unsafe {
        CGL_shutdown();
    }
}

pub fn log(level: i32, log_format: &str) -> () {
    unsafe {
        let log_format = CString::new(log_format).unwrap();
        CGL_logger_log(level, log_format.as_ptr());
    }
}


/*

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        let result = add(2, 2);
        assert_eq!(result, 4);
    }
}

*/