#![allow(dead_code)]

use std::result;
use libc::{c_void, c_int, c_char};

pub mod window_manager;
pub use window_manager::*;

pub mod logger;
// pub use logger::*;

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