var searchIndex = JSON.parse('{\
"cgl_rs":{"doc":"","t":"FOOOOAAFAANDENNNNNENNNNFLLLLLLFFFFLLLLLFFLLLFFFFLLLLLLLLLDDDALLLLLLLLLLLLLLLLLLALLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLMMMMLLLMMMLMMLLLFFFFFFFFFFFFRRRRRRRRRFFFFFFFFFFFFFFFFFFFFFFFFFFFFFNENNNNNNNNNNNNNNNNNNNNNEGNNNNNNNNNNNNNNNNNNNNNNNNNNNNDNNNNNNNNNNNNNNNNNENNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNENNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNDNNNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL","n":["init","log_error","log_info","log_trace","log_warning","logger","math","shutdown","utils","window_manager","Blue","CGL_logger_context","ConsoleColor","Error","Gray","Green","Info","Internal","LogLevel","Red","Reset","Trace","Warning","attach_log_file","borrow","borrow","borrow","borrow_mut","borrow_mut","borrow_mut","detach_log_file","disable_console_logging","enable_console_logging","flush","fmt","fmt","from","from","from","get_context","init","into","into","into","log","set_auto_flush_on_log","set_context","shutdown","try_from","try_from","try_from","try_into","try_into","try_into","type_id","type_id","type_id","Vector2","Vector3","Vector4","activations","add","add","add","angle","angle_between","angle_between","borrow","borrow","borrow","borrow_mut","borrow_mut","borrow_mut","clone","clone","clone","clone_into","clone_into","clone_into","constants","cross","cross","div","div","div","div","div","div","dot","dot","eq","eq","eq","fmt","fmt","fmt","fmt","from","from","from","from_angle","from_vec2","from_vec2","from_vec3","index","index","index","index_mut","index_mut","index_mut","into","into","into","length","length","mul","mul","mul","mul","mul","mul","neg","neg","neg","new","new","new","normalize","normalize","normalized","normalized","one","one","one","perpendicular","reflect","reflect","rotate_about_axis","rotate_about_origin","sub","sub","sub","to_owned","to_owned","to_owned","to_string","try_from","try_from","try_from","try_into","try_into","try_into","type_id","type_id","type_id","w","x","x","x","xy","xyz","xz","y","y","y","yz","z","z","zero","zero","zero","relu","relu_derivative","relu_leaky","relu_leaky_derivative","relu_smooth","relu_smooth_derivative","sigmoid","sigmoid_derivative","step","step_derivative","tanh","tanh_derivative","E","PI","PI2","PI_2","SQRT_2","SQRT_3","SQRT_5","SQRT_6","SQRT_7","append_file","append_string_to_file","big_endian_to_current","crc32","crc64","fast_rand","fast_srand","get_file_size","get_random_with_probability","get_time","get_timestamp","is_little_endian","lerp","little_endian_to_current","map","rand31","random_float","random_float_range","random_int","read_file","read_file_as_string","reverse_bytes","rot13","sleep","srand31","super_fast_hash","write_file","write_string_to_file","xorshf96","A","Action","Apostrophe","B","B4","B5","B6","B7","B8","Backslash","Backspace","C","CapsLock","Comma","D","Delete","Down","DragNDrop","E","End","Enter","Equal","Escape","Event","EventFunction","F","F1","F10","F11","F12","F13","F14","F15","F16","F17","F18","F19","F2","F20","F21","F22","F23","F24","F25","F3","F4","F5","F6","F7","F8","F9","FramebufferSize","G","GLFWwindow","GraveAccent","H","Home","I","Insert","J","K","K0","K1","K2","K3","K4","K5","K6","K7","K8","K9","Key","Key","KeyPad0","KeyPad1","KeyPad2","KeyPad3","KeyPad4","KeyPad5","KeyPad6","KeyPad7","KeyPad8","KeyPad9","KeyPadAdd","KeyPadDecimal","KeyPadDivide","KeyPadEnter","KeyPadEqual","KeyPadMultiply","KeyPadSubtract","L","Last","Left","Left","LeftAlt","LeftBracket","LeftControl","LeftShift","LeftSuper","M","Menu","Middle","Minus","MouseButton","MouseButton","MousePosition","MouseScroll","N","NumLock","O","P","PageDown","PageUp","Pause","Period","Press","PrintScreen","Q","R","Release","Repeat","Right","Right","RightAlt","RightBracket","RightControl","RightShift","RightSuper","S","ScrollLock","Semicolon","Slash","Space","T","Tab","U","Unknown","Up","V","W","Window","WindowClose","X","Y","Z","attach_event_handler","borrow","borrow","borrow","borrow","borrow","borrow","borrow_mut","borrow_mut","borrow_mut","borrow_mut","borrow_mut","borrow_mut","destroy","detach_event_handler","drop","fmt","fmt","fmt","fmt","from","from","from","from","from","from","get_framebuffer_size","get_glfw_handle","get_key","get_mouse_button","get_mouse_position","get_position","get_size","get_user_data","into","into","into","into","into","into","is_key_pressed","is_mouse_button_pressed","make_context_current","new","new_undecorated","poll_events","register_for_events","rescure_callbacks","set_hidden","set_position","set_size","set_title","set_user_data","should_close","swap_buffers","try_from","try_from","try_from","try_from","try_from","try_from","try_into","try_into","try_into","try_into","try_into","try_into","type_id","type_id","type_id","type_id","type_id","type_id"],"q":[[0,"cgl_rs"],[10,"cgl_rs::logger"],[57,"cgl_rs::math"],[171,"cgl_rs::math::activations"],[183,"cgl_rs::math::constants"],[192,"cgl_rs::utils"],[221,"cgl_rs::window_manager"]],"d":["Initializes the CGL library.","Logs an error message with the specified format string and …","Logs an info message with the specified format string and …","Logs a trace message with the specified format string and …","Logs a warning message with the specified format string …","This module contains functions for logging.","Math Library of CGL","","This module contains CGL utility functions, as well some …","The window manager module is responsible for creating and …","","A struct representing a logger context (internal use only, …","An enum representing console colors.","","","","","","An enum representing log levels.","","","","","Attaches a log file to the logger. If the log file already …","","","","","","","Detaches a log file from the logger.","Disables console logging.","Enables console logging.","Flushes all log messages to their respective log files.","","","Returns the argument unchanged.","Returns the argument unchanged.","Returns the argument unchanged.","Returns a pointer to the logger context.","Initializes the logger.","Calls <code>U::from(self)</code>.","Calls <code>U::from(self)</code>.","Calls <code>U::from(self)</code>.","Logs a message with the specified log level.","Sets whether the logger should automatically flush all log …","Sets the logger context.","Shuts down the logger.","","","","","","","","","","A 2-dimensional vector with <code>x</code> and <code>y</code> components.","A 3-dimensional vector with <code>x</code>, <code>y</code> and <code>z</code> components.","A 4-dimensional vector with <code>x</code>, <code>y</code>, <code>z</code> and <code>w</code> components.","Some popular activation functions and their derivatives.","","","","Calculates the angle (in radians) between this vector and …","Calculates the angle (in radians) between this vector and …","Returns the angle between this <code>Vector3</code> and another <code>Vector3</code>.","","","","","","","","","","","","","The CGL Math Constants","Calculates the cross product of this vector and another …","Computes the cross product of two <code>Vector3</code>s.","","","","","","","Calculates the dot product of this vector and another …","Computes the dot product of two <code>Vector3</code>s.","","","","","Formats the vector as a string.","","","Returns the argument unchanged.","Returns the argument unchanged.","Returns the argument unchanged.","Creates a new <code>Vector2</code> from the given angle (in radians).","Creates a new <code>Vector3</code> from a <code>Vector2</code> and a <code>z</code> component.","Creates a new <code>Vector4</code> from a <code>Vector2</code>, <code>z</code>, and <code>w</code> components.","Creates a new <code>Vector4</code> from a <code>Vector3</code> and a <code>w</code> component.","Returns a reference to the <code>f32</code> value at the given index.","Returns a reference to the element at the given index.","Arguments","Returns a mutable reference to the <code>f32</code> value at the given …","Allows mutable indexing into a <code>Vector3</code>.","Returns a mutable reference to the element at the given …","Calls <code>U::from(self)</code>.","Calls <code>U::from(self)</code>.","Calls <code>U::from(self)</code>.","Calculates the length of this vector.","","","","","","","","","","","Creates a new <code>Vector2</code> with the given <code>x</code> and <code>y</code> components.","Creates a new <code>Vector3</code> with the given <code>x</code>, <code>y</code>, and <code>z</code> …","Creates a new <code>Vector4</code> with the given <code>x</code>, <code>y</code>, <code>z</code>, and <code>w</code> …","Normalizes this vector in place.","Normalizes the <code>Vector3</code> in place.","Returns a new normalized <code>Vector2</code> representing this vector.","Returns a new <code>Vector3</code> that is the normalized version of …","Creates a new <code>Vector2</code> with <code>x</code> and <code>y</code> components set to <code>1.0</code>.","Returns a new <code>Vector3</code> with all components set to one.","Returns a new <code>Vector4</code> with all components set to <code>1.0</code>.","Returns a new <code>Vector2</code> that is perpendicular to this vector.","Returns the reflection of this vector off a surface with …","Calculates the reflection of this <code>Vector3</code> off a surface …","Rotates this <code>Vector3</code> about the given <code>axis</code> by the given …","Rotates this vector about the origin by the given angle …","","","","","","","","","","","","","","","","","","","","","Returns a new <code>Vector2</code> containing the <code>x</code> and <code>y</code> components of …","Extracts the <code>x</code>, <code>y</code>, and <code>z</code> components of the <code>Vector4</code> as a …","Returns a new <code>Vector2</code> containing the <code>x</code> and <code>z</code> components of …","","","","Returns a new <code>Vector2</code> containing the <code>y</code> and <code>z</code> components of …","","","Creates a new <code>Vector2</code> with <code>x</code> and <code>y</code> components set to <code>0.0</code>.","Returns a new <code>Vector3</code> with all components set to zero.","Returns a new <code>Vector4</code> with all components set to <code>0.0</code>.","Calculates the ReLU activation function for a given input.","Calculates the derivative of the ReLU activation function …","Calculates the leaky ReLU activation function for a given …","Calculates the derivative of the leaky ReLU activation …","Calculates the smooth ReLU activation function for a given …","Calculates the derivative of the smooth ReLU activation …","Calculates the sigmoid activation function for a given …","Calculates the derivative of the sigmoid activation …","Calculates the step activation function for a given input.","Calculates the derivative of the step activation function …","Calculates the hyperbolic tangent activation function for …","Calculates the derivative of the hyperbolic tangent …","","","","","","","","","","Arguments","Appends a string to a file.","Converts a slice of <code>u8</code> values from big-endian byte order …","Calculates the CRC32 checksum of the given data.","The C implementation is based on linux’s crc64 …","Generates a fast random number.","Sets the seed for the fast random number generator.","Returns the size of a file in bytes.","Returns a random index from the given slice of …","Returns the time in seconds since the program started.","Returns a string representing the current timestamp.","Returns a boolean indicating whether the system is …","Linearly interpolates between two values.","Converts a slice of <code>u8</code> values from little-endian byte …","Maps a value from one range to another.","Generates a random number using the rand31 algorithm.","Generates a random floating-point number between 0 and 1.","Generates a random floating-point number within the given …","Generates a random integer within the given range.","Reads the contents of a file and returns it as a vector of …","Reads the contents of a file and returns it as a string.","Reverses the order of bytes in the given slice of <code>u8</code> …","Applies the ROT13 substitution cipher to the given string.","Suspends the current thread for the specified amount of …","Sets the seed for the rand31 random number generator.","Calculates the SuperFastHash of the given data.","Arguments","Writes a string to a file.","Generates a fast random number using the xorshf96 …","","Represents an action, with each variant corresponding to a …","","","","","","","","","","","","","","","","","","","","","","Represents an event that can be received by a window, with …","Represents a function that handles a window event. Takes a …","","","","","","","","","","","","","","","","","","","","","","","","","","","","","The internal window handle used by GLFW","","","","","","","","","","","","","","","","","","Represents a keyboard key, with each variant corresponding …","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","Represents a mouse button, with each variant corresponding …","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","Represents a window, with a handle to the underlying CGL …","","","","","Attaches an named event handler to the window.","","","","","","","","","","","","","Destroys the window, freeing any resources associated with …","Detaches a named event handler from the window.","","","","","","Returns the argument unchanged.","Returns the argument unchanged.","Returns the argument unchanged.","Returns the argument unchanged.","Returns the argument unchanged.","Returns the argument unchanged.","Example","Example","Arguments","Gets the state of a mouse button of the window.","Gets the position of the mouse cursor relative to the …","Example","Gets the size of the window.","Gets the user data associated with the window.","Calls <code>U::from(self)</code>.","Calls <code>U::from(self)</code>.","Calls <code>U::from(self)</code>.","Calls <code>U::from(self)</code>.","Calls <code>U::from(self)</code>.","Calls <code>U::from(self)</code>.","Checks if a keyboard key of the window is pressed.","Checks if a mouse button of the window is pressed.","Makes the OpenGL context of the window current.","Creates a new window with the given title, width, and …","Creates a new undecorated window with the given title, …","Polls for events on the window.","Registers the window to receive events by setting the …","Rescues the callbacks of the window. This is usefule whn …","Sets the visibility of the window.","Sets the position of the window.","Sets the size of the window.","Sets the title of the window.","Sets the user data associated with the window.","Returns whether or not the window should be closed.","Swaps the front and back buffers of the window.","","","","","","","","","","","","","","","","","",""],"i":[0,0,0,0,0,0,0,0,0,0,4,0,0,7,4,4,7,7,0,4,4,7,7,0,8,4,7,8,4,7,0,0,0,0,4,7,8,4,7,0,0,8,4,7,0,0,0,0,8,4,7,8,4,7,8,4,7,0,0,0,0,10,11,12,10,10,11,10,11,12,10,11,12,10,11,12,10,11,12,0,10,11,10,10,11,11,12,12,10,11,10,11,12,10,10,11,12,10,11,12,10,11,12,12,10,11,12,10,11,12,10,11,12,10,11,10,10,11,11,12,12,10,11,12,10,11,12,10,11,10,11,10,11,12,10,10,11,11,10,10,11,12,10,11,12,10,10,11,12,10,11,12,10,11,12,12,10,11,12,11,12,11,10,11,12,11,11,12,10,11,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,29,0,29,29,30,30,30,30,30,29,29,29,29,29,29,29,29,32,29,29,29,29,29,0,0,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,32,29,0,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,0,32,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,30,29,30,29,29,29,29,29,29,29,30,29,0,32,32,32,29,29,29,29,29,29,29,29,31,29,29,29,31,31,29,30,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,0,32,29,29,29,27,33,27,29,30,31,32,33,27,29,30,31,32,27,27,27,29,30,31,32,33,27,29,30,31,32,27,27,27,27,27,27,27,27,33,27,29,30,31,32,27,27,27,27,27,27,27,27,27,27,27,27,27,27,27,33,27,29,30,31,32,33,27,29,30,31,32,33,27,29,30,31,32],"f":[[[],1],0,0,0,0,0,0,[[]],0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,[2,3],[[]],[[]],[[]],[[]],[[]],[[]],[2,3],[[]],[[]],[[]],[[4,5],6],[[7,5],6],[[]],[[]],[[]],[[],8],[3],[[]],[[]],[[]],[[7,2]],[3],[8],[[]],[[],1],[[],1],[[],1],[[],1],[[],1],[[],1],[[],9],[[],9],[[],9],0,0,0,0,[[10,10],10],[[11,11],11],[[12,12],12],[10,13],[[10,10],13],[[11,11],13],[[]],[[]],[[]],[[]],[[]],[[]],[10,10],[11,11],[12,12],[[]],[[]],[[]],0,[[10,10],13],[[11,11],11],[[10,10],10],[[10,13],10],[[11,11],11],[[11,13],11],[[12,13],12],[[12,12],12],[[10,10],13],[[11,11],13],[[10,10],3],[[11,11],3],[[12,12],3],[[10,5],6],[[10,5],6],[[11,5],6],[[12,5],6],[[]],[[]],[[]],[13,10],[[10,13],11],[[10,13,13],12],[[11,13],12],[[10,14],13],[[11,14],13],[[12,14]],[[10,14],13],[[11,14],13],[[12,14]],[[]],[[]],[[]],[10,13],[11,13],[[10,13],10],[[10,10],10],[[11,11],11],[[11,13],11],[[12,12],12],[[12,13],12],[10,10],[11,11],[12,12],[[13,13],10],[[13,13,13],11],[[13,13,13,13],12],[10],[11],[10,10],[11,11],[[],10],[[],11],[[],12],[10,10],[[10,10],10],[[11,11],11],[[11,11,13],11],[[10,13],10],[[10,10],10],[[11,11],11],[[12,12],12],[[]],[[]],[[]],[[],15],[[],1],[[],1],[[],1],[[],1],[[],1],[[],1],[[],9],[[],9],[[],9],0,0,0,0,[11,10],[12,11],[11,10],0,0,0,[11,10],0,0,[[],10],[[],11],[[],12],[13,13],[13,13],[13,13],[13,13],[13,13],[13,13],[13,13],[13,13],[13,13],[13,13],[13,13],[13,13],0,0,0,0,0,0,0,0,0,[[2,[17,[16]],14],3],[[2,2],3],[[[17,[16]]]],[[[17,[16]]],18],[[[17,[16]]],19],[[],20],[20],[2,14],[[[17,[13]]],14],[[],13],[[],15],[[],3],[[[0,[21,22,23,24]],[0,[21,22,23,24]],[0,[21,22,23,24]]],[[0,[21,22,23,24]]]],[[[17,[16]]]],[[[0,[21,22,23,24,25]],[0,[21,22,23,24,25]],[0,[21,22,23,24,25]],[0,[21,22,23,24,25]],[0,[21,22,23,24,25]]],[[0,[21,22,23,24,25]]]],[[],18],[[],13],[[13,13],13],[[20,20],20],[2],[2,15],[[[17,[16]]]],[2,15],[26],[18],[[[17,[16]]],18],[[2,[17,[16]],14],3],[[2,2],3],[[],19],0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,[[27,2,28]],[[]],[[]],[[]],[[]],[[]],[[]],[[]],[[]],[[]],[[]],[[]],[[]],[27],[[27,2]],[27],[[29,5],6],[[30,5],6],[[31,5],6],[[32,5],6],[[]],[[]],[[]],[[]],[[]],[[]],[27],[27,33],[[27,29],31],[[27,30],31],[27],[27],[27],[27],[[]],[[]],[[]],[[]],[[]],[[]],[[27,29],3],[[27,30],3],[27],[[2,20,20],[[1,[27,2]]]],[[2,20,20],[[1,[27,2]]]],[27],[27],[27],[[27,3]],[[27,20,20]],[[27,20,20]],[[27,2]],[27],[27,3],[27],[[],1],[[],1],[[],1],[[],1],[[],1],[[],1],[[],1],[[],1],[[],1],[[],1],[[],1],[[],1],[[],9],[[],9],[[],9],[[],9],[[],9],[[],9]],"c":[],"p":[[4,"Result"],[15,"str"],[15,"bool"],[4,"ConsoleColor"],[3,"Formatter"],[6,"Result"],[4,"LogLevel"],[3,"CGL_logger_context"],[3,"TypeId"],[3,"Vector2"],[3,"Vector3"],[3,"Vector4"],[15,"f32"],[15,"usize"],[3,"String"],[15,"u8"],[15,"slice"],[15,"u32"],[15,"u64"],[15,"i32"],[8,"Copy"],[8,"Add"],[8,"Sub"],[8,"Mul"],[8,"Div"],[6,"size_t"],[3,"Window"],[6,"EventFunction"],[4,"Key"],[4,"MouseButton"],[4,"Action"],[4,"Event"],[3,"GLFWwindow"]]},\
"libc":{"doc":"libc - Raw FFI bindings to platforms’ system libraries","t":"RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRERRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGRRRRRRRRRRRRRRRRRFFFFFMFFFFFFFFLLLLLLLLLLLLLLLLLLLLGGGGGGGGGGGGGEFFFGLLLLLLLLLFFFFGFFGFFFFFFFFFFFFFFFFFFLFEFFFFFFLLLLLLLLLLFFFFFFFFFFFFFFGGGGGGLLLLLLLLLLGFFFFFFFFFFFFFFFFFFFFFFFFFFMGFFFFFFFGFFFFFFFFFFFFFMMFFFFFGFGDFFGMMMMMMMMMMMDFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFGGDDEDMMMMMMMMMFFFLLLLLLLLLLLLLLLLLLLLMMMMLLLLLLLLLLGGGGGGFFDGFFFFFFFFFFFFFFFFFFF","n":["BUFSIZ","E2BIG","EACCES","EADDRINUSE","EADDRNOTAVAIL","EAFNOSUPPORT","EAGAIN","EALREADY","EBADF","EBADMSG","EBUSY","ECANCELED","ECHILD","ECONNABORTED","ECONNREFUSED","ECONNRESET","EDEADLK","EDEADLOCK","EDESTADDRREQ","EDOM","EEXIST","EFAULT","EFBIG","EHOSTUNREACH","EIDRM","EILSEQ","EINPROGRESS","EINTR","EINVAL","EIO","EISCONN","EISDIR","ELOOP","EMFILE","EMLINK","EMSGSIZE","ENAMETOOLONG","ENETDOWN","ENETRESET","ENETUNREACH","ENFILE","ENOBUFS","ENODATA","ENODEV","ENOENT","ENOEXEC","ENOLCK","ENOLINK","ENOMEM","ENOMSG","ENOPROTOOPT","ENOSPC","ENOSR","ENOSTR","ENOSYS","ENOTCONN","ENOTDIR","ENOTEMPTY","ENOTRECOVERABLE","ENOTSOCK","ENOTSUP","ENOTTY","ENXIO","EOF","EOPNOTSUPP","EOTHER","EOVERFLOW","EOWNERDEAD","EPERM","EPIPE","EPROTO","EPROTONOSUPPORT","EPROTOTYPE","ERANGE","EROFS","ESPIPE","ESRCH","ETIME","ETIMEDOUT","ETXTBSY","EWOULDBLOCK","EXDEV","EXIT_FAILURE","EXIT_SUCCESS","FILE","FILENAME_MAX","FOPEN_MAX","INT_MAX","INT_MIN","LC_ALL","LC_COLLATE","LC_CTYPE","LC_MONETARY","LC_NUMERIC","LC_TIME","L_tmpnam","NSIG","O_APPEND","O_BINARY","O_CREAT","O_EXCL","O_NOINHERIT","O_RANDOM","O_RAW","O_RDONLY","O_RDWR","O_SEQUENTIAL","O_TEMPORARY","O_TEXT","O_TRUNC","O_WRONLY","RAND_MAX","SEEK_CUR","SEEK_END","SEEK_SET","SIGABRT","SIGFPE","SIGILL","SIGINT","SIGSEGV","SIGTERM","SIG_ACK","SIG_DFL","SIG_ERR","SIG_GET","SIG_IGN","SIG_SGE","SOCKET","STRUNCATE","S_IEXEC","S_IFCHR","S_IFDIR","S_IFMT","S_IFREG","S_IREAD","S_IWRITE","TMP_MAX","_IOFBF","_IOLBF","_IONBF","_O_OBTAIN_DIR","_O_SHORT_LIVED","_O_U16TEXT","_O_U8TEXT","_O_WTEXT","_exit","abort","abs","accept","access","actime","aligned_free","aligned_malloc","atexit","atof","atoi","atol","atoll","bind","borrow","borrow","borrow","borrow","borrow","borrow","borrow","borrow","borrow","borrow","borrow_mut","borrow_mut","borrow_mut","borrow_mut","borrow_mut","borrow_mut","borrow_mut","borrow_mut","borrow_mut","borrow_mut","c_char","c_double","c_float","c_int","c_long","c_longlong","c_schar","c_short","c_uchar","c_uint","c_ulong","c_ulonglong","c_ushort","c_void","calloc","chdir","chmod","clock_t","clone","clone","clone","clone","clone","clone","clone","clone","clone","close","commit","connect","creat","dev_t","dup","dup2","errno_t","execl","execle","execlp","execlpe","execv","execve","execvp","execvpe","exit","fclose","fdopen","feof","ferror","fflush","fgetc","fgetpos","fgets","fileno","fmt","fopen","fpos_t","fprintf","fputc","fputs","fread","free","freopen","from","from","from","from","from","from","from","from","from","from","fseek","fsetpos","fstat","ftell","fwrite","get_osfhandle","getchar","getcwd","getenv","getpeername","getpid","getsockname","getsockopt","gmtime_s","ino_t","int16_t","int32_t","int64_t","int8_t","intmax_t","into","into","into","into","into","into","into","into","into","into","intptr_t","isalnum","isalpha","isatty","isblank","iscntrl","isdigit","isgraph","islower","isprint","ispunct","isspace","isupper","isxdigit","labs","listen","localtime_s","lseek","lseek64","malloc","memccpy","memchr","memcmp","memcpy","memmove","memset","mkdir","modtime","off_t","open","open_osfhandle","pclose","perror","pipe","popen","printf","ptrdiff_t","putchar","putenv","putenv_s","puts","raise","rand","read","realloc","recvfrom","remove","rename","rewind","rmdir","sa_data","sa_family","sendto","setbuf","setlocale","setsockopt","setvbuf","sighandler_t","signal","size_t","sockaddr","socket","srand","ssize_t","st_atime","st_ctime","st_dev","st_gid","st_ino","st_mode","st_mtime","st_nlink","st_rdev","st_size","st_uid","stat","stat","strcat","strchr","strcmp","strcoll","strcpy","strcspn","strdup","strerror","stricmp","strlen","strncat","strncmp","strncpy","strnicmp","strnlen","strpbrk","strrchr","strspn","strstr","strtod","strtof","strtok","strtol","strtoll","strtoul","strtoull","strxfrm","system","time","time64_t","time_t","timespec","timeval","timezone","tm","tm_hour","tm_isdst","tm_mday","tm_min","tm_mon","tm_sec","tm_wday","tm_yday","tm_year","tmpfile","tolower","toupper","try_from","try_from","try_from","try_from","try_from","try_from","try_from","try_from","try_from","try_from","try_into","try_into","try_into","try_into","try_into","try_into","try_into","try_into","try_into","try_into","tv_nsec","tv_sec","tv_sec","tv_usec","type_id","type_id","type_id","type_id","type_id","type_id","type_id","type_id","type_id","type_id","uint16_t","uint32_t","uint64_t","uint8_t","uintmax_t","uintptr_t","ungetc","unlink","utimbuf","wchar_t","wchmod","wcslen","wcstombs","wexecl","wexecle","wexeclp","wexeclpe","wexecv","wexecve","wexecvp","wexecvpe","wopen","wputenv","wputenv_s","write","wrmdir","wsetlocale","wstat","wutime"],"q":[[0,"libc"]],"d":["","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","Equivalent to C’s <code>void</code> type when used as a pointer.","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","Returns the argument unchanged.","Returns the argument unchanged.","Returns the argument unchanged.","Returns the argument unchanged.","Returns the argument unchanged.","Returns the argument unchanged.","Returns the argument unchanged.","Returns the argument unchanged.","Returns the argument unchanged.","Returns the argument unchanged.","","","","","","","","","","","","","","","","","","","","","Calls <code>U::from(self)</code>.","Calls <code>U::from(self)</code>.","Calls <code>U::from(self)</code>.","Calls <code>U::from(self)</code>.","Calls <code>U::from(self)</code>.","Calls <code>U::from(self)</code>.","Calls <code>U::from(self)</code>.","Calls <code>U::from(self)</code>.","Calls <code>U::from(self)</code>.","Calls <code>U::from(self)</code>.","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","",""],"i":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,10,1,2,3,4,5,6,7,8,9,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,9,0,0,0,0,0,0,0,0,0,0,0,0,4,4,4,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,6,6,6,6,6,6,0,0,0,1,2,3,4,5,6,7,8,9,10,1,2,3,4,5,6,7,8,9,10,8,7,8,7,1,2,3,4,5,6,7,8,9,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],"f":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,[[]],[[]],[[]],[[]],[[]],[[]],[[]],[[]],[[]],[[]],[[]],[[]],[[]],[[]],[[]],[[]],[[]],[[]],[[]],[[]],0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,[1,1],[2,2],[3,3],[4,4],[5,5],[6,6],[7,7],[8,8],[9,9],0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,[[10,11],[[13,[12]]]],0,0,0,0,0,0,0,0,[[]],[[]],[[]],[[]],[[]],[[]],[[]],[[]],[[]],[[]],0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,[[]],[[]],[[]],[[]],[[]],[[]],[[]],[[]],[[]],[[]],0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,[[],13],[[],13],[[],13],[[],13],[[],13],[[],13],[[],13],[[],13],[[],13],[[],13],[[],13],[[],13],[[],13],[[],13],[[],13],[[],13],[[],13],[[],13],[[],13],[[],13],0,0,0,0,[[],14],[[],14],[[],14],[[],14],[[],14],[[],14],[[],14],[[],14],[[],14],[[],14],0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],"c":[266,267,268,269,439,440,441,442],"p":[[4,"timezone"],[4,"FILE"],[4,"fpos_t"],[3,"stat"],[3,"utimbuf"],[3,"tm"],[3,"timeval"],[3,"timespec"],[3,"sockaddr"],[4,"c_void"],[3,"Formatter"],[3,"Error"],[4,"Result"],[3,"TypeId"]]},\
"sandbox":{"doc":"","t":"FF","n":["main","on_window_event"],"q":[[0,"sandbox"]],"d":["",""],"i":[0,0],"f":[[[]],[[1,2],3]],"c":[],"p":[[3,"Window"],[4,"Event"],[15,"bool"]]}\
}');
if (typeof window !== 'undefined' && window.initSearch) {window.initSearch(searchIndex)};
if (typeof exports !== 'undefined') {exports.searchIndex = searchIndex};
