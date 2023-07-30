#![allow(non_camel_case_types)]
use libc::{c_void, c_int, c_char, c_float, c_uchar};

/// A struct representing a logger context (internal use only, c side)
#[repr(C)]
pub struct CGL_logger_context {
    _private: c_void
}

extern {
    fn CGL_console_set_color(color: c_uchar) -> c_void;

    fn CGL_printf_red(log_format: *const c_char, ...) -> c_void;
    fn CGL_printf_green(log_format: *const c_char, ...) -> c_void;
    fn CGL_printf_gray(log_format: *const c_char, ...) -> c_void;
    fn CGL_printf_blue(log_format: *const c_char, ...) -> c_void;
    fn CGL_console_progress_bar(progress: c_float, width: c_int, prefix: *const c_char, suffix: *const c_char, complete_char: c_char, incomplete_char: c_char) -> c_void;

    
    fn CGL_logger_init(enable_console_logging: c_int) -> c_void;
    fn CGL_logger_shutdown() -> c_void;
    fn CGL_logger_get_context() -> *const CGL_logger_context;
    fn CGL_logger_set_context(context: *const CGL_logger_context) -> c_void;
    fn CGL_logger_attach_log_file(log_file_path: *const c_char) -> c_int;
    fn CGL_logger_detach_log_file(log_file_path: *const c_char) -> c_int;
    fn CGL_logger_flush() -> c_void;
    fn CGL_logger_disable_console_logging() -> c_void;
    fn CGL_logger_enable_console_logging() -> c_void;
    fn CGL_logger_log(level: c_int, log_format: *const c_char, ...) -> c_void;    
}


/// An enum representing console colors.
#[repr(C)] #[derive(Debug)]
pub enum ConsoleColor {
    Reset = 0,
    Red   = 1,
    Green = 2,
    Gray  = 3,
    Blue  = 4
}

/// An enum representing log levels.
#[repr(C)] #[derive(Debug)]
pub enum LogLevel {
    Trace    = 0,
    Info     = 1,
    Warning  = 2,
    Error    = 3,
    Internal = 4
}

/// This is a temporary thing. This has already been implemented in the c side.
/// Just the API is not public yet, so I'm using this for now.
static mut AUTO_FLUSH_ON_LOG: bool = false;


/// Initializes the logger.
///
/// NOTE: This is automatically called when you call cgl_rs::init().
///       So it's not necessary to call this function manually, unless you are 
///       doing something special, like creating a seperate context for some dll or stuff.
/// 
/// # Arguments
///
/// * `enable_console_logging` - A boolean value indicating whether to enable console logging or not.
///
/// # Example
///
/// ```
/// cgl_rs::logger::init(true);
/// // ...
/// cgl_rs::logger::shutdown();
/// ```
pub fn init(enable_console_logging: bool) -> () {
    unsafe {
        CGL_logger_init(enable_console_logging as c_int);
    }
}

/// Shuts down the logger.
///
/// NOTE: This is automatically called when you call cgl_rs::shutdown().
/// 
/// # Example
///
/// ```
/// cgl_rs::logger::init(true);
/// // ...
/// cgl_rs::logger::shutdown();
/// ```
pub fn shutdown() -> () {
    unsafe {
        CGL_logger_shutdown();
    }
}

/// Returns a pointer to the logger context.
///
/// # Safety
///
/// This function is unsafe because it returns a raw pointer to the logger context.
///
/// # Example
///
/// ```
/// cgl_rs::logger::init(true);
/// let context = cgl_rs::logger::get_context();
/// // ...
/// cgl_rs::logger::shutdown();
/// ```
pub fn get_context() -> *const CGL_logger_context {
    unsafe {
        CGL_logger_get_context()
    }
}

/// Sets the logger context.
/// 
/// # Safety
/// 
/// This function is unsafe because it takes a raw pointer to the logger context.
/// 
/// # Arguments
/// 
/// * `context` - A raw pointer to the logger context.
/// 
/// # Example
/// 
/// ```
/// cgl_rs::logger::init(true);
/// let context = cgl_rs::logger::get_context();
/// cgl_rs::logger::set_context(context);
/// // ...
/// cgl_rs::logger::shutdown();
/// ```
pub fn set_context(context: *const CGL_logger_context) -> () {
    unsafe {
        CGL_logger_set_context(context);
    }
}


/// Attaches a log file to the logger. If the log file already exists, it will be appended to, otherwise it will be created.
/// Also note that there is a limit of 32 (may change in the future) log files that can be attached at the same time.
///
/// # Arguments
///
/// * `log_file_path` - A string slice containing the path to the log file.
///
/// # Returns
///
/// A boolean value indicating whether the log file was successfully attached or not.
///
/// # Example
///
/// ```
/// cgl_rs::logger::init(true);
/// let log_file_path = "log.txt";
/// let attached = cgl_rs::logger::attach_log_file(log_file_path);
/// // ...
/// cgl_rs::logger::shutdown();
/// ```
pub fn attach_log_file(log_file_path: &str) -> bool {
    let c_log_file_path = std::ffi::CString::new(log_file_path).unwrap();
    unsafe {
        CGL_logger_attach_log_file(c_log_file_path.as_ptr()) != 0
    }
}

/// Detaches a log file from the logger.
///
/// # Arguments
///
/// * `log_file_path` - A string slice containing the path to the log file.
///
/// # Returns
///
/// A boolean value indicating whether the log file was successfully detached or not.
///
/// # Example
///
/// ```
/// cgl_rs::logger::init(true);
/// let log_file_path = "log.txt";
/// cgl_rs::logger::attach_log_file(log_file_path);
/// let detached = cgl_rs::logger::detach_log_file(log_file_path);
/// // ...
/// cgl_rs::logger::shutdown();
/// ```
pub fn detach_log_file(log_file_path: &str) -> bool {
    let c_log_file_path = std::ffi::CString::new(log_file_path).unwrap();
    unsafe {
        CGL_logger_detach_log_file(c_log_file_path.as_ptr()) != 0
    }
}

/// Flushes all log messages to their respective log files.
///
/// # Example
///
/// ```
/// cgl_rs::logger::init(true);
/// // ...
/// cgl_rs::logger::flush();
/// // ...
/// cgl_rs::logger::shutdown();
/// ```
pub fn flush() -> () {
    unsafe {
        CGL_logger_flush();
    }
}
/// Disables console logging.
///
/// # Example
///
/// ```
/// cgl_rs::logger::init(true);
/// cgl_rs::logger::disable_console_logging();
/// // ...
/// cgl_rs::logger::shutdown();
/// ```
pub fn disable_console_logging() -> () {
    unsafe {
        CGL_logger_disable_console_logging();
    }
}

/// Enables console logging.
///
/// # Example
///
/// ```
/// cgl_rs::logger::init(true);
/// cgl_rs::logger::enable_console_logging();
/// // ...
/// cgl_rs::logger::shutdown();
/// ```
pub fn enable_console_logging() -> () {
    unsafe {
        CGL_logger_enable_console_logging();
    }
}

/// Sets whether the logger should automatically flush all log messages to their respective log files after each log message.
///
/// # Arguments
///
/// * `auto_flush_on_log` - A boolean value indicating whether the logger should automatically flush all log messages to their respective log files after each log message.
///
/// # Example
///
/// ```
/// cgl_rs::logger::init(true);
/// cgl_rs::logger::set_auto_flush_on_log(true);
/// // ...
/// cgl_rs::logger::shutdown();
/// ```
pub fn set_auto_flush_on_log(auto_flush_on_log: bool) -> () {
    unsafe {
        AUTO_FLUSH_ON_LOG = auto_flush_on_log;
    }
}


/// Logs a message with the specified log level.
///
/// # Arguments
///
/// * `level` - The log level of the message.
/// * `message` - A string slice containing the message to log.
///
/// # Example
///
/// ```
/// cgl_rs::logger::init(true);
/// cgl_rs::logger::log(cgl_rs::logger::LogLevel::Info, "This is an info message.");
/// // ...
/// cgl_rs::logger::shutdown();
/// ```
pub fn log(level: LogLevel, message: &str) -> () {
    let c_message = std::ffi::CString::new(message).unwrap();
    unsafe {
        CGL_logger_log(level as c_int, c_message.as_ptr());

        if AUTO_FLUSH_ON_LOG {
            flush();
        }
    }
}

/// Logs a trace message with the specified format string and arguments.
///
/// # Arguments
///
/// * `format` - A format string for the trace message.
/// * `args` - Arguments to be formatted into the trace message.
///
/// # Example
///
/// ```
/// cgl_rs::logger::init(true);
/// cgl_rs::log_trace!("This is a trace message with an argument: {}", 42);
/// // ...
/// cgl_rs::logger::shutdown();
/// ```
#[macro_export]
macro_rules! log_trace {
    ($($arg:tt)*) => ($crate::logger::log($crate::logger::LogLevel::Trace, &format!($($arg)*)));
}

/// Logs a warning message with the specified format string and arguments.
///
/// # Arguments
///
/// * `format` - A format string for the warning message.
/// * `args` - Arguments to be formatted into the warning message.
///
/// # Example
///
/// ```
/// cgl_rs::logger::init(true);
/// cgl_rs::log_warning!("This is a warning message with an argument: {}", 42);
/// // ...
/// cgl_rs::logger::shutdown();
/// ```
#[macro_export]
macro_rules! log_warning {
    ($($arg:tt)*) => ($crate::logger::log($crate::logger::LogLevel::Warning, &format!($($arg)*)));
}


/// Logs an error message with the specified format string and arguments.
/// 
/// # Arguments
/// 
/// * `format` - A format string for the error message.
/// * `args` - Arguments to be formatted into the error message.
/// 
/// # Example
/// 
/// ```
/// cgl_rs::logger::init(true);
/// cgl_rs::log_error!("This is an error message with an argument: {}", 42);
/// // ...
/// cgl_rs::logger::shutdown();
/// ```
#[macro_export]
macro_rules! log_error {
    ($($arg:tt)*) => ($crate::logger::log($crate::logger::LogLevel::Error, &format!($($arg)*)));
}


/// Logs an info message with the specified format string and arguments.
///
/// # Arguments
///
/// * `format` - A format string for the info message.
/// * `args` - Arguments to be formatted into the info message.
///
/// # Example
///
/// ```
/// cgl_rs::logger::init(true);
/// cgl_rs::log_info!("This is an info message with an argument: {}", 42);
/// // ...
/// cgl_rs::logger::shutdown();
/// ```
#[macro_export]
macro_rules! log_info {
    ($($arg:tt)*) => ($crate::logger::log($crate::logger::LogLevel::Info, &format!($($arg)*)));
}