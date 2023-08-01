//! The CGL Graphics Module

#![allow(non_camel_case_types)]

use std::result;
use libc::{c_void, c_int, c_float};

mod framebuffer;
pub use framebuffer::*;

mod texture;
pub use texture::*;

extern {
    fn CGL_gl_init() -> c_int;
    fn CGL_gl_shutdown() -> c_void;
    fn CGL_gl_clear(r: c_float, g: c_float, b: c_float, a: c_float) -> c_void;
    fn CGL_gl_render_screen_quad() -> c_void;
}


/// Initializes the CGL Graphics Module.
///
/// Note: This method should only be called after there exists a valid OpenGL context.
/// 
/// # Returns
///
/// Returns `Ok(())` if the initialization was successful, otherwise returns `Err(())`.
/// 
/// # Example
/// 
/// ```
/// cgl_rs::init().expect("Failed to initialize CGL");
/// let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
/// window.make_context_current();
/// cgl_rs::graphics::init().expect("Failed to initialize CGL Graphics Module");
/// // ...
/// cgl_rs::graphics::shutdown();
/// window.destroy();
/// cgl_rs::shutdown();
/// ```
pub fn init() -> result::Result<(), ()> {
    unsafe {
        let result = CGL_gl_init();
        if result == 0 {
            Err(())
        } else {
            Ok(())
        }   
    }
}

/// Shuts down the CGL Graphics Module.
/// 
/// This function should be called before cgl_rs::shutdown().
/// 
/// # Example
/// 
/// ```
/// cgl_rs::init().expect("Failed to initialize CGL");
/// let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
/// cgl_rs::graphics::init().expect("Failed to initialize CGL Graphics Module");
/// // ...
/// cgl_rs::graphics::shutdown();
/// window.destroy();
/// cgl_rs::shutdown();
pub fn shutdown() -> () {
    unsafe {
        CGL_gl_shutdown();
    }
}



/// Clears the screen with the specified color.
///
/// # Arguments
///
/// * `r` - The red component of the color, in the range [0.0, 1.0].
/// * `g` - The green component of the color, in the range [0.0, 1.0].
/// * `b` - The blue component of the color, in the range [0.0, 1.0].
/// * `a` - The alpha component of the color, in the range [0.0, 1.0].
///
/// # Example
///
/// ```
/// cgl_rs::init().expect("Failed to initialize CGL");
/// let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
/// cgl_rs::graphics::init().expect("Failed to initialize CGL Graphics Module");
/// cgl_rs::graphics::clear(0.0, 0.0, 0.0, 1.0); // Clears the screen to black
/// // ...
/// cgl_rs::graphics::shutdown();
/// window.destroy();
/// cgl_rs::shutdown();
/// ```
pub fn clear(r: f32, g: f32, b: f32, a: f32) -> () {
    unsafe {
        CGL_gl_clear(r, g, b, a);
    }
}

/// Renders a screen quad.
///
/// # Example
///
/// ```
/// cgl_rs::init().expect("Failed to initialize CGL");
/// let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
/// cgl_rs::graphics::init().expect("Failed to initialize CGL Graphics Module");
/// cgl_rs::graphics::render_screen_quad(); // Renders a screen quad
/// // ...
/// cgl_rs::graphics::shutdown();
/// window.destroy();
/// cgl_rs::shutdown();
/// ```
pub fn render_screen_quad() -> () {
    unsafe {
        CGL_gl_render_screen_quad();
    }
}