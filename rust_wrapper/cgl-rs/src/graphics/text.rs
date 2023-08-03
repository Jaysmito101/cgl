//! The text module contains the text rendering, font loading functionality of CGL

#![allow(non_camel_case_types)]
use libc::{c_void, c_int, c_char, size_t, c_uchar};

use crate::graphics::texture::{Texture,  CGL_texture};
use crate::math::Vector2;

// The internal font handle
#[repr(C)]
pub(crate) struct CGL_font {
    _private: c_void
}

/// The Font Character struct contains all the information about a character in a font
#[repr(C)] #[derive(Copy, Clone)]
pub struct FontCharacter {
    pub size: Vector2,
    pub normalized_size: Vector2,
    pub offset: Vector2,
    pub normalized_offset: Vector2,
    pub bearing: Vector2,
    pub bearing_normalized: Vector2,
    pub(crate) bitmap: *mut c_uchar,
    pub ch: c_char
}


extern {
    fn CGL_text_init() -> c_int;
    fn CGL_text_shutdown() -> c_void;
    fn CGL_font_load(path: *const c_char) -> *mut CGL_font;
    fn CGL_font_load_from_memory(data: *const c_char, size: size_t) -> *mut CGL_font;
    fn CGL_font_destory(font: *mut CGL_font) -> c_void;
    fn CGL_font_get_atlas(font: *mut CGL_font) -> *mut CGL_texture;
    fn CGL_font_build_atlas(font: *mut CGL_font, width: size_t, height: size_t, font_size: size_t) -> c_int;
    fn CGL_font_get_characters(font: *mut CGL_font) -> *mut FontCharacter;
    fn CGL_text_bake_to_texture(string: *const c_char, string_length: size_t, font: *mut CGL_font, width: *mut size_t, height: *mut size_t) -> *mut CGL_texture;
}

/// Initialized the text module
/// 
/// Note: This function must be called before any other text functions
/// 
/// # Example
/// 
/// ```
/// cgl_rs::init();
/// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
/// cgl_rs::graphics::init();
/// cgl_rs::graphics::text::init();
/// // Do stuff
/// cgl_rs::graphics::text::shutdown();
/// cgl_rs::graphics::shutdown();
/// window.destroy();
/// cgl_rs::shutdown();
/// ```
pub fn init() {
    unsafe {
        CGL_text_init();
    }
}

/// Shuts down the text module
/// 
/// Note: This function must be called after all other text functions have been called
/// 
/// # Example
/// 
/// ```
/// cgl_rs::init();
/// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
/// cgl_rs::graphics::init();
/// cgl_rs::graphics::text::init();
/// // Do stuff
/// cgl_rs::graphics::text::shutdown();
/// cgl_rs::graphics::shutdown();
/// window.destroy();
/// cgl_rs::shutdown();
/// ```
pub fn shutdown() {
    unsafe {
        CGL_text_shutdown();
    }
}

/// The public font object
pub struct Font {
    pub(crate) handle: *mut CGL_font,
    pub(crate) has_been_destroyed: bool
}

impl std::ops::Index<u8> for Font {
    type Output = FontCharacter;

    /// Indexes the font's characters by their ASCII value.
    ///
    /// # Arguments
    ///
    /// * `index` - An `i8` representing the ASCII value of the character to retrieve.
    ///
    /// # Returns
    ///
    /// Returns a reference to the `FontCharacter` object corresponding to the given ASCII value.
    ///
    /// # Safety
    ///
    /// This function is marked as unsafe because it dereferences a raw pointer returned by the `CGL_font_get_characters` function.
    ///
    /// # Example
    /// 
    /// ```no_run
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// cgl_rs::graphics::text::init();
    /// let font = cgl_rs::graphics::text::Font::load("path/to/font.ttf").unwrap();
    /// let character = font[b'c'];
    /// cgl_rs::graphics::text::shutdown();
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    fn index(&self, index: u8) -> &Self::Output {
        unsafe {
            &*CGL_font_get_characters(self.handle).offset(index as isize)
        }
    }
}

impl Font {

    /// Loads a font from a file path
    ///
    /// # Arguments
    ///
    /// * `path` - A string slice that holds the path to the font file
    ///
    /// # Returns
    ///
    /// Returns a `Result` containing a `Font` object if the font was loaded successfully, or an error message if it failed to load.
    ///
    /// # Example
    ///
    /// ```no_run
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// cgl_rs::graphics::text::init();
    /// {
    ///     let font = cgl_rs::graphics::text::Font::load("path/to/font.ttf").unwrap();
    /// }
    /// cgl_rs::graphics::text::shutdown();
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn load(path: &str) -> Result<Font, &'static str> {
        let c_path = std::ffi::CString::new(path).unwrap();
        let handle = unsafe { CGL_font_load(c_path.as_ptr()) };
        if handle.is_null() {
            Err("Failed to load font")
        } else {
            Ok(Font {
                handle,
                has_been_destroyed: false
            })
        }
    }

    /// Loads a font from memory
    ///
    /// # Arguments
    ///
    /// * `data` - A pointer to the font data in memory
    /// * `size` - The size of the font data in bytes
    ///
    /// # Returns
    ///
    /// Returns a `Result` containing a `Font` object if the font was loaded successfully, or an error message if it failed to load.
    ///
    /// # Example
    ///
    /// ```no_run
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// cgl_rs::graphics::text::init();
    /// {
    ///     // let font_data = include_bytes!("path/to/font.ttf");
    ///     // let font = cgl_rs::graphics::text::Font::load_from_memory(font_data.as_ptr(), font_data.len()).unwrap();
    /// }
    /// cgl_rs::graphics::text::shutdown();
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn load_from_memory(data: *const u8, size: usize) -> Result<Font, &'static str> {
        let handle = unsafe { CGL_font_load_from_memory(data as *const c_char, size) };
        if handle.is_null() {
            Err("Failed to load font")
        } else {
            Ok(Font {
                handle,
                has_been_destroyed: false
            })
        }
    }

    /// Destroys the font object and frees any resources associated with it.
    ///
    /// # Example
    ///
    /// ```no_run
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// cgl_rs::graphics::text::init();
    /// {
    ///     let mut font = cgl_rs::graphics::text::Font::load("path/to/font.ttf").unwrap();
    ///     // Use the font...
    ///     font.destroy(); // Or, just let the font go out of scope and it will be destroyed automatically.
    /// }
    /// cgl_rs::graphics::text::shutdown();
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn destroy(&mut self) {
        if !self.has_been_destroyed {
            unsafe {
                CGL_font_destory(self.handle);
            }
            self.has_been_destroyed = true;
        }
    }

    /// Returns the texture atlas for the font. This function must be called after `build_atlas` has been called.
    /// 
    /// Note: This atlas texture is managed by the font object and will be destroyed when the font object is destroyed.
    ///
    /// # Returns
    ///
    /// Returns a `Texture` object representing the texture atlas for the font.
    ///
    /// # Example
    ///
    /// ```no_run
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// cgl_rs::graphics::text::init();
    /// {
    ///     let font = cgl_rs::graphics::text::Font::load("path/to/font.ttf").unwrap();
    ///     font.build_atlas(512, 512, 32).unwrap();
    ///     let texture = font.get_atlas();
    ///     // Use the texture...
    /// }
    /// cgl_rs::graphics::text::shutdown();
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn get_atlas(&self) -> Texture {
        let handle = unsafe { CGL_font_get_atlas(self.handle) };
        Texture {
            handle,
            has_been_destroyed: true
        }
    }

    /// Builds the texture atlas for the font. This function must be called before any text can be rendered with the font.
    ///
    /// # Arguments
    ///
    /// * `width` - The width of the texture atlas in pixels.
    /// * `height` - The height of the texture atlas in pixels.
    /// * `font_size` - The size of the font in pixels.
    ///
    /// # Returns
    ///
    /// Returns `Ok(())` if the texture atlas was built successfully, otherwise returns an error message.
    ///
    /// # Example
    ///
    /// ```no_run
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// cgl_rs::graphics::text::init();
    /// {
    ///     let font = cgl_rs::graphics::text::Font::load("path/to/font.ttf").unwrap();
    ///     font.build_atlas(512, 512, 32).unwrap();
    ///     // Use the font...
    /// }
    /// cgl_rs::graphics::text::shutdown();
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn build_atlas(&self, width: usize, height: usize, font_size: usize) -> Result<(), &'static str> {
        let result = unsafe { CGL_font_build_atlas(self.handle, width, height, font_size) };
        if result != 0 {
            Ok(())
        } else {
            Err("Failed to build font atlas")
        }
    }
}

impl Drop for Font {
    fn drop(&mut self) {
        self.destroy();
    }
}

impl Clone for Font {
    fn clone(&self) -> Self {
        Font {
            handle: self.handle.clone(),
            has_been_destroyed: true
        }
    }
}

/// Bakes the given text into a texture using the specified font and font size.
/// 
/// Note: This texture is not managed by the font object and the ownership of the texture is passed to the caller.
///
/// # Arguments
///
/// * `font` - The font to use for rendering the text.
/// * `text` - The text to render.
///
/// # Returns
///
/// Returns a tuple containing the resulting `Texture` object, as well as the width and height of the texture in pixels.
///
/// # Example
///
/// ```no_run
/// cgl_rs::init();
/// let mut window = cgl_rs::Window::new("CGL Window", 800, 600).unwrap();
/// cgl_rs::graphics::init();
/// cgl_rs::graphics::text::init();
/// {
///     let font = cgl_rs::graphics::text::Font::load("path/to/font.ttf").unwrap();
///     let (texture, width, height) = cgl_rs::graphics::text::bake_to_texture(&font, "Hello, world!").unwrap();
///     // Use the texture...
/// }
/// cgl_rs::graphics::text::shutdown();
/// cgl_rs::graphics::shutdown();
/// window.destroy();
/// cgl_rs::shutdown();
/// ```
pub fn bake_to_texture(font: &Font, text: &str) -> Result<(Texture, u32, u32), &'static str> {
    let mut width_sz: size_t = 0;
    let mut height_sz: size_t = 0;
    let width_ptr = &mut width_sz as *mut size_t;
    let height_ptr = &mut height_sz as *mut size_t;
    let string_in_c = std::ffi::CString::new(text).unwrap();
    let handle = unsafe { CGL_text_bake_to_texture(string_in_c.as_ptr(), text.len(), font.handle, width_ptr, height_ptr) };
    if handle.is_null() {
        Err("Failed to bake text to texture")
    } else {
        Ok((Texture {
            handle,
            has_been_destroyed: false
        }, width_sz as u32, height_sz as u32))
    }
}