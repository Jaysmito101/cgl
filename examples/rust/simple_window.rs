fn on_window_event(_window: &cgl_rs::Window, event: &cgl_rs::Event) -> bool {
    match event {
        cgl_rs::Event::WindowClose => {
            println!("Window Close Event");
            true
        },
        cgl_rs::Event::FramebufferSize(width, height) => {
            println!("Window Resize Event: {}x{}", width, height);
            false
        },   
        cgl_rs::Event::DragNDrop(paths) => {
            println!("Drag and Drop Event: {:?}", paths);
            false
        },
        _ => {false}
    }
}

fn main() {
    cgl_rs::init().expect("Failed to initialize CGL");
    {
        // new scope so the window is dropped before shutdown
        let mut window = cgl_rs::Window::new("My Window", 800, 600).unwrap();
        window.register_for_events();
        window.attach_event_handler("my_event_handler", &on_window_event);
        while !window.should_close() {
            if window.is_key_pressed(cgl_rs::Key::Escape) {
                break;
            }

            window.poll_events();
            window.swap_buffers();
        }
    }
    cgl_rs::shutdown();
}