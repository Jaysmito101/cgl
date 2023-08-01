#![allow(dead_code)]

use std::result;
use libc::{c_void, c_int};



mod macros;

pub mod utils;
pub mod math;
pub mod graphics;
pub mod logger;
// pub use logger::*;
pub mod window;
pub use window::*;

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


/// Shuts down the CGL library.
///
/// This function should be called when the CGL library is no longer needed.
pub fn shutdown() -> () {
    unsafe {
        CGL_shutdown();
    }
}