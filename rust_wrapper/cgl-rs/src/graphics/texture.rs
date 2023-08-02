//! The framebuffer module for CGL

#![allow(non_camel_case_types)]
use libc::{c_void, c_int, c_uint, size_t};

/// The internal handle used by CGL
#[repr(C)]
pub(crate) struct CGL_texture {
    _private: c_void
}

/// The texture format used by CGL
/// These are reflections of OpenGL's enum values
/// You can also use the _i version of methods to pass integers directly instead of enums
pub enum TextureFormat {
    RED     = 0x1903,
    GREEN   = 0x1904,
    BLUE    = 0x1905,
    ALPHA   = 0x1906,
    RGB     = 0x1907,
    RGBA    = 0x1908,
    BGR     = 0x80E0,
    BGRA    = 0x80E1
}

/// The texture internal format used by CGL
/// These are reflections of OpenGL's enum values
/// You can also use the _i version of methods to pass integers directly instead of enums
pub enum TextureInternalFormat {
    RGBA32F             = 0x8814,
    RGB32F              = 0x8815,
    RGBA16F             = 0x881A,
    RGB16F              = 0x881B,
    RGBA32UI            = 0x8D70,
    RGB32UI             = 0x8D71,
    RGBA16UI            = 0x8D76,
    RGB16UI             = 0x8D77,
    RGBA8UI             = 0x8D7C,
    RGB8UI              = 0x8D7D,
    RGBA32I             = 0x8D82,
    RGB32I              = 0x8D83,
    RGBA16I             = 0x8D88,
    RGB16I              = 0x8D89,
    RGBA8I              = 0x8D8E,
    RGB8I               = 0x8D8F,
    RED_INTEGER         = 0x8D94,
    GREEN_INTEGER       = 0x8D95,
    BLUE_INTEGER        = 0x8D96,
    RGB_INTEGER         = 0x8D98,
    RGBA_INTEGER        = 0x8D99,
    BGR_INTEGER         = 0x8D9A,
    BGRA_INTEGER        = 0x8D9B
}


/// The texture data type used by CGL
/// These are reflections of OpenGL's enum values
/// You can also use the _i version of methods to pass integers directly instead of enums
pub enum TextureDataType {
    BYTE              = 0x1400,
    UNSIGNED_BYTE     = 0x1401,
    SHORT             = 0x1402,
    UNSIGNED_SHORT    = 0x1403,
    INT               = 0x1404,
    UNSIGNED_INT      = 0x1405,
    FLOAT             = 0x1406
}

/// The texture scaling mode used by CGL
/// These are reflections of OpenGL's enum values
/// You can also use the _i version of methods to pass integers directly instead of enums
pub enum TextureScalingMode {
    NEAREST                 = 0x2600,
    LINEAR                  = 0x2601,
    NEAREST_MIPMAP_NEAREST  = 0x2700,
    LINEAR_MIPMAP_NEAREST   = 0x2701,
    NEAREST_MIPMAP_LINEAR   = 0x2702,
    LINEAR_MIPMAP_LINEAR    = 0x2703
}


/// The texture wrapping mode used by CGL
/// These are reflections of OpenGL's enum values
/// You can also use the _i version of methods to pass integers directly instead of enums
pub enum TextureWrappingMode {
    REPEAT              = 0x2901,
    MIRRORED_REPEAT     = 0x8370,
    CLAMP_TO_EDGE       = 0x812F,
    CLAMP_TO_BORDER     = 0x812D
}

/// The texture object
pub struct Texture {
    pub(crate) handle: *mut CGL_texture,
    pub(crate) has_been_destroyed: bool
}


#[repr(C)]
pub(crate) struct CGL_image {
    pub(crate) data: *mut c_void,
    pub(crate) height: c_int,
    pub(crate) width: c_int,
    pub(crate) bytes_per_channel: c_int,
    pub(crate) channels: c_int
}

extern {
    fn CGL_texture_create(image: *mut CGL_image) -> *mut CGL_texture;
    fn CGL_texture_create_blank(width: c_int, height: c_int, format: c_uint, internal_format: c_uint, data_type: c_uint) -> *mut CGL_texture;
    fn CGL_texture_create_array(width: c_int, height: c_int, layers: c_int, format: c_uint, internal_format: c_uint, data_type: c_uint) -> *mut CGL_texture;
    fn CGL_texture_create_3d(width: c_int, height: c_int, depth: c_int, format: c_uint, internal_format: c_uint, data_type: c_uint) -> *mut CGL_texture;
    fn CGL_texture_create_cubemap() -> *mut CGL_texture;
    fn CGL_texture_cubemap_set_face(texture: *mut CGL_texture, face: c_int, image: *mut CGL_image);
    fn CGL_texture_array_set_layer_data(texture: *mut CGL_texture, layer: c_int, data: *mut c_void);
    fn CGL_texture_destroy(texture: *mut CGL_texture);
    fn CGL_texture_bind(texture: *mut CGL_texture, unit: c_int) -> c_int;
    fn CGL_texture_set_data(texture: *mut CGL_texture, data: *mut c_void);
    fn CGL_texture_set_sub_data(texture: *mut CGL_texture, offset_x: size_t, offset_y: size_t, size_x: size_t, size_y: size_t, data: *mut c_void);
    fn CGL_texture_set_pixel_data(texture: *mut CGL_texture, x: c_int, y: c_int, data: *mut c_void);
    fn CGL_texture_set_user_data(texture: *mut CGL_texture, user_data: *mut c_void);
    fn CGL_texture_get_user_data(texture: *mut CGL_texture) -> *mut c_void;
    fn CGL_texture_get_internal_handle(texture: *mut CGL_texture) -> c_uint;
    fn CGL_texture_get_size(texture: *mut CGL_texture, width: *mut c_int, height: *mut c_int);
    fn CGL_texture_set_scaling_method(texture: *mut CGL_texture, method: c_int);
    fn CGL_texture_set_wrapping_method(texture: *mut CGL_texture, method: c_int);
}



impl Texture {
    
     /// Creates a new blank texture with the specified dimensions and format.
    /// 
    /// # Arguments
    /// 
    /// * `width` - The width of the texture in pixels.
    /// * `height` - The height of the texture in pixels.
    /// * `format` - The format of the texture data.
    /// * `internal_format` - The internal format of the texture data.
    /// * `data_type` - The data type of the texture data.
    /// 
    /// # Returns
    /// 
    /// A `Result` containing the newly created `Texture` if successful, or an error message if creation failed.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let texture = cgl_rs::graphics::Texture::blank_i(256, 256, cgl_rs::graphics::TextureFormat::RGBA as u32, cgl_rs::graphics::TextureInternalFormat::RGBA8UI as u32, cgl_rs::graphics::TextureDataType::UNSIGNED_BYTE as u32).unwrap();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn blank_i(width: i32, height: i32, format: u32, internal_format: u32, data_type: u32) -> Result<Texture, &'static str> {
        let handle = unsafe {
            CGL_texture_create_blank(width, height, format as c_uint, internal_format as c_uint, data_type as c_uint)
        };
        if handle.is_null() {
            Err("Failed to create texture")
        } else {
            Ok(Texture {
                handle,
                has_been_destroyed: false
            })
        }
    }

    /// Creates a new blank texture with the specified dimensions and format.
    /// 
    /// # Arguments
    /// 
    /// * `width` - The width of the texture in pixels.
    /// * `height` - The height of the texture in pixels.
    /// * `format` - The format of the texture data.
    /// * `internal_format` - The internal format of the texture data.
    /// * `data_type` - The data type of the texture data.
    /// 
    /// # Returns
    /// 
    /// A `Result` containing the newly created `Texture` if successful, or an error message if creation failed.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let texture = cgl_rs::graphics::Texture::blank(256, 256, cgl_rs::graphics::TextureFormat::RGBA, cgl_rs::graphics::TextureInternalFormat::RGBA8UI, cgl_rs::graphics::TextureDataType::UNSIGNED_BYTE).unwrap();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    pub fn blank(width: i32, height: i32, format: TextureFormat, internal_format: TextureInternalFormat, data_type: TextureDataType) -> Result<Texture, &'static str> {
        Texture::blank_i(width, height, format as u32, internal_format as u32, data_type as u32)
    }

    /// Creates a new dummy texture with a single pixel.
    /// 
    /// # Returns
    /// 
    /// A `Result` containing the newly created `Texture` if successful, or an error message if creation failed.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let texture = cgl_rs::graphics::Texture::dummy().unwrap();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    pub fn dummy() -> Result<Texture, &'static str> {
        Texture::blank(1, 1, TextureFormat::RGBA, TextureInternalFormat::RGBA8UI, TextureDataType::UNSIGNED_BYTE)
    }

    /// Creates a new dummy texture with the specified dimensions.
    /// 
    /// # Arguments
    /// 
    /// * `width` - The width of the texture in pixels.
    /// * `height` - The height of the texture in pixels.
    /// 
    /// # Returns
    /// 
    /// A `Result` containing the newly created `Texture` if successful, or an error message if creation failed.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let texture = cgl_rs::graphics::Texture::dummy2(256, 256).unwrap();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    pub fn dummy2(width: i32, height: i32) -> Result<Texture, &'static str> {
        Texture::blank(width, height, TextureFormat::RGBA, TextureInternalFormat::RGBA8UI, TextureDataType::UNSIGNED_BYTE)
    }

    /// Creates a new texture array with the specified dimensions and format.
    /// 
    /// # Arguments
    /// 
    /// * `width` - The width of the texture in pixels.
    /// * `height` - The height of the texture in pixels.
    /// * `depth` - The depth of the texture array.
    /// * `format` - The format of the texture data.
    /// * `internal_format` - The internal format of the texture data.
    /// * `data_type` - The data type of the texture data.
    /// 
    /// # Returns
    /// 
    /// A `Result` containing the newly created `Texture` if successful, or an error message if creation failed.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let texture = cgl_rs::graphics::Texture::array_i(256, 256, 6, cgl_rs::graphics::TextureFormat::RGBA as u32, cgl_rs::graphics::TextureInternalFormat::RGBA8UI as u32, cgl_rs::graphics::TextureDataType::UNSIGNED_BYTE as u32).unwrap();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    pub fn array_i(width: i32, height: i32, depth: i32, format: u32, internal_format: u32, data_type: u32) -> Result<Texture, &'static str> {
        let handle = unsafe {
            CGL_texture_create_array(width, height, depth, format as c_uint, internal_format as c_uint, data_type as c_uint)
        };
        if handle.is_null() {
            Err("Failed to create texture")
        } else {
            Ok(Texture {
                handle,
                has_been_destroyed: false
            })
        }
    }

    // Creates a new texture array with the specified dimensions and format.
    /// 
    /// # Arguments
    /// 
    /// * `width` - The width of the texture in pixels.
    /// * `height` - The height of the texture in pixels.
    /// * `depth` - The depth of the texture array.
    /// * `format` - The format of the texture data.
    /// * `internal_format` - The internal format of the texture data.
    /// * `data_type` - The data type of the texture data.
    /// 
    /// # Returns
    /// 
    /// A `Result` containing the newly created `Texture` if successful, or an error message if creation failed.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let texture = cgl_rs::graphics::Texture::array(256, 256, 6, cgl_rs::graphics::TextureFormat::RGBA, cgl_rs::graphics::TextureInternalFormat::RGBA8UI, cgl_rs::graphics::TextureDataType::UNSIGNED_BYTE).unwrap();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    pub fn array(width: i32, height: i32, depth: i32, format: TextureFormat, internal_format: TextureInternalFormat, data_type: TextureDataType) -> Result<Texture, &'static str> {
        Texture::array_i(width, height, depth, format as u32, internal_format as u32, data_type as u32)
    }

    /// Creates a new cubemap texture.
    /// 
    /// # Returns
    /// 
    /// A `Result` containing the newly created `Texture` if successful, or an error message if creation failed.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let texture = cgl_rs::graphics::Texture::cubemap().unwrap();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    pub fn cubemap() -> Result<Texture, &'static str> {
        let handle = unsafe {
            CGL_texture_create_cubemap()
        };
        if handle.is_null() {
            Err("Failed to create texture")
        } else {
            Ok(Texture {
                handle,
                has_been_destroyed: false
            })
        }
    }

    // TODO : Implement cubemap_set_face

    /// Sets the data for a single layer of a texture array.
    /// 
    /// # Arguments
    /// 
    /// * `layer` - The index of the layer to set the data for.
    /// * `data` - A slice containing the data to set for the layer.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut texture = cgl_rs::graphics::Texture::array(256, 256, 6, cgl_rs::graphics::TextureFormat::RGBA, cgl_rs::graphics::TextureInternalFormat::RGBA8UI, cgl_rs::graphics::TextureDataType::UNSIGNED_BYTE).unwrap();
    ///     let data = vec![0u8; 256 * 256 * 4];
    ///     texture.array_set_layer_data(0, &data);
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    pub fn array_set_layer_data(&self, layer: i32, data: &[u8]) {
        unsafe {
            CGL_texture_array_set_layer_data(self.handle, layer, data.as_ptr() as *mut c_void);
        }
    }

    /// Binds the texture to a texture unit.
    /// 
    /// # Arguments
    /// 
    /// * `unit` - The texture unit to bind the texture to.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let texture = cgl_rs::graphics::Texture::dummy().unwrap();
    ///     texture.bind(0);
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    pub fn bind(&self, unit: u32) {
        unsafe {
            CGL_texture_bind(self.handle, unit as i32);
        }
    }

    /// Sets the data for the texture.
    /// 
    /// # Arguments
    /// 
    /// * `data` - A slice containing the data to set for the texture.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut texture = cgl_rs::graphics::Texture::dummy2(256, 256).unwrap();
    ///     let data = vec![0u8; 256 * 256 * 4];
    ///     texture.set_data(data.as_ptr());
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    pub fn set_data(&self, data: *const u8) {
        unsafe {
            CGL_texture_set_data(self.handle, data as *mut c_void);
        }
    }

    /// Sets a rectangular subregion of the texture with new data.
    ///
    /// # Arguments
    ///
    /// * `x` - The x-coordinate of the lower left corner of the subregion.
    /// * `y` - The y-coordinate of the lower left corner of the subregion.
    /// * `width` - The width of the subregion.
    /// * `height` - The height of the subregion.
    /// * `data` - A slice containing the new data to set for the subregion.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut texture = cgl_rs::graphics::Texture::dummy2(256, 256).unwrap();
    ///     let data = vec![255u8; 100 * 100 * 4];
    ///     texture.set_sub_data(0, 0, 100, 100, data.as_ptr());
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_sub_data(&self, x: usize, y: usize, width: usize, height: usize, data: *const u8) {
        unsafe {
            CGL_texture_set_sub_data(self.handle, x, y, width, height, data as *mut c_void);
        }
    }

    /// Returns the internal OpenGL handle of the texture.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let texture = cgl_rs::graphics::Texture::dummy().unwrap();
    ///     let handle = texture.get_internal_gl_handle();
    ///     println!("Internal OpenGL handle: {}", handle);
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn get_internal_gl_handle(&self) -> u32 {
        unsafe {
            CGL_texture_get_internal_handle(self.handle) as u32
        }
    }

    /// Sets the scaling method of the texture.
    ///
    /// # Arguments
    ///
    /// * `method` - The scaling method to use for the texture.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut texture = cgl_rs::graphics::Texture::dummy2(256, 256).unwrap();
    ///     texture.set_scaling_method_i(1);
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_scaling_method_i(&self, method: i32) {
        unsafe {
            CGL_texture_set_scaling_method(self.handle, method as i32);
        }
    }

    // Sets the scaling method of the texture.
    ///
    /// # Arguments
    ///
    /// * `method` - The scaling method to use for the texture.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut texture = cgl_rs::graphics::Texture::dummy2(256, 256).unwrap();
    ///     texture.set_scaling_method(cgl_rs::graphics::TextureScalingMode::LINEAR);
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_scaling_method(&self, method: TextureScalingMode) {
        self.set_scaling_method_i(method as i32);
    }

    /// Sets the wrapping method of the texture.
    ///
    /// # Arguments
    ///
    /// * `method` - The wrapping method to use for the texture.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let mut texture = cgl_rs::graphics::Texture::dummy2(256, 256).unwrap();
    ///    texture.set_wrapping_method_i(1);
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_wrapping_method_i(&self, method: i32) {
        unsafe {
            CGL_texture_set_wrapping_method(self.handle, method as i32);
        }
    }

    /// Sets the wrapping method of the texture.
    ///
    /// # Arguments
    ///
    /// * `method` - The wrapping method to use for the texture.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let mut texture = cgl_rs::graphics::Texture::dummy2(256, 256).unwrap();
    ///    texture.set_wrapping_method(cgl_rs::graphics::TextureWrappingMode::CLAMP_TO_EDGE);
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_wrapping_method(&self, method: TextureWrappingMode) {
        self.set_wrapping_method_i(method as i32);
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
    ///     let mut texture = cgl_rs::graphics::Texture::dummy().unwrap();
    ///     texture.destroy(); // optional
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    pub fn destroy(&mut self) {
        if !self.has_been_destroyed {
            unsafe {
                CGL_texture_destroy(self.handle);
            }
            self.has_been_destroyed = true;
        }
    }

}

impl Drop for Texture {
    fn drop(&mut self) {
        self.destroy();
    }
}


impl Clone for Texture {
    /// Clones the texture.
    /// 
    /// NOTE: The new instance will have the same handle, `has_been_destroyed` flag.
    ///       This means that the new instance will not be able to receive events nor will the internal window handle be
    ///       destroyed when the new instance is dropped. The internal window handle will be destroyed when the original
    ///       instance is dropped.
    /// 
    /// 
    /// # Returns
    /// 
    /// A new instance of `Texture` with the same handle as the original texture.
    fn clone(&self) -> Self {
        Texture {
            handle: self.handle.clone(),
            has_been_destroyed: true
        }
    }
}