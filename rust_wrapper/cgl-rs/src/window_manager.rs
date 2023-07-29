#![allow(non_camel_case_types)]
use std::ops::{Drop};
use libc::{c_void, c_int, c_char, c_double};

/// The internal window handle used by CGL
#[repr(C)]
struct CGL_window {
    nothing: c_void
}

/// The internal window handle used by GLFW
#[repr(C)]
pub struct GLFWwindow {
    nothing: c_void
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
    KeyUnknown            = -1,
    KeySpace              = 32,
    KeyApostrophe         = 39,
    KeyComma              = 44,
    KeyMinus              = 45,
    KeyPeriod             = 46,
    KeySlash              = 47,
    Key0                  = 48,
    Key1                  = 49,
    Key2                  = 50,
    Key3                  = 51,
    Key4                  = 52,
    Key5                  = 53,
    Key6                  = 54,
    Key7                  = 55,
    Key8                  = 56,
    Key9                  = 57,
    KeySemicolon          = 59,
    KeyEqual              = 61,
    KeyA                  = 65,
    KeyB                  = 66,
    KeyC                  = 67,
    KeyD                  = 68,
    KeyE                  = 69,
    KeyF                  = 70,
    KeyG                  = 71,
    KeyH                  = 72,
    KeyI                  = 73,
    KeyJ                  = 74,
    KeyK                  = 75,
    KeyL                  = 76,
    KeyM                  = 77,
    KeyN                  = 78,
    KeyO                  = 79,
    KeyP                  = 80,
    KeyQ                  = 81,
    KeyR                  = 82,
    KeyS                  = 83,
    KeyT                  = 84,
    KeyU                  = 85,
    KeyV                  = 86,
    KeyW                  = 87,
    KeyX                  = 88,
    KeyY                  = 89,
    KeyZ                  = 90,
    KeyLeftBracket        = 91,
    KeyBackslash          = 92,
    KeyRightBracket       = 93,
    KeyGraveAccent        = 96,
    KeyEscape             = 256,
    KeyEnter              = 257,
    KeyTab                = 258,
    KeyBackspace          = 259,
    KeyInsert             = 260,
    KeyDelete             = 261,
    KeyRight              = 262,
    KeyLeft               = 263,
    KeyDown               = 264,
    KeyUp                 = 265,
    KeyPageUp             = 266,
    KeyPageDown           = 267,
    KeyHome               = 268,
    KeyEnd                = 269,
    KeyCapsLock           = 280,
    KeyScrollLock         = 281,
    KeyNumLock            = 282,
    KeyPrintScreen        = 283,
    KeyPause              = 284,
    KeyF1                 = 290,
    KeyF2                 = 291,
    KeyF3                 = 292,
    KeyF4                 = 293,
    KeyF5                 = 294,
    KeyF6                 = 295,
    KeyF7                 = 296,
    KeyF8                 = 297,
    KeyF9                 = 298,
    KeyF10                = 299,
    KeyF11                = 300,
    KeyF12                = 301,
    KeyF13                = 302,
    KeyF14                = 303,
    KeyF15                = 304,
    KeyF16                = 305,
    KeyF17                = 306,
    KeyF18                = 307,
    KeyF19                = 308,
    KeyF20                = 309,
    KeyF21                = 310,
    KeyF22                = 311,
    KeyF23                = 312,
    KeyF24                = 313,
    KeyF25                = 314,
    KeyPad0               = 320,
    KeyPad1               = 321,
    KeyPad2               = 322,
    KeyPad3               = 323,
    KeyPad4               = 324,
    KeyPad5               = 325,
    KeyPad6               = 326,
    KeyPad7               = 327,
    KeyPad8               = 328,
    KeyPad9               = 329,
    KeyPadDecimal         = 330,
    KeyPadDivide          = 331,
    KeyPadMultiply        = 332,
    KeyPadSubtract        = 333,
    KeyPadAdd             = 334,
    KeyPadEnter           = 335,
    KeyPadEqual           = 336,
    KeyLeftShift          = 340,
    KeyLeftControl        = 341,
    KeyLeftAlt            = 342,
    KeyLeftSuper          = 343,
    KeyRightShift         = 344,
    KeyRightControl       = 345,
    KeyRightAlt           = 346,
    KeyRightSuper         = 347,
    KeyMenu               = 348
}

/// Represents a mouse button, with each variant corresponding to a specific button code.
#[derive(Debug)]
pub enum MouseButton {
    MouseButtonLeft      = 0,
    MouseButtonRight     = 1,
    MouseButtonMiddle    = 2,
    MouseButton4         = 3,
    MouseButton5         = 4,
    MouseButton6         = 5,
    MouseButton7         = 6,
    MouseButton8         = 7,
    MouseButtonLast      = 8
}

/// Represents an action, with each variant corresponding to a specific action code.
#[derive(Debug)]
pub enum Action {
    ActionRelease        = 0,
    ActionPress          = 1,
    ActionRepeat         = 2
}

/// Represents an event that can be received by a window, with each variant corresponding to a specific type of event.
#[derive(Debug)]
pub enum Event {
    EventKey(Key, Action, i32, i32),
    EventMouseButton(MouseButton, Action, i32),
    EventMousePosition(f64, f64),
    EventMouseScroll(f64, f64),
    EventFramebufferSize(i32, i32),
    EventWindowClose,
    EventDragNDrop(Vec<String>)
}

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
        0 => Action::ActionRelease,
        1 => Action::ActionPress,
        2 => Action::ActionRepeat,
        _ => panic!("Invalid action code")
    }
}

fn cgl_window_i32_to_key(key: i32) -> Key {
    match key {
        32 => Key::KeySpace,
        39 => Key::KeyApostrophe,
        44 => Key::KeyComma,
        45 => Key::KeyMinus,
        46 => Key::KeyPeriod,
        47 => Key::KeySlash,
        48 => Key::Key0,
        49 => Key::Key1,
        50 => Key::Key2,
        51 => Key::Key3,
        52 => Key::Key4,
        53 => Key::Key5,
        54 => Key::Key6,
        55 => Key::Key7,
        56 => Key::Key8,
        57 => Key::Key9,
        59 => Key::KeySemicolon,
        61 => Key::KeyEqual,
        65 => Key::KeyA,
        66 => Key::KeyB,
        67 => Key::KeyC,
        68 => Key::KeyD,
        69 => Key::KeyE,
        70 => Key::KeyF,
        71 => Key::KeyG,
        72 => Key::KeyH,
        73 => Key::KeyI,
        74 => Key::KeyJ,
        75 => Key::KeyK,
        76 => Key::KeyL,
        77 => Key::KeyM,
        78 => Key::KeyN,
        79 => Key::KeyO,
        80 => Key::KeyP,
        81 => Key::KeyQ,
        82 => Key::KeyR,
        83 => Key::KeyS,
        84 => Key::KeyT,
        85 => Key::KeyU,
        86 => Key::KeyV,
        87 => Key::KeyW,
        88 => Key::KeyX,
        89 => Key::KeyY,
        90 => Key::KeyZ,
        91 => Key::KeyLeftBracket,
        92 => Key::KeyBackslash,
        93 => Key::KeyRightBracket,
        96 => Key::KeyGraveAccent,
        256 => Key::KeyEscape,
        257 => Key::KeyEnter,
        258 => Key::KeyTab,
        259 => Key::KeyBackspace,
        260 => Key::KeyInsert,
        261 => Key::KeyDelete,
        262 => Key::KeyRight,
        263 => Key::KeyLeft,
        264 => Key::KeyDown,
        265 => Key::KeyUp,
        266 => Key::KeyPageUp,
        267 => Key::KeyPageDown,
        268 => Key::KeyHome,
        269 => Key::KeyEnd,
        280 => Key::KeyCapsLock,
        281 => Key::KeyScrollLock,
        282 => Key::KeyNumLock,
        283 => Key::KeyPrintScreen,
        284 => Key::KeyPause,
        290 => Key::KeyF1,
        291 => Key::KeyF2,
        292 => Key::KeyF3,
        293 => Key::KeyF4,
        294 => Key::KeyF5,
        295 => Key::KeyF6,
        296 => Key::KeyF7,
        297 => Key::KeyF8,
        298 => Key::KeyF9,
        299 => Key::KeyF10,
        300 => Key::KeyF11,
        301 => Key::KeyF12,
        302 => Key::KeyF13,
        303 => Key::KeyF14,
        304 => Key::KeyF15,
        305 => Key::KeyF16,
        306 => Key::KeyF17,
        307 => Key::KeyF18,
        308 => Key::KeyF19,
        309 => Key::KeyF20,
        310 => Key::KeyF21,
        311 => Key::KeyF22,
        312 => Key::KeyF23,
        313 => Key::KeyF24,
        314 => Key::KeyF25,
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
        340 => Key::KeyLeftShift,
        341 => Key::KeyLeftControl,
        342 => Key::KeyLeftAlt,
        343 => Key::KeyLeftSuper,
        344 => Key::KeyRightShift,
        345 => Key::KeyRightControl,
        346 => Key::KeyRightAlt,
        347 => Key::KeyRightSuper,
        348 => Key::KeyMenu,
        _ => panic!("Invalid key code")
    }
}

fn cgl_window_i32_to_mouse_button(button: i32) -> MouseButton {
    match button {
        0 => MouseButton::MouseButtonLeft,
        1 => MouseButton::MouseButtonRight,
        2 => MouseButton::MouseButtonMiddle,
        3 => MouseButton::MouseButton4,
        4 => MouseButton::MouseButton5,
        5 => MouseButton::MouseButton6,
        6 => MouseButton::MouseButton7,
        7 => MouseButton::MouseButton8,
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
    cgl_window_dispatch_event(window, Event::EventKey(cgl_window_i32_to_key(key), cgl_window_i32_to_action(action), scancode, mods));
    fn_returning_c_void()
}

extern "C" fn cgl_window_mouse_button_callback(window: *mut CGL_window, button: i32, action: i32, mods: i32) -> c_void {
    cgl_window_dispatch_event(window, Event::EventMouseButton(cgl_window_i32_to_mouse_button(button), cgl_window_i32_to_action(action), mods));
    fn_returning_c_void()
}

extern "C" fn cgl_window_mouse_position_callback(window: *mut CGL_window, xpos: f64, ypos: f64) -> c_void {
    cgl_window_dispatch_event(window, Event::EventMousePosition(xpos, ypos));
    fn_returning_c_void()
}

extern "C" fn cgl_window_mouse_scroll_callback(window: *mut CGL_window, xoffset: f64, yoffset: f64) -> c_void {
    cgl_window_dispatch_event(window, Event::EventMouseScroll(xoffset, yoffset));
    fn_returning_c_void()
}

extern "C" fn cgl_window_framebuffer_size_callback(window: *mut CGL_window, width: i32, height: i32) -> c_void {
    cgl_window_dispatch_event(window, Event::EventFramebufferSize(width, height));
    fn_returning_c_void()
}

extern "C" fn cgl_window_close_callback(window: *mut CGL_window) -> c_void {
    cgl_window_dispatch_event(window, Event::EventWindowClose);
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
    cgl_window_dispatch_event(window, Event::EventDragNDrop(paths_vec));
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
    /// ```norun
    /// fn my_event_handler(window: &Window, event: &Event) {
    ///     println!("Event occurred!");
    /// }
    /// ```
    ///
    /// # Example
    ///
    /// ```norun
    /// cgl_rs::init();
    /// {
    ///     let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
    ///
    ///     window.attach_event_handler("my_event", &my_event_handler);
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
    /// ```norun
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
    pub fn set_title(&mut self, title: &str) {
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
    pub fn set_size(&mut self, width: i32, height: i32) {
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
    pub fn set_position(&mut self, x: i32, y: i32) {
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
    pub fn set_user_data<T>(&mut self, user_data: *mut T) {
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
    pub fn rescure_callbacks(&mut self) {
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
    pub fn make_context_current(&mut self) {
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
    ///     let action = window.get_key(cgl_rs::Key::KeyA);
    ///     match action {
    ///         cgl_rs::Action::ActionRelease => println!("Key A was released"),
    ///         cgl_rs::Action::ActionPress => println!("Key A was pressed"),
    ///         cgl_rs::Action::ActionRepeat => println!("Key A was repeated"),
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
    ///     if window.is_key_pressed(cgl_rs::Key::KeyA) {
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
    ///     let action = window.get_mouse_button(cgl_rs::MouseButton::MouseButtonLeft);
    ///     match action {
    ///        cgl_rs::Action::ActionRelease => println!("Left mouse button was released"),
    ///        cgl_rs::Action::ActionPress => println!("Left mouse button was pressed"),
    ///        cgl_rs::Action::ActionRepeat => println!("Left mouse button was repeated"),
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
    ///     if window.is_mouse_button_pressed(cgl_rs::MouseButton::MouseButtonLeft) {
    ///         println!("Left mouse button is pressed");
    ///     }
    /// }
    /// cgl_rs::shutdown();
    /// ```
    pub fn is_mouse_button_pressed(&self, button: MouseButton) -> bool {
        match self.get_mouse_button(button) {
            Action::ActionPress => true,
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



}

impl Drop for Window {
    fn drop(&mut self) {
        self.destroy();
    }
}