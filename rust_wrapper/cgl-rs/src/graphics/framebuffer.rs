//! The framebuffer module for CGL

#![allow(non_camel_case_types)]
use libc::{c_void, c_int};

/// The internal window handle used by CGL
#[repr(C)]
pub(crate) struct CGL_framebuffer {
    _private: c_void
}

extern {
    fn CGL_framebuffer_create_from_default(window: *mut crate::window::CGL_window) -> *mut CGL_framebuffer;
    fn CGL_framebuffer_create(width: c_int, height: c_int) -> *mut CGL_framebuffer;
    fn CGL_framebuffer_create_basic(width: c_int, height: c_int) -> *mut CGL_framebuffer;
    fn CGL_framebuffer_add_color_attachment(framebuffer: *mut CGL_framebuffer, texture: *mut super::texture::CGL_texture) -> ();
    fn CGL_framebuffer_destroy(framebuffer: *mut CGL_framebuffer) -> ();
    fn CGL_framebuffer_get_color_attachment(framebuffer: *mut CGL_framebuffer, index: c_int) -> *mut super::texture::CGL_texture;
    fn CGL_framebuffer_bind(framebuffer: *mut CGL_framebuffer) -> ();
    fn CGL_framebuffer_get_size(framebuffer: *mut CGL_framebuffer, width: *mut c_int, height: *mut c_int) -> ();
    fn CGL_framebuffer_set_user_data(framebuffer: *mut CGL_framebuffer, user_data: *mut c_void) -> ();
    fn CGL_framebuffer_get_user_data(framebuffer: *mut CGL_framebuffer) -> *mut c_void;
    fn CGL_framebuffer_read_pixels(framebuffer: *mut CGL_framebuffer, x: c_int, y: c_int, width: c_int, height: c_int, pixels: *mut c_void) -> ();
    fn CGL_framebuffer_get_mouse_pick_id(framebuffer: *mut CGL_framebuffer, x: c_int, y: c_int, index: c_int) -> c_int;
    fn CGL_framebuffer_get_color_texture(framebuffer: *mut CGL_framebuffer) -> *mut super::texture::CGL_texture;
    fn CGL_framebuffer_get_depth_texture(framebuffer: *mut CGL_framebuffer) -> *mut super::texture::CGL_texture;
}


/// A framebuffer object that can be used for offscreen rendering.
#[derive(Debug)]
pub struct Framebuffer {
    pub(crate) handle: *mut CGL_framebuffer,
    pub(crate) has_been_destroyed: bool
}


impl Framebuffer {

    /// Creates a new framebuffer object from the default window.
    ///
    /// # Arguments
    ///
    /// * `window` - A reference to the window object to create the framebuffer from.
    ///
    /// # Returns
    ///
    /// Returns a `Result` containing the newly created `Framebuffer` object or an error message if the creation failed.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window =  cgl_rs::Window::new("Hello World", 600, 800).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let framebuffer = cgl_rs::graphics::Framebuffer::from_default(&window).unwrap();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    pub fn from_default(window: &crate::window::Window) -> Result<Framebuffer, &'static str> {
        unsafe {
            let result = CGL_framebuffer_create_from_default(window.get_cgl_handle());
            if result.is_null() {
                Err("Failed to create framebuffer from default")
            } else {
                Ok(Framebuffer {
                    handle: result,
                    has_been_destroyed: false
                })
            }
        }
    }

    /// Creates a new framebuffer object with the specified width and height.
    ///
    /// # Arguments
    ///
    /// * `width` - The width of the framebuffer.
    /// * `height` - The height of the framebuffer.
    ///
    /// # Returns
    ///
    /// Returns a `Result` containing the newly created `Framebuffer` object or an error message if the creation failed.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window =  cgl_rs::Window::new("Hello World", 600, 800).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let framebuffer = cgl_rs::graphics::Framebuffer::new(800, 600).unwrap();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn new(width: i32, height: i32) -> Result<Framebuffer, &'static str> {
        unsafe {
            let result = CGL_framebuffer_create(width, height);
            if result.is_null() {
                Err("Failed to create framebuffer")
            } else {
                Ok(Framebuffer {
                    handle: result,
                    has_been_destroyed: false
                })
            }
        }
    }

    /// Creates a new basic framebuffer object with the specified width and height.
    ///
    /// # Arguments
    ///
    /// * `width` - The width of the framebuffer.
    /// * `height` - The height of the framebuffer.
    ///
    /// # Returns
    ///
    /// Returns a `Result` containing the newly created `Framebuffer` object or an error message if the creation failed.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window =  cgl_rs::Window::new("Hello World", 600, 800).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let framebuffer = cgl_rs::graphics::Framebuffer::new_basic(800, 600).unwrap();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn new_basic(width: i32, height: i32) -> Result<Framebuffer, &'static str> {
        unsafe {
            let result = CGL_framebuffer_create_basic(width, height);
            if result.is_null() {
                Err("Failed to create framebuffer")
            } else {
                Ok(Framebuffer {
                    handle: result,
                    has_been_destroyed: false
                })
            }
        }
    }

    /// Destroys the framebuffer object.
    /// 
    /// Note: This function is called automatically when the framebuffer object goes out of scope.
    ///       But this can also be called manually to destroy the framebuffer object before it goes out of scope.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window =  cgl_rs::Window::new("Hello World", 600, 800).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut framebuffer = cgl_rs::graphics::Framebuffer::new(800, 600).unwrap();
    ///     framebuffer.destroy(); // This is not necessary, but can be called manually.
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn destroy(&mut self) {
        if !self.has_been_destroyed {
            unsafe {
                CGL_framebuffer_destroy(self.handle);
            }
            self.has_been_destroyed = true;
        }
    }

    /// Adds a color attachment to the framebuffer object.
    ///
    /// Note: The texture object passed in will be destroyed automatically after being added to the framebuffer object.
    ///       So even if you clone the parent texture 
    ///
    /// # Arguments
    ///
    /// * `texture` - The texture object to be added as a color attachment to the framebuffer object.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window =  cgl_rs::Window::new("Hello World", 600, 800).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut framebuffer = cgl_rs::graphics::Framebuffer::new(800, 600).unwrap();
    ///     let texture = cgl_rs::graphics::Texture::dummy().unwrap();
    ///     framebuffer.add_color_attachment(texture);
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn add_color_attachment(&mut self, mut texture: crate::graphics::texture::Texture) {
        texture.has_been_destroyed = true;
        unsafe {
            CGL_framebuffer_add_color_attachment(self.handle, texture.handle);
        }
    }

    /// Gets the color attachment at the specified index.
    ///
    /// # Arguments
    ///
    /// * `index` - The index of the color attachment to retrieve.
    ///
    /// # Returns
    ///
    /// Returns `Some(Texture)` if the color attachment exists, otherwise returns `None`.
    /// This returned texture object is managed by the framebuffer object and will be 
    /// destroyed automatically when the framebuffer object is destroyed.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window =  cgl_rs::Window::new("Hello World", 600, 800).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut framebuffer = cgl_rs::graphics::Framebuffer::new_basic(800, 600).unwrap();
    ///     let texture = cgl_rs::graphics::Texture::dummy().unwrap();
    ///     framebuffer.add_color_attachment(texture);
    ///     let color_attachment = framebuffer.get_color_atttachment(0);
    ///     assert!(color_attachment.is_some());
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn get_color_atttachment(&self, index: i32) -> Option<crate::graphics::texture::Texture> {
        unsafe {
            let result = CGL_framebuffer_get_color_attachment(self.handle, index);
            if result.is_null() {
                None
            } else {
                Some(crate::graphics::texture::Texture {
                    handle: result,
                    has_been_destroyed: true
                })
            }
        }
    }


    /// Binds the framebuffer object for rendering.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window =  cgl_rs::Window::new("Hello World", 600, 800).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut framebuffer = cgl_rs::graphics::Framebuffer::new(800, 600).unwrap();
    ///     framebuffer.bind();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn bind(&self) {
        unsafe {
            CGL_framebuffer_bind(self.handle);
        }
    }


    // Gets the size of the framebuffer object.
    ///
    /// # Returns
    ///
    /// Returns a tuple containing the width and height of the framebuffer object.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window =  cgl_rs::Window::new("Hello World", 600, 800).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let framebuffer = cgl_rs::graphics::Framebuffer::new(800, 600).unwrap();
    ///     let (width, height) = framebuffer.get_size();
    ///     assert_eq!(width, 800);
    ///     assert_eq!(height, 600);
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn get_size(&self) -> (i32, i32) {
        unsafe {
            let mut width = 0;
            let mut height = 0;
            CGL_framebuffer_get_size(self.handle, &mut width, &mut height);
            (width, height)
        }
    }

    /// Reads the pixels from the framebuffer object.
    ///
    /// Note: This function currently has some issues and may crash the program.
    /// 
    /// # Arguments
    ///
    /// * `x` - The x coordinate of the lower left corner of the rectangle of pixels to read.
    /// * `y` - The y coordinate of the lower left corner of the rectangle of pixels to read.
    /// * `width` - The width of the rectangle of pixels to read.
    /// * `height` - The height of the rectangle of pixels to read.
    /// 
    ///
    /// # Returns
    ///
    /// Returns a vector of bytes containing the pixel data.
    ///
    /// # Example
    ///
    /// ```norun
    /// cgl_rs::init();
    /// let mut window =  cgl_rs::Window::new("Hello World", 600, 800).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let framebuffer = cgl_rs::graphics::Framebuffer::new(800, 600).unwrap();
    ///     let pixels = framebuffer.read_pixels(0, 0, 100, 100);
    ///     assert_eq!(pixels.len(), 100 * 100 * 4 * 4);
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn read_pixels(&self, x: i32, y: i32, width: i32, height: i32) -> Vec<u8> {
        unsafe {
            let mut buffer = Vec::with_capacity((width * height * 4) as usize);
            buffer.set_len((width * height * 4 * 4) as usize);
            CGL_framebuffer_read_pixels(self.handle, x, y, width, height, buffer.as_mut_ptr() as *mut std::ffi::c_void);
            buffer          
        }
    }


    /// Gets the mouse pick ID at the specified screen coordinates and index.
    ///
    /// # Arguments
    ///
    /// * `x` - The x coordinate of the screen position to check.
    /// * `y` - The y coordinate of the screen position to check.
    /// * `index` - The index of the mouse pick ID to retrieve.
    ///
    /// # Returns
    ///
    /// Returns the mouse pick ID at the specified screen coordinates and index.
    ///
    /// # Safety
    ///
    /// This function is marked as unsafe because it directly calls an unsafe C function.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window =  cgl_rs::Window::new("Hello World", 600, 800).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let framebuffer = cgl_rs::graphics::Framebuffer::new(800, 600).unwrap();
    ///     let mouse_pick_id = framebuffer.get_mouse_pick_id(400, 300, 0);
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn get_mouse_pick_id(&self, x: i32, y: i32, index: i32) -> i32 {
        unsafe {
            CGL_framebuffer_get_mouse_pick_id(self.handle, x, y, index)
        }
    }

    /// Gets the color texture of the framebuffer.
    ///
    /// # Returns
    ///
    /// Returns a `Texture` object representing the color texture of the framebuffer.
    ///
    /// # Safety
    ///
    /// This function is marked as unsafe because it directly calls an unsafe C function.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window =  cgl_rs::Window::new("Hello World", 600, 800).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let framebuffer = cgl_rs::graphics::Framebuffer::new(800, 600).unwrap();
    ///    let texture = framebuffer.get_color_texture();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn get_color_texture(&self) -> crate::graphics::Texture {
        unsafe {
            let handle = CGL_framebuffer_get_color_texture(self.handle);
            crate::graphics::Texture {
                handle,
                has_been_destroyed: true
            }
        }
    }

    /// Gets the depth texture of the framebuffer.
    ///
    /// # Returns
    ///
    /// Returns a `Texture` object representing the depth texture of the framebuffer.
    ///
    /// # Safety
    ///
    /// This function is marked as unsafe because it directly calls an unsafe C function.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window =  cgl_rs::Window::new("Hello World", 600, 800).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let framebuffer = cgl_rs::graphics::Framebuffer::new(800, 600).unwrap();
    ///     let depth_texture = framebuffer.get_depth_texture();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn get_depth_texture(&self) -> crate::graphics::Texture {
        unsafe {
            let handle = CGL_framebuffer_get_depth_texture(self.handle);
            crate::graphics::Texture {
                handle,
                has_been_destroyed: true
            }
        }
    }
}

impl std::ops::Drop for Framebuffer {
    fn drop(&mut self) {
        self.destroy();
    }
}


impl Clone for Framebuffer {
    /// Clones the framebuffer object.
    ///
    /// NOTE: The new instance will have the same handle, `has_been_destroyed` flag.
    ///       This means that the new instance will not be able to receive events nor will the internal window handle be
    ///       destroyed when the new instance is dropped. The internal window handle will be destroyed when the original
    ///       instance is dropped.
    /// 
    /// 
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window =  cgl_rs::Window::new("Hello World", 600, 800).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let framebuffer = cgl_rs::graphics::Framebuffer::new(800, 600).unwrap();
    ///     let cloned_framebuffer = framebuffer.clone();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    fn clone(&self) -> Self {
        Framebuffer {
            handle: self.handle,
            has_been_destroyed: true
        }
    }
}