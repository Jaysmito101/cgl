//! This module contains CGL utility functions, as well some miscellaneous functions.

#![allow(non_camel_case_types)]
use std::{ops::{Add, Sub, Mul, Div}, ffi::CString};

use libc::{c_void, c_int, c_char, c_float, size_t};

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

    fn CGL_utils_crc32(data: *const c_void, size: size_t) -> u32;
    fn CGL_utils_crc64(data: *const c_void, size: size_t) -> u64;
    fn CGL_utils_rot13(data_in: *const c_char, data_out: *mut c_char) -> c_void;
    fn CGL_utils_super_fast_hash(data: *const c_void, size: size_t) -> u32;

    /// These are originally CGL Macros but have been proxied here as functions.
    fn CGL_utils_random_float_MACRO() -> c_float;
    fn CGL_utils_random_float_in_range_MACRO(min_val: c_float, max_val: c_float) -> c_float;
    fn CGL_utils_random_int_MACRO(min_val: c_int, max_val: c_int) -> c_int;
    fn CGL_utils_random_bool_MACRO() -> c_int;
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
/// let success = cgl_rs::utils::write_string_to_file("log.txt", "Hello, world!");
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


/// Linearly interpolates between two values.
///
/// # Arguments
///
/// * `a` - A value of type `T` representing the start value.
/// * `b` - A value of type `T` representing the end value.
/// * `t` - A value of type `T` representing the interpolation factor.
///
/// # Returns
///
/// A value of type `T` representing the interpolated value.
///
/// # Example
///
/// ```
/// let a = 0f32;
/// let b = 10f32;
/// let t = 0.5f32;
/// let result = cgl_rs::utils::lerp(a, b, t);
/// ```
pub fn lerp<T: Copy + Add<Output = T> + Sub<Output = T> + Mul<Output = T>>(a: T, b: T, t: T) -> T {
    a + (b - a) * t
}



/// Maps a value from one range to another.
///
/// # Arguments
///
/// * `value` - A value of type `T` representing the value to map.
/// * `start1` - A value of type `T` representing the start of the input range.
/// * `stop1` - A value of type `T` representing the end of the input range.
/// * `start2` - A value of type `T` representing the start of the output range.
/// * `stop2` - A value of type `T` representing the end of the output range.
///
/// # Returns
///
/// A value of type `T` representing the mapped value.
///
/// # Example
///
/// ```
/// let value = 5;
/// let start1 = 0;
/// let stop1 = 10;
/// let start2 = 0;
/// let stop2 = 100;
/// let result = cgl_rs::utils::map(value, start1, stop1, start2, stop2);
/// ```
pub fn map<T: Copy + Add<Output = T> + Sub<Output = T> + Mul<Output = T> + Div<Output = T>>(value: T, start1: T, stop1: T, start2: T, stop2: T) -> T {
    start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1))
}

/// Calculates the CRC32 checksum of the given data.
/// 
/// The C implementation is based on linux's crc32 implementation.
///
/// # Arguments
///
/// * `data` - A slice of bytes representing the data to calculate the checksum for.
///
/// # Returns
///
/// A `u32` representing the calculated CRC32 checksum.
///
/// # Example
///
/// ```
/// let data = b"hello world";
/// let crc32_checksum = cgl_rs::utils::crc32(data);
/// assert_eq!(crc32_checksum, 222957957);
/// ```
pub fn crc32(data: &[u8]) -> u32 {
    unsafe {
        CGL_utils_crc32(data.as_ptr() as *const c_void, data.len() as size_t) as u32
    }
}

// Calculates the CRC64 checksum of the given data.
///
/// The C implementation is based on linux's crc64 implementation.
/// 
/// # Arguments
///
/// * `data` - A slice of bytes representing the data to calculate the checksum for.
///
/// # Returns
///
/// A `u64` representing the calculated CRC64 checksum.
///
/// # Example
///
/// ```
/// let data = b"hello world";
/// let crc64_checksum = cgl_rs::utils::crc64(data);
/// assert_eq!(crc64_checksum, 14017174577685478301);
/// ```
pub fn crc64(data: &[u8]) -> u64 {
    unsafe {
        CGL_utils_crc64(data.as_ptr() as *const c_void, data.len() as size_t) as u64
    }
}

/// Applies the ROT13 substitution cipher to the given string.
///
/// # Arguments
///
/// * `data` - A string slice representing the data to apply the ROT13 cipher to.
///
/// # Returns
///
/// A `String` representing the result of applying the ROT13 cipher to the input string.
///
/// # Safety
///
/// This function is marked as unsafe because it calls an external C function that takes a raw pointer as an argument.
///
/// # Example
///
/// ```
/// let data = "hello world";
/// let result = cgl_rs::utils::rot13(data);
/// assert_eq!(result, "uryyb jbeyq\0");
/// ```
pub fn rot13(data: &str) -> String {
    unsafe {
        let mut result = vec![0u8; data.len() + 1];
        let c_str_data = CString::new(data).unwrap();
        CGL_utils_rot13(c_str_data.as_ptr(), result.as_mut_ptr() as *mut c_char);
        String::from_utf8_unchecked(result)
    }
}

/// Calculates the SuperFastHash of the given data.
/// 
/// This algorithm is from http://www.azillionmonkeys.com/qed/hash.html
/// 
/// # Arguments
/// 
/// * `data` - A slice of bytes representing the data to calculate the SuperFastHash for.
/// 
/// # Returns
/// 
/// A `u32` representing the calculated SuperFastHash.
/// 
/// # Example
/// 
/// ```
/// let data = b"hello world";
/// let super_fast_hash = cgl_rs::utils::super_fast_hash(data);
/// assert_eq!(super_fast_hash, 2794219650);
/// ```
pub fn super_fast_hash(data: &[u8]) -> u32 {
    unsafe {
        CGL_utils_super_fast_hash(data.as_ptr() as *const c_void, data.len() as size_t) as u32
    }
}


/// Generates a random floating-point number between 0 and 1.
///
/// # Returns
///
/// A `f32` representing the generated random number.
///
/// # Example
///
/// ```
/// let random_number = cgl_rs::utils::random_float();
/// ```
pub fn random_float() -> f32 {
    unsafe {
        CGL_utils_random_float_MACRO()
    }
}

/// Generates a random floating-point number within the given range.
///
/// # Arguments
///
/// * `min` - A `f32` representing the minimum value of the range.
/// * `max` - A `f32` representing the maximum value of the range.
///
/// # Returns
///
/// A `f32` representing the generated random number within the given range.
///
/// # Example
///
/// ```
/// let random_number = cgl_rs::utils::random_float_range(0.0, 1.0);
/// ```
pub fn random_float_range(min: f32, max: f32) -> f32 {
    unsafe {
        CGL_utils_random_float_in_range_MACRO(min as f32, max as f32) as f32
    }
}

/// Generates a random integer within the given range.
///
/// # Arguments
///
/// * `min_value` - An `i32` representing the minimum value of the range.
/// * `max_value` - An `i32` representing the maximum value of the range.
///
/// # Returns
///
/// An `i32` representing the generated random number within the given range.
///
/// # Example
///
/// ```
/// let random_number = cgl_rs::utils::random_int(0, 10);
/// ```
pub fn random_int(min_value: i32, max_value: i32) -> i32 {
    unsafe {
        CGL_utils_random_int_MACRO(min_value as i32, max_value as i32) as i32
    }
}