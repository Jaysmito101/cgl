//! The window manager module is responsible for creating and managing windows. It is also responsible for handling input events and passing them to the appropriate window.

#![allow(non_camel_case_types)]
use libc::{c_void, c_int, c_char, c_double};

/// The internal window handle used by CGL
#[repr(C)]
pub(crate) struct CGL_window {
    pub(crate) _private: c_void
}

/// The internal window handle used by GLFW
#[repr(C)]
pub struct GLFWwindow {
    _private: c_void
}

type CGL_window_key_callback = extern "C" fn(window: *mut CGL_window, key: c_int, scancode: c_int, action: c_int, mods: c_int) -> c_void;
type CGL_window_mouse_button_callback = extern "C" fn(window: *mut CGL_window, button: c_int, action: c_int, mods: c_int) -> c_void;
type CGL_window_mouse_position_callback = extern "C" fn(window: *mut CGL_window, x: c_double, y: c_double) -> c_void;
type CGL_window_mouse_scroll_callback = extern "C" fn(window: *mut CGL_window, x: c_double, y: c_double) -> c_void;
type CGL_window_framebuffer_size_callback = extern "C" fn(window: *mut CGL_window, width: c_int, height: c_int) -> c_void;
type CGL_window_close_callback = extern "C" fn(window: *mut CGL_window) -> c_void;
type CGL_window_drag_n_drop_callback = extern "C" fn(window: *mut CGL_window, paths: *const *const c_char, count: c_int) -> c_void;




extern {
    fn CGL_window_create(width: c_int, height: c_int, title: *const c_char) -> *mut CGL_window;
    fn CGL_window_create_undecorated(width: c_int, height: c_int, title: *const c_char) -> *mut CGL_window;
    fn CGL_window_destroy(window: *mut CGL_window) -> c_void;
    fn CGL_window_poll_events(window: *mut CGL_window) -> c_void;
    fn CGL_window_swap_buffers(window: *mut CGL_window) -> c_void;
    fn CGL_window_should_close(window: *mut CGL_window) -> c_int;
    fn CGL_window_set_title(window: *mut CGL_window, title: *const c_char) -> c_void;
    fn CGL_window_set_size(window: *mut CGL_window, width: c_int, height: c_int) -> c_void;
    fn CGL_window_set_position(window: *mut CGL_window, x: c_int, y: c_int) -> c_void;
    fn CGL_window_set_hidden(window: *mut CGL_window, hidden: c_int) -> c_void;
    fn CGL_window_set_user_data(window: *mut CGL_window, user_data: *mut c_void) -> c_void;
    fn CGL_window_get_user_data(window: *mut CGL_window) -> *mut c_void;
    fn CGL_window_get_size(window: *mut CGL_window, width: *mut c_int, height: *mut c_int) -> c_void;
    fn CGL_window_get_position(window: *mut CGL_window, x: *mut c_int, y: *mut c_int) -> c_void;
    fn CGL_window_get_framebuffer_size(window: *mut CGL_window, width: *mut c_int, height: *mut c_int) -> c_void;
    
    fn CGL_window_set_key_callback(window: *mut CGL_window, callback: CGL_window_key_callback) -> c_void;
    fn CGL_window_set_mouse_button_callback(window: *mut CGL_window, callback: CGL_window_mouse_button_callback) -> c_void;
    fn CGL_window_set_mouse_position_callback(window: *mut CGL_window, callback: CGL_window_mouse_position_callback) -> c_void;
    fn CGL_window_set_mouse_scroll_callback(window: *mut CGL_window, callback: CGL_window_mouse_scroll_callback) -> c_void;
    fn CGL_window_set_framebuffer_size_callback(window: *mut CGL_window, callback: CGL_window_framebuffer_size_callback) -> c_void;
    fn CGL_window_set_close_callback(window: *mut CGL_window, callback: CGL_window_close_callback) -> c_void;
    fn CGL_window_set_drag_n_drop_callback(window: *mut CGL_window, callback: CGL_window_drag_n_drop_callback) -> c_void;

    fn CGL_window_resecure_callbacks(window: *mut CGL_window) -> c_void;
    fn CGL_window_make_context_current(window: *mut CGL_window) -> c_void;
    fn CGL_window_get_glfw_handle(window: *mut CGL_window) -> *mut GLFWwindow;

    fn CGL_window_get_key(window: *mut CGL_window, key: c_int) -> c_int;
    fn CGL_window_is_key_pressed(window: *mut CGL_window, key: c_int) -> c_int;
    fn CGL_window_get_mouse_button(window: *mut CGL_window, button: c_int) -> c_int;
    fn CGL_window_get_mouse_position(window: *mut CGL_window, x: *mut c_double, y: *mut c_double) -> c_void;

    fn CGL_utils_sleep(duration: usize) -> c_void; // temporary
}


/// Represents a keyboard key, with each variant corresponding to a specific key code.
#[repr(C)] #[derive(Debug)]
pub enum Key {
    Unknown            = -1,
    Space              = 32,
    Apostrophe         = 39,
    Comma              = 44,
    Minus              = 45,
    Period             = 46,
    Slash              = 47,
    K0                 = 48,
    K1                 = 49,
    K2                 = 50,
    K3                 = 51,
    K4                 = 52,
    K5                 = 53,
    K6                 = 54,
    K7                 = 55,
    K8                 = 56,
    K9                 = 57,
    Semicolon          = 59,
    Equal              = 61,
    A                  = 65,
    B                  = 66,
    C                  = 67,
    D                  = 68,
    E                  = 69,
    F                  = 70,
    G                  = 71,
    H                  = 72,
    I                  = 73,
    J                  = 74,
    K                  = 75,
    L                  = 76,
    M                  = 77,
    N                  = 78,
    O                  = 79,
    P                  = 80,
    Q                  = 81,
    R                  = 82,
    S                  = 83,
    T                  = 84,
    U                  = 85,
    V                  = 86,
    W                  = 87,
    X                  = 88,
    Y                  = 89,
    Z                  = 90,
    LeftBracket        = 91,
    Backslash          = 92,
    RightBracket       = 93,
    GraveAccent        = 96,
    Escape             = 256,
    Enter              = 257,
    Tab                = 258,
    Backspace          = 259,
    Insert             = 260,
    Delete             = 261,
    Right              = 262,
    Left               = 263,
    Down               = 264,
    Up                 = 265,
    PageUp             = 266,
    PageDown           = 267,
    Home               = 268,
    End                = 269,
    CapsLock           = 280,
    ScrollLock         = 281,
    NumLock            = 282,
    PrintScreen        = 283,
    Pause              = 284,
    F1                 = 290,
    F2                 = 291,
    F3                 = 292,
    F4                 = 293,
    F5                 = 294,
    F6                 = 295,
    F7                 = 296,
    F8                 = 297,
    F9                 = 298,
    F10                = 299,
    F11                = 300,
    F12                = 301,
    F13                = 302,
    F14                = 303,
    F15                = 304,
    F16                = 305,
    F17                = 306,
    F18                = 307,
    F19                = 308,
    F20                = 309,
    F21                = 310,
    F22                = 311,
    F23                = 312,
    F24                = 313,
    F25                = 314,
    KeyPad0            = 320,
    KeyPad1            = 321,
    KeyPad2            = 322,
    KeyPad3            = 323,
    KeyPad4            = 324,
    KeyPad5            = 325,
    KeyPad6            = 326,
    KeyPad7            = 327,
    KeyPad8            = 328,
    KeyPad9            = 329,
    KeyPadDecimal      = 330,
    KeyPadDivide       = 331,
    KeyPadMultiply     = 332,
    KeyPadSubtract     = 333,
    KeyPadAdd          = 334,
    KeyPadEnter        = 335,
    KeyPadEqual        = 336,
    LeftShift          = 340,
    LeftControl        = 341,
    LeftAlt            = 342,
    LeftSuper          = 343,
    RightShift         = 344,
    RightControl       = 345,
    RightAlt           = 346,
    RightSuper         = 347,
    Menu               = 348
}

/// Represents a mouse button, with each variant corresponding to a specific button code.
#[derive(Debug)]
pub enum MouseButton {
    Left       = 0,
    Right      = 1,
    Middle     = 2,
    B4         = 3,
    B5         = 4,
    B6         = 5,
    B7         = 6,
    B8         = 7,
    Last       = 8
}

/// Represents an action, with each variant corresponding to a specific action code.
#[derive(Debug, PartialEq)]
pub enum Action {
    Release        = 0,
    Press          = 1,
    Repeat         = 2
}

/// Represents an event that can be received by a window, with each variant corresponding to a specific type of event.
#[derive(Debug)]
pub enum Event {
    Key(Key, Action, i32, i32),
    MouseButton(MouseButton, Action, i32),
    MousePosition(f64, f64),
    MouseScroll(f64, f64),
    FramebufferSize(i32, i32),
    WindowClose,
    DragNDrop(Vec<String>)
}

/// Represents a function that handles a window event.
/// Takes a reference to the window and the event, and returns a boolean indicating whether the event was handled successfully.
/// If this function returns false, the event will be passed to the next handler in the chain, or if there are no more handlers, the event will be ignored.
/// If this function returns true, the event will not be passed to any more handlers.
pub type EventFunction = dyn Fn(&Window, &Event) -> bool;

/// Represents a window, with a handle to the underlying CGL window and a flag indicating whether it has been destroyed.
pub struct Window {
    handle: *mut CGL_window,
    has_been_destroyed: bool,
    event_handlers: std::collections::HashMap<String, Box<EventFunction>>
}

const MAX_ACTIVE_WINDOWS: usize = 1024;
static mut ACTIVE_WINDOWS: [*const Window; MAX_ACTIVE_WINDOWS] = [0 as *const Window; MAX_ACTIVE_WINDOWS];

// this is just a temporary hack, will be replaced with a proper solution later
extern "C" fn fn_returning_c_void() -> c_void {
    unsafe { CGL_utils_sleep(1) }
}

fn cgl_window_i32_to_action(action: i32) -> Action {
    match action {
        0 => Action::Release,
        1 => Action::Press,
        2 => Action::Repeat,
        _ => panic!("Invalid action code")
    }
}

fn cgl_window_i32_to_key(key: i32) -> Key {
    match key {
        32 => Key::Space,
        39 => Key::Apostrophe,
        44 => Key::Comma,
        45 => Key::Minus,
        46 => Key::Period,
        47 => Key::Slash,
        48 => Key::K0,
        49 => Key::K1,
        50 => Key::K2,
        51 => Key::K3,
        52 => Key::K4,
        53 => Key::K5,
        54 => Key::K6,
        55 => Key::K7,
        56 => Key::K8,
        57 => Key::K9,
        59 => Key::Semicolon,
        61 => Key::Equal,
        65 => Key::A,
        66 => Key::B,
        67 => Key::C,
        68 => Key::D,
        69 => Key::E,
        70 => Key::F,
        71 => Key::G,
        72 => Key::H,
        73 => Key::I,
        74 => Key::J,
        75 => Key::K,
        76 => Key::L,
        77 => Key::M,
        78 => Key::N,
        79 => Key::O,
        80 => Key::P,
        81 => Key::Q,
        82 => Key::R,
        83 => Key::S,
        84 => Key::T,
        85 => Key::U,
        86 => Key::V,
        87 => Key::W,
        88 => Key::X,
        89 => Key::Y,
        90 => Key::Z,
        91 => Key::LeftBracket,
        92 => Key::Backslash,
        93 => Key::RightBracket,
        96 => Key::GraveAccent,
        256 => Key::Escape,
        257 => Key::Enter,
        258 => Key::Tab,
        259 => Key::Backspace,
        260 => Key::Insert,
        261 => Key::Delete,
        262 => Key::Right,
        263 => Key::Left,
        264 => Key::Down,
        265 => Key::Up,
        266 => Key::PageUp,
        267 => Key::PageDown,
        268 => Key::Home,
        269 => Key::End,
        280 => Key::CapsLock,
        281 => Key::ScrollLock,
        282 => Key::NumLock,
        283 => Key::PrintScreen,
        284 => Key::Pause,
        290 => Key::F1,
        291 => Key::F2,
        292 => Key::F3,
        293 => Key::F4,
        294 => Key::F5,
        295 => Key::F6,
        296 => Key::F7,
        297 => Key::F8,
        298 => Key::F9,
        299 => Key::F10,
        300 => Key::F11,
        301 => Key::F12,
        302 => Key::F13,
        303 => Key::F14,
        304 => Key::F15,
        305 => Key::F16,
        306 => Key::F17,
        307 => Key::F18,
        308 => Key::F19,
        309 => Key::F20,
        310 => Key::F21,
        311 => Key::F22,
        312 => Key::F23,
        313 => Key::F24,
        314 => Key::F25,
        320 => Key::KeyPad0,
        321 => Key::KeyPad1,
        322 => Key::KeyPad2,
        323 => Key::KeyPad3,
        324 => Key::KeyPad4,
        325 => Key::KeyPad5,
        326 => Key::KeyPad6,
        327 => Key::KeyPad7,
        328 => Key::KeyPad8,
        329 => Key::KeyPad9,
        330 => Key::KeyPadDecimal,
        331 => Key::KeyPadDivide,
        332 => Key::KeyPadMultiply,
        333 => Key::KeyPadSubtract,
        334 => Key::KeyPadAdd,
        335 => Key::KeyPadEnter,
        336 => Key::KeyPadEqual,
        340 => Key::LeftShift,
        341 => Key::LeftControl,
        342 => Key::LeftAlt,
        343 => Key::LeftSuper,
        344 => Key::RightShift,
        345 => Key::RightControl,
        346 => Key::RightAlt,
        347 => Key::RightSuper,
        348 => Key::Menu,
        _ => panic!("Invalid key code")
    }
}

fn cgl_window_i32_to_mouse_button(button: i32) -> MouseButton {
    match button {
        0 => MouseButton::Left,
        1 => MouseButton::Right,
        2 => MouseButton::Middle,
        3 => MouseButton::B4,
        4 => MouseButton::B5,
        5 => MouseButton::B6,
        6 => MouseButton::B7,
        7 => MouseButton::B8,
        _ => panic!("Invalid mouse button code")
    }
}

fn cgl_window_dispatch_event(window: *mut CGL_window, event: Event) {
    unsafe {
        let window_id = (window as usize) % MAX_ACTIVE_WINDOWS;
        if ACTIVE_WINDOWS[window_id].is_null() {
            return;
        }
        let window = &*ACTIVE_WINDOWS[window_id];
        for (_, handler) in &window.event_handlers {
            if handler(window, &event) {
                break;
            }
        }
    }    
}

extern "C" fn cgl_window_key_callback(window: *mut CGL_window, key: i32, scancode: i32, action: i32, mods: i32) -> c_void {
    cgl_window_dispatch_event(window, Event::Key(cgl_window_i32_to_key(key), cgl_window_i32_to_action(action), scancode, mods));
    fn_returning_c_void()
}

extern "C" fn cgl_window_mouse_button_callback(window: *mut CGL_window, button: i32, action: i32, mods: i32) -> c_void {
    cgl_window_dispatch_event(window, Event::MouseButton(cgl_window_i32_to_mouse_button(button), cgl_window_i32_to_action(action), mods));
    fn_returning_c_void()
}

extern "C" fn cgl_window_mouse_position_callback(window: *mut CGL_window, xpos: f64, ypos: f64) -> c_void {
    cgl_window_dispatch_event(window, Event::MousePosition(xpos, ypos));
    fn_returning_c_void()
}

extern "C" fn cgl_window_mouse_scroll_callback(window: *mut CGL_window, xoffset: f64, yoffset: f64) -> c_void {
    cgl_window_dispatch_event(window, Event::MouseScroll(xoffset, yoffset));
    fn_returning_c_void()
}

extern "C" fn cgl_window_framebuffer_size_callback(window: *mut CGL_window, width: i32, height: i32) -> c_void {
    cgl_window_dispatch_event(window, Event::FramebufferSize(width, height));
    fn_returning_c_void()
}

extern "C" fn cgl_window_close_callback(window: *mut CGL_window) -> c_void {
    cgl_window_dispatch_event(window, Event::WindowClose);
    fn_returning_c_void()
}

extern "C" fn cgl_window_drag_n_drop_callback(window: *mut CGL_window, paths: *const *const c_char, count: i32) -> c_void {
    let mut paths_vec = Vec::new();
    for i in 0..count {
        unsafe {
            let path = std::ffi::CStr::from_ptr(*paths.offset(i as isize)).to_str().unwrap();
            paths_vec.push(path.to_string());
        }
    }
    cgl_window_dispatch_event(window, Event::DragNDrop(paths_vec));
    fn_returning_c_void()
}

impl Window {
    /// Creates a new window with the given title, width, and height.
    ///
    /// # Arguments
    ///
    /// * `title` - The title of the window.
    /// * `width` - The width of the window.
    /// * `height` - The height of the window.
    ///
    /// # Returns
    ///
    /// Returns a `Result` containing the created `Window` if successful, or an error message if the window creation failed.
    ///
    /// # Panics
    ///
    /// Panics if the width or height is not positive.
    ///
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
    /// window.destroy(); // we must destroy the window before shutting down
    /// cgl_rs::shutdown();
    /// ```
    pub fn new(title: &str, width: i32, height: i32) -> Result<Window, &'static str> {
        assert!(width > 0 && height > 0, "Window dimensions must be positive");
        let title = std::ffi::CString::new(title).unwrap();
        let handle = unsafe {
            let created_window = CGL_window_create(width, height, title.as_ptr());
            created_window
        };
        if handle.is_null() {
            Err("Failed to create window")
        } else {
            unsafe { CGL_window_make_context_current(handle); }
            Ok(Window {
                handle: handle,
                has_been_destroyed: false,
                event_handlers: std::collections::HashMap::new()
            })
        }
    }

    /// Creates a new undecorated window with the given title, width, and height.
    ///
    /// An undecorated window is a window without any borders or title bar.
    ///
    /// # Arguments
    ///
    /// * `title` - The title of the window.
    /// * `width` - The width of the window.
    /// * `height` - The height of the window.
    ///
    /// # Returns
    ///
    /// Returns a `Result` containing the created `Window` if successful, or an error message if the window creation failed.
    ///
    /// # Panics
    ///
    /// Panics if the width or height is not positive.
    ///
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///     let mut window = cgl_rs::Window::new_undecorated("My Window", 800, 600).unwrap();
    ///     // window.destroy(); // optional
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn new_undecorated(title: &str, width: i32, height: i32) -> Result<Window, &'static str> {
        assert!(width > 0 && height > 0, "Window dimensions must be positive");
        let title = std::ffi::CString::new(title).unwrap();
        let handle = unsafe {
            let created_window = CGL_window_create_undecorated(width, height, title.as_ptr());
            created_window
        };
        if handle.is_null() {
            Err("Failed to create window")
        } else {
            unsafe { CGL_window_make_context_current(handle); }
            Ok(Window {
                handle: handle,
                has_been_destroyed: false,
                event_handlers: std::collections::HashMap::new()
            })
        }
    }

    /// Destroys the window, freeing any resources associated with it.
    ///
    /// NOTE: This is called automatically when the `Window` goes out of scope, so it is not necessary to call this manually.
    ///       However, it is safe to call this multiple times, and the subsequent calls will have no effect.
    ///       But one thing to be noted is that this function must be called before `cgl_rs::shutdown()` is called.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///     let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
    ///     window.destroy(); // optional
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn destroy(&mut self) {
        if !self.has_been_destroyed {
            unsafe {
                CGL_window_destroy(self.handle);
                ACTIVE_WINDOWS[(self.handle as usize) % MAX_ACTIVE_WINDOWS] = 0 as *const Window;
                self.has_been_destroyed = true;
            }
        }
    }


    /// Registers the window to receive events by setting the appropriate callbacks.
    ///
    /// This function should be called after creating the window and before entering the event loop.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///     // window inside scope so that it is dropped before shutdown is called
    ///     let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
    ///     window.register_for_events();
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn register_for_events(&self) {
        unsafe {
            let window_id = (self.handle as usize) % MAX_ACTIVE_WINDOWS;
            ACTIVE_WINDOWS[window_id] = self as *const Window;
            CGL_window_set_key_callback(self.handle, cgl_window_key_callback);
            CGL_window_set_mouse_button_callback(self.handle, cgl_window_mouse_button_callback);
            CGL_window_set_mouse_position_callback(self.handle, cgl_window_mouse_position_callback);
            CGL_window_set_mouse_scroll_callback(self.handle, cgl_window_mouse_scroll_callback);
            CGL_window_set_framebuffer_size_callback(self.handle, cgl_window_framebuffer_size_callback);
            CGL_window_set_close_callback(self.handle, cgl_window_close_callback);
            CGL_window_set_drag_n_drop_callback(self.handle, cgl_window_drag_n_drop_callback);   
        }
    }


    /// Attaches an named event handler to the window.
    ///
    /// The `name` parameter is a string that identifies the event handler. This can be any string that is unique to the event handler.
    ///
    /// The `handler` parameter is a reference to a function that will be called when the event occurs. The function must have the following signature:
    ///
    /// ```no_run
    /// fn my_event_handler(window: &cgl_rs::Window, event: &cgl_rs::Event) {
    ///     println!("Event occurred!");
    /// }
    ///
    /// ```
    /// # Example
    ///
    /// ```no_run
    /// cgl_rs::init();
    /// {
    ///     let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
    ///
    ///     // window.attach_event_handler("my_event", &my_event_handler);
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn attach_event_handler(&mut self, name: &str, handler: &'static EventFunction) {
        self.event_handlers.insert(name.to_string(), Box::new(handler));
    }


    /// Detaches a named event handler from the window.
    ///
    /// The `name` parameter is a string that identifies the event handler. This should be the same string that was used to attach the event handler.
    ///
    /// # Example
    ///
    /// ```no_run
    /// cgl_rs::init();
    /// {
    ///     let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
    ///     
    ///     window.detach_event_handler("my_event");
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn detach_event_handler(&mut self, name: &str) {
        self.event_handlers.remove(name);
    }

    /// Polls for events on the window.
    ///
    /// This function should be called in a loop to continuously poll for events on the window.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///     // window inside scope so that it is dropped before shutdown is called
    ///     let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
    ///
    ///     while true {
    ///         window.poll_events();
    ///         // Handle events here
    ///         // ...
    ///         break; // break out of the loop for testing purposes
    ///     }
    ///
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn poll_events(&self) {
        unsafe {
            CGL_window_poll_events(self.handle);
        }
    }

    /// Swaps the front and back buffers of the window.
    ///
    /// This function should be called after rendering to the back buffer to display the rendered image on the screen.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///     // window inside scope so that it is dropped before shutdown is called
    ///     let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
    ///
    ///     while true {
    ///         window.swap_buffers();
    ///         // Handle events here
    ///         // ...
    ///         break; // break out of the loop for testing purposes
    ///     }
    ///
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn swap_buffers(&self) {
        unsafe {
            CGL_window_swap_buffers(self.handle);
        }
    }

    /// Returns whether or not the window should be closed.
    ///
    /// This function should be called in a loop to continuously check if the window should be closed.
    ///
    /// # Returns
    ///
    /// A boolean value indicating whether or not the window should be closed.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///     // window inside scope so that it is dropped before shutdown is called
    ///     let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
    ///
    ///     while !window.should_close() {
    ///         // ...
    ///         break; // break out of the loop for testing purposes
    ///     }
    ///
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn should_close(&self) -> bool {
        unsafe {
            CGL_window_should_close(self.handle) != 0
        } 
    }

    /// Sets the title of the window.
    ///
    /// # Arguments
    ///
    /// * `title` - A string slice containing the new title of the window.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///     // window inside scope so that it is dropped before shutdown is called
    ///     let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
    ///     window.set_title("New Title");
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_title(&self, title: &str) {
        let title = std::ffi::CString::new(title).unwrap();
        unsafe {
            CGL_window_set_title(self.handle, title.as_ptr());
        }
    }
    
    /// Sets the size of the window.
    ///
    /// # Arguments
    ///
    /// * `width` - The new width of the window.
    /// * `height` - The new height of the window.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///     // window inside scope so that it is dropped before shutdown is called
    ///     let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
    ///     window.set_size(1024, 768);
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_size(&self, width: i32, height: i32) {
        unsafe {
            CGL_window_set_size(self.handle, width, height);
        }
    }

    /// Sets the position of the window.
    ///
    /// # Arguments
    ///
    /// * `x` - The new x position of the window.
    /// * `y` - The new y position of the window.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///     // window inside scope so that it is dropped before shutdown is called
    ///     let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
    ///     window.set_position(100, 100);
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_position(&self, x: i32, y: i32) {
        unsafe {
            CGL_window_set_position(self.handle, x, y);
        }
    }

    /// Sets the visibility of the window.
    ///
    /// # Arguments
    ///
    /// * `hidden` - A boolean indicating whether the window should be hidden or not.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///     // window inside scope so that it is dropped before shutdown is called
    ///     let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
    ///     window.set_hidden(true);
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_hidden(&mut self, hidden: bool) {
        unsafe {
            CGL_window_set_hidden(self.handle, hidden as i32);
        }
    }

    /// Sets the user data associated with the window.
    ///
    /// # Arguments
    ///
    /// * `user_data` - A pointer to the user data to associate with the window.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///     // window inside scope so that it is dropped before shutdown is called
    ///     let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
    ///     let user_data = Box::into_raw(Box::new(42));
    ///     window.set_user_data(user_data);
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn set_user_data<T>(&self, user_data: *mut T) {
        unsafe {
            CGL_window_set_user_data(self.handle, user_data as *mut c_void);
        }
    }

    /// Gets the user data associated with the window.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///     // window inside scope so that it is dropped before shutdown is called
    ///     let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
    ///     let user_data = Box::into_raw(Box::new(42));
    ///     window.set_user_data(user_data);
    ///     let retrieved_user_data = window.get_user_data();
    ///     assert_eq!(retrieved_user_data, user_data);
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn get_user_data<T>(&self) -> *mut T {
        unsafe {
            CGL_window_get_user_data(self.handle) as *mut T
        }
    }

    /// Gets the size of the window.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///     // window inside scope so that it is dropped before shutdown is called
    ///     let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
    ///     let (width, height) = window.get_size();
    ///     assert_eq!(width, 800);
    ///     assert_eq!(height, 600);
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn get_size(&self) -> (i32, i32) {
        unsafe {
            let (mut width, mut height) = (0, 0);
            CGL_window_get_size(self.handle, &mut width, &mut height);
            (width, height)
        }
    }

    // Gets the position of the window.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///     // window inside scope so that it is dropped before shutdown is called
    ///     let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
    ///     let (x, y) = window.get_position();
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn get_position(&self) -> (i32, i32) {
        unsafe {
            let (mut x, mut y) = (0, 0);
            CGL_window_get_position(self.handle, &mut x, &mut y);
            (x, y)
        }
    }

    // Gets the size of the framebuffer of the window.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///     // window inside scope so that it is dropped before shutdown is called
    ///     let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
    ///     let (width, height) = window.get_framebuffer_size();
    ///     assert_eq!(width, 800);
    ///     assert_eq!(height, 600);
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn get_framebuffer_size(&self) -> (i32, i32) {
        unsafe {
            let (mut width, mut height) = (0, 0);
            CGL_window_get_framebuffer_size(self.handle, &mut width, &mut height);
            (width, height)
        }
    }

    /// Rescues the callbacks of the window.
    /// This is usefule whn usiong cgl::window along with any third party library
    /// that internally uses glfw with cgl::window.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///     // window inside scope so that it is dropped before shutdown is called
    ///     let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
    ///     window.rescure_callbacks();
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn rescure_callbacks(&self) {
        unsafe {
            CGL_window_resecure_callbacks(self.handle);
        }
    }


    /// Makes the OpenGL context of the window current.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///     // window inside scope so that it is dropped before shutdown is called
    ///     let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
    ///     window.make_context_current();
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn make_context_current(&self) {
        unsafe {
            CGL_window_make_context_current(self.handle);
        }
    }

    // Gets the GLFW handle of the window.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///     // window inside scope so that it is dropped before shutdown is called
    ///     let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
    ///     let handle = window.get_glfw_handle();
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn get_glfw_handle(&self) -> *mut GLFWwindow {
        unsafe {
            CGL_window_get_glfw_handle(self.handle)
        }
    }

    // Gets the state of a keyboard key of the window.
    ///
    /// # Arguments
    ///
    /// * `key` - A `Key` enum value representing the key to get the state of.
    ///
    /// # Returns
    ///
    /// An `Action` enum value representing the state of the key.
    ///
    /// # Example
    ///
    /// ```
    /// cgl_rs::init();
    /// {
    ///     // window inside scope so that it is dropped before shutdown is called
    ///     let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
    ///     let action = window.get_key(cgl_rs::Key::A);
    ///     match action {
    ///         cgl_rs::Action::Release => println!("Key A was released"),
    ///         cgl_rs::Action::Press => println!("Key A was pressed"),
    ///         cgl_rs::Action::Repeat => println!("Key A was repeated"),
    ///     }
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn get_key(&self, key: Key) -> Action {
        unsafe {
            cgl_window_i32_to_action(CGL_window_get_key(self.handle, key as i32))
        }
    }

    /// Checks if a keyboard key of the window is pressed.
    /// 
    /// # Arguments
    /// 
    /// * `key` - A `Key` enum value representing the key to check.
    /// 
    /// # Returns
    /// 
    /// A `bool` value representing if the key is pressed.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// {
    ///     // window inside scope so that it is dropped before shutdown is called
    ///     let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
    ///     if window.is_key_pressed(cgl_rs::Key::A) {
    ///         println!("Key A is pressed");
    ///     }
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn is_key_pressed(&self, key: Key) -> bool {
        unsafe {
            CGL_window_is_key_pressed(self.handle, key as i32) == 1
        }
    }

    /// Gets the state of a mouse button of the window.
    /// 
    /// # Arguments
    ///
    /// * `button` - A `MouseButton` enum value representing the mouse button to get the state of.
    /// 
    /// # Returns
    /// 
    /// An `Action` enum value representing the state of the mouse button.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// {
    ///     // window inside scope so that it is dropped before shutdown is called
    ///     let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
    ///     let action = window.get_mouse_button(cgl_rs::MouseButton::Left);
    ///     match action {
    ///        cgl_rs::Action::Release => println!("Left mouse button was released"),
    ///        cgl_rs::Action::Press => println!("Left mouse button was pressed"),
    ///        cgl_rs::Action::Repeat => println!("Left mouse button was repeated"),
    ///     }
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn get_mouse_button(&self, button: MouseButton) -> Action {
        unsafe {
            cgl_window_i32_to_action(CGL_window_get_mouse_button(self.handle, button as i32))               
        }
    }

    /// Checks if a mouse button of the window is pressed.
    /// 
    /// # Arguments
    ///
    /// * `button` - A `MouseButton` enum value representing the mouse button to check.
    /// 
    /// # Returns
    /// 
    /// A `bool` value representing if the mouse button is pressed.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// {
    ///     // window inside scope so that it is dropped before shutdown is called
    ///     let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
    ///     if window.is_mouse_button_pressed(cgl_rs::MouseButton::Left) {
    ///         println!("Left mouse button is pressed");
    ///     }
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn is_mouse_button_pressed(&self, button: MouseButton) -> bool {
        match self.get_mouse_button(button) {
            Action::Press => true,
            _ => false
        }
    }

    /// Gets the position of the mouse cursor relative to the top-left corner of the window.
    /// 
    /// # Returns
    /// 
    /// A tuple containing the x and y coordinates of the mouse cursor.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// {
    ///     // window inside scope so that it is dropped before shutdown is called
    ///     let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
    ///     let (x, y) = window.get_mouse_position();
    ///     println!("Mouse position: ({}, {})", x, y);
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn get_mouse_position(&self) -> (f64, f64) {
        unsafe {
            let (mut x, mut y) = (0.0, 0.0);
            CGL_window_get_mouse_position(self.handle, &mut x, &mut y);
            (x, y)
        }
    }

    /// Gets the internal handle of the window.
    /// 
    /// # Returns
    /// 
    /// A pointer to the internal handle of the window.
    pub(crate) fn get_cgl_handle(&self) -> *mut CGL_window {
        self.handle
    }

}

impl Drop for Window {
    fn drop(&mut self) {
        self.destroy();
    }
}

impl Clone for Window {
    /// Clones the current window instance.
    /// 
    /// NOTE: The new instance will have the same handle, `has_been_destroyed` flag and empty `event_handlers` map.
    ///       This means that the new instance will not be able to receive events nor will the internal window handle be
    ///       destroyed when the new instance is dropped. The internal window handle will be destroyed when the original
    ///       instance is dropped.
    /// 
    /// # Returns
    /// 
    /// A new `Window` instance with the same handle, `has_been_destroyed` flag and empty `event_handlers` map.
    /// 
    /// # Example
    /// 
    /// ```
    /// cgl_rs::init();
    /// {
    ///     // window inside scope so that it is dropped before shutdown is called
    ///     let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
    ///     let mut window_clone = window.clone();
    /// }
    /// cgl_rs::shutdown();
    fn clone(&self) -> Self {
        Window {
            handle: self.handle,
            has_been_destroyed: true,
            event_handlers: std::collections::HashMap::new()
        }
    }
}