//! This provices CGL's Bloom post-processing effect (based on Unity's Bloom effect)

#![allow(non_camel_case_types)]
use libc::{c_void, c_int, c_float};

use crate::graphics::texture::{Texture, CGL_texture};

/// The internal handle used by CGL
#[repr(C)]
pub(crate) struct CGL_bloom {
    _private: c_void
}

extern {
    fn CGL_bloom_create(width: c_int, height: c_int, iterations: c_int) -> *mut CGL_bloom;
    fn CGL_bloom_destroy(bloom: *mut CGL_bloom) -> c_void;
    fn CGL_bloom_set_threshold(bloom: *mut CGL_bloom, val: c_float) -> c_void;
    fn CGL_bloom_get_threshold(bloom: *mut CGL_bloom) -> c_float;
    fn CGL_bloom_set_knee(bloom: *mut CGL_bloom, val: c_float) -> c_void;
    fn CGL_bloom_get_knee(bloom: *mut CGL_bloom) -> c_float;
    fn CGL_bloom_set_offset(bloom: *mut CGL_bloom, x: c_float, y: c_float) -> c_void;
    fn CGL_bloom_apply(bloom: *mut CGL_bloom, tex: *mut CGL_texture) -> c_void;
    fn CGL_bloom_apply2(bloom: *mut CGL_bloom, tex_src: *mut CGL_texture, tex_dst: *mut CGL_texture) -> c_void;
    fn CGL_bloom_get_iterations(bloom: *mut CGL_bloom) -> c_int;
    fn CGL_bloom_get_lod_texture(bloom: *mut CGL_bloom, index: c_int) -> *mut CGL_texture;
    fn CGL_bloom_get_prefiltered_texture(bloom: *mut CGL_bloom) -> *mut CGL_texture;
}

/// The Bloom Object
#[repr(C)]
pub struct Bloom {
    pub(crate) handle: *mut CGL_bloom,
    pub(crate) has_been_destroyed: bool
}

impl Bloom {
    /// Creates a new Bloom object with the specified width, height, and number of iterations.
    /// 
    /// # Arguments
    /// 
    /// * `width` - The width of the Bloom effect.
    /// * `height` - The height of the Bloom effect.
    /// * `iterations` - The number of iterations to apply the Bloom effect.
    /// 
    /// # Returns
    /// 
    /// Returns a new Bloom object if successful, otherwise returns an error message.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let bloom = cgl_rs::graphics::Bloom::new(800, 600, 3).unwrap();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn new(width: i32, height: i32, iterations: i32) -> Result<Bloom, &'static str> {
        unsafe {
            let handle = CGL_bloom_create(width, height, iterations);
            if handle.is_null() {
                Err("Failed to create Bloom")
            } else {
                Ok(Bloom {
                    handle,
                    has_been_destroyed: false
                })
            }
        }
    }

    

    /// Destroys the texture and frees its resources. If the texture has already been destroyed, this method does nothing.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut bloom = cgl_rs::graphics::Bloom::new(800, 600, 3).unwrap();
    ///     bloom.destroy(); // optional
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    pub fn destroy(&mut self) {
        if !self.has_been_destroyed {
            unsafe {
                CGL_bloom_destroy(self.handle);
            }
            self.has_been_destroyed = true;
        }
    }


    /// Sets the threshold value for the Bloom effect.
    /// 
    /// # Arguments
    /// 
    /// * `val` - The threshold value to set.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let bloom = cgl_rs::graphics::Bloom::new(800, 600, 3).unwrap();
    ///    bloom.set_threshold(0.5);
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_threshold(&self, val: f32) {
        unsafe {
            CGL_bloom_set_threshold(self.handle, val);
        }
    }

    /// Gets the threshold value for the Bloom effect.
    /// 
    /// # Returns
    /// 
    /// Returns the current threshold value.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let bloom = cgl_rs::graphics::Bloom::new(800, 600, 3).unwrap();
    ///    let threshold = bloom.get_threshold();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn get_threshold(&self) -> f32 {
        unsafe {
            CGL_bloom_get_threshold(self.handle)
        }
    }

    /// Sets the knee value for the Bloom effect.
    /// 
    /// # Arguments
    /// 
    /// * `val` - The knee value to set.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let bloom = cgl_rs::graphics::Bloom::new(800, 600, 3).unwrap();
    ///    bloom.set_knee(0.5);
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_knee(&self, val: f32) {
        unsafe {
            CGL_bloom_set_knee(self.handle, val);
        }
    }

    /// Gets the knee value for the Bloom effect.
    /// 
    /// # Returns
    /// 
    /// Returns the current knee value.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let bloom = cgl_rs::graphics::Bloom::new(800, 600, 3).unwrap();
    ///    let knee = bloom.get_knee();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn get_knee(&self) -> f32 {
        unsafe {
            CGL_bloom_get_knee(self.handle)
        }
    }

    /// Sets the offset values for the Bloom effect.
    /// 
    /// # Arguments
    /// 
    /// * `x` - The x offset value to set.
    /// * `y` - The y offset value to set.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let bloom = cgl_rs::graphics::Bloom::new(800, 600, 3).unwrap();
    ///    bloom.set_offset(0.5, 0.5);
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_offset(&self, x: f32, y: f32) {
        unsafe {
            CGL_bloom_set_offset(self.handle, x, y);
        }
    }


    /// Applies the Bloom effect to a texture.
    /// 
    /// # Arguments
    /// 
    /// * `tex` - The texture to apply the Bloom effect to.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let bloom = cgl_rs::graphics::Bloom::new(800, 600, 3).unwrap();
    ///    let texture = cgl_rs::graphics::Texture::dummy2(600, 600).unwrap();
    ///    bloom.apply(&texture);
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn apply(&self, tex: &Texture) {
        unsafe {
            CGL_bloom_apply(self.handle, tex.handle);
        }
    }

    /// Applies the Bloom effect to a source texture and writes the result to a destination texture.
    /// 
    /// # Arguments
    /// 
    /// * `tex_src` - The source texture to apply the Bloom effect to.
    /// * `tex_dst` - The destination texture to write the result to.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let bloom = cgl_rs::graphics::Bloom::new(800, 600, 3).unwrap();
    ///    let texture_src = cgl_rs::graphics::Texture::dummy2(600, 600).unwrap();
    ///    let texture_dst = cgl_rs::graphics::Texture::dummy2(600, 600).unwrap();
    ///    bloom.apply2(&texture_src, &texture_dst);
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn apply2(&self, tex_src: &Texture, tex_dst: &Texture) {
        unsafe {
            CGL_bloom_apply2(self.handle, tex_src.handle, tex_dst.handle);
        }
    }

    /// Gets the value of the number of iterations for the Bloom effect.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let bloom = cgl_rs::graphics::Bloom::new(800, 600, 3).unwrap();
    ///    let iterations = bloom.get_iterations();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn get_iterations(&self) -> i32 {
        unsafe {
            CGL_bloom_get_iterations(self.handle)
        }
    }

    /// Gets the texture for a specific level of detail (LOD) for the Bloom effect.
    /// 
    /// This texture is owned and managed by the Bloom effect and should not be destroyed manually.
    /// 
    /// # Arguments
    /// 
    /// * `index` - The index of the LOD texture to retrieve.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let bloom = cgl_rs::graphics::Bloom::new(800, 600, 3).unwrap();
    ///    let lod_texture = bloom.get_lod_texture(0);
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn get_lod_texture(&self, index: i32) -> Result<Texture, &'static str> {
        unsafe {
            let handle = CGL_bloom_get_lod_texture(self.handle, index);
            if handle.is_null() {
                Err("Failed to get LOD texture")
            } else {
                Ok(Texture {
                    handle,
                    has_been_destroyed: true
                })
            }
        }
    }

    /// Gets the prefiltered texture for the Bloom effect.
    /// 
    /// This texture is owned and managed by the Bloom effect and should not be destroyed manually.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let bloom = cgl_rs::graphics::Bloom::new(800, 600, 3).unwrap();
    ///    let prefiltered_texture = bloom.get_prefiltered_texture();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn get_prefiltered_texture(&self) -> Result<Texture, &'static str> {
        unsafe {
            let handle = CGL_bloom_get_prefiltered_texture(self.handle);
            if handle.is_null() {
                Err("Failed to get prefiltered texture")
            } else {
                Ok(Texture {
                    handle,
                    has_been_destroyed: true
                })
            }
        }
    }
}


impl Drop for Bloom {
    fn drop(&mut self) {
        self.destroy();
    }
}

impl Clone for Bloom {
    fn clone(&self) -> Self {
        Bloom {
            handle: self.handle.clone(),
            has_been_destroyed: true
        }
    }
}