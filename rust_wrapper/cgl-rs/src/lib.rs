#![allow(dead_code)]

use std::result;
use libc::{c_void, c_int, c_char};

pub mod window_manager;
pub use window_manager::*;

pub mod logger;
// pub use logger::*;

pub mod utils;

extern {
    fn CGL_init() -> c_int;
    fn CGL_shutdown() -> c_void;
}


/// Initializes the CGL library.
/// 
/// This function must be called before any other CGL functions are called.
///
/// # Returns
///
/// Returns `Ok(())` if the initialization was successful, otherwise returns `Err(())`.
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