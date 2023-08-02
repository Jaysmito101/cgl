//! The framebuffer module for CGL

#![allow(non_camel_case_types)]
use libc::{c_void, c_int, size_t};

/// The internal handle used by CGL
#[repr(C)]
pub(crate) struct CGL_ssbo {
    _private: c_void
}


/// The Shader Storage Buffer Object
pub struct ShaderStorageBufferObject {
    pub(crate) handle: *mut CGL_ssbo,
    pub(crate) has_been_destroyed: bool
}

// This is just an alias for ShaderStorageBufferObject
pub type SSBO = ShaderStorageBufferObject; 


extern {
    fn CGL_ssbo_create(binding: u32) -> *mut CGL_ssbo;
    fn CGL_ssbo_destroy(ssbo: *mut CGL_ssbo);
    fn CGL_ssbo_bind(ssbo: *mut CGL_ssbo);
    fn CGL_ssbo_bind2(ssbo: *mut CGL_ssbo, binding: u32);
    fn CGL_ssbo_set_data(ssbo: *mut CGL_ssbo, size: size_t, data: *mut c_void, static_draw: c_int);
    fn CGL_ssbo_set_sub_data(ssbo: *mut CGL_ssbo, offset: size_t, size: size_t, data: *mut c_void, static_draw: c_int);
    fn CGL_ssbo_get_data(ssbo: *mut CGL_ssbo, size: *mut size_t, data: *mut c_void);
    fn CGL_ssbo_get_sub_data(ssbo: *mut CGL_ssbo, offset: size_t, size: size_t, data: *mut c_void);
    fn CGL_ssbo_set_user_data(ssbo: *mut CGL_ssbo, user_data: *mut c_void);
    fn CGL_ssbo_get_user_data(ssbo: *mut CGL_ssbo) -> *mut c_void;
    fn CGL_ssbo_get_size(ssbo: *mut CGL_ssbo) -> size_t;
    fn CGL_ssbo_copy(dst: *mut CGL_ssbo, src: *mut CGL_ssbo, src_offset: size_t, dst_offset: size_t, size: size_t);
}



impl ShaderStorageBufferObject {

    /// Creates a new Shader Storage Buffer Object with the specified binding.
    /// 
    /// # Arguments
    /// 
    /// * `binding` - The binding point for the SSBO.
    /// 
    /// # Returns
    /// 
    /// A new instance of `ShaderStorageBufferObject` with the specified binding, or `Err(())` if the creation failed.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let mut ssbo = cgl_rs::graphics::ShaderStorageBufferObject::new(0).unwrap();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn new(binding: u32) -> Result<ShaderStorageBufferObject, ()> {
        let handle = unsafe { CGL_ssbo_create(binding) };
        if handle.is_null() {
            Err(())
        } else {
            Ok(ShaderStorageBufferObject {
                handle,
                has_been_destroyed: false
            })
        }
    }

    /// Creates a new Shader Storage Buffer Object with the specified binding and sets its data to the provided byte slice.
    /// 
    /// # Arguments
    /// 
    /// * `data` - The byte slice containing the data to be set.
    /// * `binding` - The binding point for the SSBO.
    /// 
    /// # Returns
    /// 
    /// A new instance of `ShaderStorageBufferObject` with the specified binding and data, or `Err(())` if the creation failed.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let data = vec![0u8; 1024];
    ///     let ssbo = cgl_rs::graphics::ShaderStorageBufferObject::from(&data, 0).unwrap();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn from(data: &[u8], binding: u32) -> Result<ShaderStorageBufferObject, ()> {
        let mut ssbo = ShaderStorageBufferObject::new(binding)?;
        ssbo.set_data(data.len(), data.as_ptr() as *mut u8, false);
        Ok(ssbo)
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
    /// ```
    pub fn destroy(&mut self) {
        if !self.has_been_destroyed {
            unsafe {
                CGL_ssbo_destroy(self.handle);
            }
            self.has_been_destroyed = true;
        }
    }

    /// Binds the Shader Storage Buffer Object to the current context.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let ssbo = cgl_rs::graphics::ShaderStorageBufferObject::new(0).unwrap();
    ///   ssbo.bind();
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn bind(&self) {
        unsafe {
            CGL_ssbo_bind(self.handle);
        }
    }
    

    /// Binds the Shader Storage Buffer (Base)  to the specified binding point.
    /// 
    /// # Arguments
    /// 
    /// * `binding` - The binding point to bind the Shader Storage Buffer Object to.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let mut ssbo = cgl_rs::graphics::ShaderStorageBufferObject::new(0).unwrap();
    ///    ssbo.bind2(1);
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn bind2(&mut self, binding: u32) {
        unsafe {
            CGL_ssbo_bind2(self.handle, binding);
        }
    }

    /// Sets the data of the Shader Storage Buffer Object.
    /// This also allocates the memory on the GPU if necessary.
    /// 
    /// If you only want to update a part of the data, use `set_sub_data` instead.
    /// 
    /// # Arguments
    /// 
    /// * `size` - The size of the data in bytes.
    /// * `data` - A pointer to the data.
    /// * `static_draw` - A flag indicating whether the data is static or not.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let mut ssbo = cgl_rs::graphics::ShaderStorageBufferObject::new(0).unwrap();
    ///    let data: [u8; 4] = [0, 1, 2, 3];
    ///    ssbo.set_data(data.len(), data.as_ptr() as *mut u8, true);
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    /// 
    /// # See also
    /// 
    /// * `set_sub_data`
    pub fn set_data(&mut self, size: usize, data: *mut u8, static_draw: bool) {
        unsafe {
            CGL_ssbo_set_data(self.handle, size, data as *mut c_void, static_draw as c_int);
        }
    }

    /// Sets a portion of the data of the Shader Storage Buffer Object.
    /// 
    /// Note: This does not allocate the memory on the GPU if necessary. 
    ///       So, if the data is not allocated on the GPU, this method will do nothing.
    ///       To allocate the memory on the GPU, use `set_data` instead.
    /// 
    /// # Arguments
    /// 
    /// * `offset` - The offset in bytes from the beginning of the buffer where the data should be written.
    /// * `size` - The size of the data in bytes.
    /// * `data` - A pointer to the data.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let mut ssbo = cgl_rs::graphics::ShaderStorageBufferObject::new(0).unwrap();
    ///    let data: [u8; 4] = [0, 1, 2, 3];
    ///    ssbo.set_data(data.len(), data.as_ptr() as *mut u8, true);
    ///    let sub_data: [u8; 2] = [4, 5];
    ///   ssbo.set_sub_data(2, sub_data.len(), sub_data.as_ptr() as *mut u8);
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    /// 
    /// # See also
    /// 
    /// * `set_data`
    pub fn set_sub_data(&mut self, offset: usize, size: usize, data: *mut u8) {
        unsafe {
            CGL_ssbo_set_sub_data(self.handle, offset, size, data as *mut c_void, false as c_int);
        }
    }

    /// Gets the entire data of the Shader Storage Buffer Object.
    /// 
    /// If you only want to get a part of the data, use `get_sub_data` instead.
    /// 
    /// # Returns
    /// 
    /// A vector containing the data of the Shader Storage Buffer Object.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut ssbo = cgl_rs::graphics::ShaderStorageBufferObject::new(0).unwrap();
    ///     let data: [u8; 4] = [0, 1, 2, 3];
    ///     ssbo.set_data(data.len(), data.as_ptr() as *mut u8, true);
    ///     let retrieved_data = ssbo.get_data();
    ///     assert_eq!(data, retrieved_data.as_slice());
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    /// 
    /// # See also
    /// 
    /// * `get_sub_data`
    pub fn get_data(&self) -> Vec<u8> {
        unsafe {
            let mut size: usize = CGL_ssbo_get_size(self.handle);
            let mut data: Vec<u8> = Vec::with_capacity(size);
            CGL_ssbo_get_data(self.handle, &mut size, data.as_mut_ptr() as *mut c_void);
            data.set_len(size);
            data
        }
    }

    /// Gets a part of the data of the Shader Storage Buffer Object.
    /// 
    /// If you want to get the entire data, use `get_data` instead.
    /// 
    /// # Arguments
    /// 
    /// * `offset` - The offset in bytes from the beginning of the buffer where the data should be read.
    /// * `size` - The size of the data in bytes.
    /// 
    /// # Returns
    /// 
    /// A vector containing the requested part of the data of the Shader Storage Buffer Object.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut ssbo = cgl_rs::graphics::ShaderStorageBufferObject::new(0).unwrap();
    ///     let data: [u8; 4] = [0, 1, 2, 3];
    ///     ssbo.set_data(data.len(), data.as_ptr() as *mut u8, true);
    ///     let retrieved_data = ssbo.get_sub_data(1, 2);
    ///     assert_eq!(&data[1..3], retrieved_data.as_slice());
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    /// 
    /// # See also
    /// 
    /// * `get_data`
    pub fn get_sub_data(&self, offset: usize, size: usize) -> Vec<u8> {
        unsafe {
            let mut data: Vec<u8> = Vec::with_capacity(size);
            CGL_ssbo_get_sub_data(self.handle, offset, size, data.as_mut_ptr() as *mut c_void);
            data.set_len(size);
            data
        }
    }

    /// Gets the size of the Shader Storage Buffer Object.
    /// 
    /// # Returns
    /// 
    /// The size of the Shader Storage Buffer Object in bytes.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///    let mut ssbo = cgl_rs::graphics::ShaderStorageBufferObject::new(0).unwrap();
    ///    let data: [u8; 4] = [0, 1, 2, 3];
    ///    ssbo.set_data(data.len(), data.as_ptr() as *mut u8, true);
    ///    assert_eq!(data.len(), ssbo.size());
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn size(&self) -> usize {
        unsafe {
            CGL_ssbo_get_size(self.handle) as usize
        }
    }

    /// Copies a part of the data of the Shader Storage Buffer Object to another Shader Storage Buffer Object.
    /// 
    /// Note: This does not allocate the memory if the destination buffer is not big enough.
    ///       In such a case nothing will happen.
    /// 
    /// # Arguments
    /// 
    /// * `other` - The destination Shader Storage Buffer Object.
    /// * `src_offset` - The offset in bytes from the beginning of the source buffer where the data should be read.
    /// * `dst_offset` - The offset in bytes from the beginning of the destination buffer where the data should be written.
    /// * `size` - The size of the data in bytes.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut ssbo1 = cgl_rs::graphics::ShaderStorageBufferObject::new(0).unwrap();
    ///     let mut ssbo2 = cgl_rs::graphics::ShaderStorageBufferObject::new(0).unwrap();
    ///     let data: [u8; 4] = [0, 1, 2, 3];
    ///     let data2: [u8; 2] = [4, 5];
    ///     ssbo1.set_data(data.len(), data.as_ptr() as *mut u8, true);
    ///     ssbo2.set_data(data2.len(), data2.as_ptr() as *mut u8, true);
    ///     ssbo1.copy_to(&ssbo2, 1, 0, 2);
    ///     let retrieved_data = ssbo2.get_data();
    ///     assert_eq!(&data[1..3], retrieved_data.as_slice());
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn copy_to(&self, other: &ShaderStorageBufferObject, src_offset: usize, dst_offset: usize, size: usize) {
        unsafe {
            CGL_ssbo_copy(other.handle, self.handle, src_offset, dst_offset, size);
        }
    }

    /// Copies the entire data of the Shader Storage Buffer Object to another Shader Storage Buffer Object.
    /// 
    /// Note: This does not allocate the memory if the destination buffer is not big enough.
    ///       In such a case nothing will happen.
    /// 
    /// # Arguments
    /// 
    /// * `other` - The destination Shader Storage Buffer Object.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("Hello, World!", 800, 600).unwrap();
    /// cgl_rs::graphics::init();
    /// {
    ///     let mut ssbo1 = cgl_rs::graphics::ShaderStorageBufferObject::new(0).unwrap();
    ///     let mut ssbo2 = cgl_rs::graphics::ShaderStorageBufferObject::new(0).unwrap();
    ///     let data: [u8; 4] = [0, 1, 2, 3];
    ///     let data2: [u8; 4] = [4, 5, 6, 7];
    ///     ssbo1.set_data(data.len(), data.as_ptr() as *mut u8, true);
    ///     ssbo2.set_data(data2.len(), data2.as_ptr() as *mut u8, true);
    ///     ssbo1.copy_whole_to(&ssbo2);
    ///     let retrieved_data = ssbo2.get_data();
    ///     assert_eq!(data, retrieved_data.as_slice());
    /// }
    /// cgl_rs::graphics::shutdown();
    /// window.destroy();
    /// cgl_rs::shutdown();
    /// ```
    pub fn copy_whole_to(&self, other: &ShaderStorageBufferObject) {
        unsafe {
            CGL_ssbo_copy(other.handle, self.handle, 0, 0, self.size());
        }
    }


}

impl Drop for ShaderStorageBufferObject {
    fn drop(&mut self) {
        self.destroy();
    }
}


impl Clone for ShaderStorageBufferObject {
    /// Clones the ssbo.
    /// 
    /// NOTE: The new instance will have the same handle, `has_been_destroyed` flag.
    ///       This means that the new instance will not be able to receive events nor will the internal window handle be
    ///       destroyed when the new instance is dropped. The internal window handle will be destroyed when the original
    ///       instance is dropped.
    /// 
    /// 
    /// # Returns
    /// 
    /// A new instance of `ShaderStorageBufferObject` with the same handle as the original ssbo.
    fn clone(&self) -> Self {
        ShaderStorageBufferObject {
            handle: self.handle.clone(),
            has_been_destroyed: true
        }
    }
}