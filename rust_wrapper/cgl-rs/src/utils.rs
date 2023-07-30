//! This module contains CGL utility functions, as well some miscellaneous functions.


#![allow(non_camel_case_types)]
use libc::{c_void, c_int, c_char, c_float, c_uchar, size_t};

extern {
    fn CGL_utils_sleep(milis: size_t) -> c_void;
    fn CGL_utils_read_file(path: *const c_char, size: *mut size_t) -> *mut c_char;
    fn CGL_utils_get_file_size(path: *const c_char) -> size_t;
    fn CGL_utils_append_file(path: *const c_char, data: *const c_char, size: size_t) -> c_int;
    fn CGL_utils_write_file(path: *const c_char, data: *const c_char, size: size_t) -> c_int;
    fn CGL_utils_get_time() -> c_float;
    fn CGL_utils_get_timestamp(buffer: *mut c_char) -> c_void;
    fn CGL_utils_is_little_endian() -> c_int;
    fn CGL_utils_get_random_with_probability(probabilities: *mut c_float, count: size_t) -> size_t;
    fn CGL_utils_reverse_bytes(data: *mut c_void, size: size_t) -> c_void;
    fn CGL_utils_little_endian_to_current(data: *mut c_void, size: size_t) -> c_void;
    fn CGL_utils_big_endian_to_current(data: *mut c_void, size: size_t) -> c_void;
    fn CGL_utils_fast_srand(seed: c_int) -> c_void;
    fn CGL_utils_fast_rand() -> c_int;
    fn CGL_utils_xorshf96() -> u64;
    fn CGL_utils_srand31(seed: u32) -> c_void;
    fn CGL_utils_rand31() -> u32;

    fn CGL_utils_sigmoid(x: c_float) -> c_float;
    fn CGL_utils_sigmoid_derivative(x: c_float) -> c_float;
    fn CGL_utils_relu(x: c_float) -> c_float;
    fn CGL_utils_relu_derivative(x: c_float) -> c_float;
    fn CGL_utils_tanh(x: c_float) -> c_float;
    fn CGL_utils_tanh_derivative(x: c_float) -> c_float;
    fn CGL_utils_step(x: c_float) -> c_float;
    fn CGL_utils_step_derivative(x: c_float) -> c_float;
    fn CGL_utils_relu_leaky(x: c_float) -> c_float;
    fn CGL_utils_relu_leaky_derivative(x: c_float) -> c_float;
    fn CGL_utils_relu_smooth(x: c_float) -> c_float;
    fn CGL_utils_relu_smooth_derivative(x: c_float) -> c_float;
}


/// Suspends the current thread for the specified amount of time.
///
/// # Arguments
///
/// * `milis` - The amount of time to sleep, in milliseconds.
/// 
/// # Example
/// 
/// ```
/// cgl_rs::utils::sleep(1000);
/// ```
pub fn sleep(milis: size_t) -> () {
    unsafe {
        CGL_utils_sleep(milis);
    }
}


/// Reads the contents of a file and returns it as a vector of bytes.
///
/// # Arguments
///
/// * `path` - The path to the file to read.
/// * `size` - A pointer to a size_t variable that will be set to the size of the file.
///
/// # Example
///
/// ```
/// let (size, contents) = cgl_rs::utils::read_file("log.txt");
/// ```
pub fn read_file(path: &str) -> (usize, Vec<i8>) {
    unsafe {
        let mut size: size_t = 0;
        let path = std::ffi::CString::new(path).unwrap();
        let data = CGL_utils_read_file(path.as_ptr(), &mut size);
        let mut vec = Vec::new();
        for i in 0..size {
            vec.push(*data.offset(i as isize));
        }
        libc::free(data as *mut c_void);
        (size as usize, vec)
    }
}


/// Reads the contents of a file and returns it as a string.
///
/// # Arguments
///
/// * `path` - The path to the file to read.
///
/// # Example
///
/// ```
/// let contents = cgl_rs::utils::read_file_as_string("log.txt");
/// ```
pub fn read_file_as_string(path: &str) -> String {
    let (size, data) = read_file(path);
    let mut string = String::new();
    for i in 0..size {
        string.push(data[i] as u8 as char);
    }
    string
}

/// Returns the size of a file in bytes.
///
/// # Arguments
///
/// * `path` - The path to the file to get the size of.
///
/// # Example
///
/// ```norun
/// let path = "path/to/file";
/// let size = cgl_rs::utils::get_file_size(path);
/// ```
pub fn get_file_size(path: &str) -> usize {
    unsafe {
        let path = std::ffi::CString::new(path).unwrap();
        CGL_utils_get_file_size(path.as_ptr()) as usize
    }
}


// Appends data to a file.
///
/// # Arguments
///
/// * `path` - The path to the file to append data to.
/// * `data` - A slice of bytes to append to the file.
/// * `size` - The size of the data slice.
///
/// # Example
///
/// ```
/// let data = [1, 2, 3];
/// let success = cgl_rs::utils::append_file("log.txt", &data, 3);
/// ```
pub fn append_file(path: &str, data: &[u8], size: usize) -> bool {
    unsafe {
        let path = std::ffi::CString::new(path).unwrap();
        CGL_utils_write_file(path.as_ptr(), data.as_ptr() as *const c_char, size as size_t) != 0
    }
}


/// Appends a string to a file.
///
/// # Arguments
///
/// * `path` - The path to the file to append the string to.
/// * `data` - The string to append to the file.
///
/// # Example
///
/// ```
/// let success = cgl_rs::utils::append_string_to_file("log.txt", "Hello, world!");
/// ```
pub fn append_string_to_file(path: &str, data: &str) -> bool {
    append_file(path, data.as_bytes(), data.len())
}


// Writes data to a file.
///
/// # Arguments
///
/// * `path` - The path to the file to write data to.
/// * `data` - A slice of bytes to write to the file.
/// * `size` - The size of the data slice.
///
/// # Example
///
/// ```
/// let data = [1, 2, 3];
/// let success = cgl_rs::utils::write_file("log.txt", &data, 3);
/// ```
pub fn write_file(path: &str, data: &[u8], size: usize) -> bool {
    unsafe {
        let path = std::ffi::CString::new(path).unwrap();
        CGL_utils_write_file(path.as_ptr(), data.as_ptr() as *const c_char, size as size_t) != 0
    }
}

/// Writes a string to a file.
///
/// # Arguments
///
/// * `path` - The path to the file to write the string to.
/// * `data` - The string to write to the file.
///
/// # Example
///
/// ```
/// let success = cgl_rs::utils::write_string_to_file("log.txt", "Hello, world!", 13);
/// ```
pub fn write_string_to_file(path: &str, data: &str) -> bool {
    write_file(path, data.as_bytes(), data.len())
}

/// Returns the time in seconds since the program started.
///
/// # Returns
///
/// * A `f32` representing the current time in seconds.
///
/// # Example
///
/// ```
/// let time = cgl_rs::utils::get_time();
/// ```
pub fn get_time() -> f32 {
    unsafe {
        CGL_utils_get_time() as f32
    }
}

/// Returns a string representing the current timestamp.
///
/// # Returns
///
/// * A `String` representing the current timestamp.
///
/// # Example
///
/// ```
/// let timestamp = cgl_rs::utils::get_timestamp();
/// ```
pub fn get_timestamp() -> String {
    unsafe {
        let mut buffer = [0 as c_char; 128];
        CGL_utils_get_timestamp(buffer.as_mut_ptr());
        std::ffi::CStr::from_ptr(buffer.as_ptr()).to_str().unwrap().to_string()
    }
}

/// Returns a boolean indicating whether the system is little-endian or not.
///
/// # Returns
///
/// * A `bool` indicating whether the system is little-endian or not.
///
/// # Example
///
/// ```
/// let is_little_endian = cgl_rs::utils::is_little_endian();
/// ```
pub fn is_little_endian() -> bool {
    unsafe {
        CGL_utils_is_little_endian() != 0
    }
}

/// Returns a random index from the given slice of probabilities, where the probability of each index being chosen is proportional to its value in the slice.
///
/// # Arguments
///
/// * `probabilities` - A mutable slice of `f32` values representing the probabilities of each index being chosen.
///
/// # Returns
///
/// * A `usize` representing the randomly chosen index.
///
/// # Example
///
/// ```
/// let mut probabilities = vec![0.1, 0.2, 0.3, 0.4];
/// let index = cgl_rs::utils::get_random_with_probability(&mut probabilities);
/// ```
pub fn get_random_with_probability(probabilities: &mut [f32]) -> usize {
    unsafe {
        CGL_utils_get_random_with_probability(probabilities.as_mut_ptr(), probabilities.len() as size_t)
    }
}

///
/// Reverses the order of bytes in the given slice of `u8` values.
///
/// # Arguments
///
/// * `data` - A mutable slice of `u8` values to reverse.
///
/// # Example
///
/// ```
/// let mut data = vec![0x12, 0x34, 0x56, 0x78];
/// cgl_rs::utils::reverse_bytes(&mut data);
/// assert_eq!(data, vec![0x78, 0x56, 0x34, 0x12]);
/// ```
pub fn reverse_bytes(data: &mut [u8]) -> () {
    unsafe {
        CGL_utils_reverse_bytes(data.as_mut_ptr() as *mut c_void, data.len() as size_t);
    }
}

/// Converts a slice of `u8` values from little-endian byte order to the system's native byte order.
///
/// # Arguments
///
/// * `data` - A mutable slice of `u8` values to convert.
///
/// # Example
///
/// ```
/// let mut data = vec![0x78, 0x56, 0x34, 0x12];
/// cgl_rs::utils::little_endian_to_current(&mut data);
/// ```
pub fn little_endian_to_current(data: &mut [u8]) -> () {
    unsafe {
        CGL_utils_little_endian_to_current(data.as_mut_ptr() as *mut c_void, data.len() as size_t);
    }
}

/// Converts a slice of `u8` values from big-endian byte order to the system's native byte order.
///
/// # Arguments
///
/// * `data` - A mutable slice of `u8` values to convert.
///
/// # Example
///
/// ```
/// let mut data = vec![0x12, 0x34, 0x56, 0x78];
/// cgl_rs::utils::big_endian_to_current(&mut data);
/// ```
pub fn big_endian_to_current(data: &mut [u8]) -> () {
    unsafe {
        CGL_utils_big_endian_to_current(data.as_mut_ptr() as *mut c_void, data.len() as size_t);
    }
}

/// Sets the seed for the fast random number generator.
///
/// # Arguments
///
/// * `seed` - An `i32` representing the seed value to set.
///
/// # Example
///
/// ```
/// cgl_rs::utils::fast_srand(42);
/// ```
pub fn fast_srand(seed: i32) -> () {
    unsafe {
        CGL_utils_fast_srand(seed);
    }
}

/// Generates a fast random number.
///
/// # Returns
///
/// An `i32` representing the generated random number.
///
/// # Example
///
/// ```
/// let random_number = cgl_rs::utils::fast_rand();
/// ```
pub fn fast_rand() -> i32 {
    unsafe {
        CGL_utils_fast_rand()
    }
}

/// Generates a fast random number using the xorshf96 algorithm.
/// 
/// # Returns
/// 
/// A `u64` representing the generated random number.
/// 
/// # Example
/// 
/// ```
/// let random_number = cgl_rs::utils::xorshf96();
/// ```
pub fn xorshf96() -> u64 {
    unsafe {
        CGL_utils_xorshf96()
    }
}



/// Sets the seed for the rand31 random number generator.
///
/// # Arguments
///
/// * `seed` - A `u32` representing the seed value to set.
///
/// # Example
///
/// ```
/// cgl_rs::utils::srand31(42);
/// ```
pub fn srand31(seed: u32) -> () {
    unsafe {
        CGL_utils_srand31(seed);
    }
}

/// Generates a random number using the rand31 algorithm.
///
/// # Returns
///
/// A `u32` representing the generated random number.
///
/// # Example
///
/// ```
/// let random_number = cgl_rs::utils::rand31();
/// ```
pub fn rand31() -> u32 {
    unsafe {
        CGL_utils_rand31()
    }
}
