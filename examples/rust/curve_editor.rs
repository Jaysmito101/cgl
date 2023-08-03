//! The Curve Editor Example

/*
MIT License

Copyright (c) 2023 Jaysmito Mukherjee (jaysmito101@gmail.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

use cgl_rs::math::Vector2;
use cgl_rs::graphics::widgets as widgets;

fn main() {

    // Initialze CGL Core
    cgl_rs::init().expect("Failed to initialize CGL");

    {

        // Create the window
        let window = cgl_rs::Window::new("Curve Editor [cgl-rs] - Jaysmito Mukherjee", 512, 512).expect("Failed to create window");
        window.make_context_current();

        // Initialize CGL Graphics
        cgl_rs::graphics::init().expect("Failed to initialize CGL Graphics");

        // Initialize CGL Widgets
        widgets::init().expect("Failed to initialize CGL Widgets");

        let framebuffer = cgl_rs::graphics::Framebuffer::from_default(&window).expect("Failed to create framebuffer");
        let (mut selected_point, mut line_resolution) = (0i32, 10i32);
        let mut points =vec![
            Vector2::new(-0.8, 0.0),
            Vector2::new(0.8, 0.0),
            Vector2::new(0.0, 0.5),
            Vector2::new(0.0, -0.5)
        ];

        while !window.should_close() {
            window.set_size(512, 512);

            framebuffer.bind();

            cgl_rs::graphics::clear(0.2, 0.2, 0.2, 0.2);

            widgets::begin();

            for i in 0..4 {
                let ref point = points[i];
                widgets::set_fill_colorf(0.9, 0.8, 0.2, 1.0);
                if i as i32 == selected_point {
                    widgets::add_circle2f(point.x, point.y, 0.06);
                }
                if i < 2 {
                    widgets::set_fill_colorf(0.2, 0.8, 0.2, 1.0);
                }
                else {
                    widgets::set_fill_colorf(0.8, 0.2, 0.2, 1.0);
                }
                widgets::add_circle2f(point.x, point.y, 0.05);
            }

            widgets::set_stroke_colorf(0.8, 0.7, 0.3, 1.0);
            widgets::set_stroke_thickness(0.01);

            widgets::add_cubic_bazier2v(points[0], points[1], points[2], points[3], line_resolution);
            widgets::add_cubic_bazier_points2v(points[0], points[1], points[2], points[3], line_resolution);


            widgets::set_fill_colorf(0.8, 0.8, 0.8, 1.0);
            widgets::add_string(format!("Line Resolution: {}", line_resolution).as_str(), -1.0, 0.9, 1.0, 0.1);

            if window.is_key_pressed(cgl_rs::Key::Up) {
                line_resolution += 1;
                cgl_rs::utils::sleep(250);
            }
            else if window.is_key_pressed(cgl_rs::Key::Down) {
                line_resolution -= 1;
                cgl_rs::utils::sleep(250);
            }

            let (mut mpx, mut  mpy) = window.get_mouse_position();
            mpx = mpx / 512.0 * 2.0 - 1.0;
            mpy = 2.0 - mpy / 512.0 * 2.0 - 1.0;
            selected_point = -1;

            for i in 0..4 {
                let ref point = points[i];
                if ((point.x - mpx as f32).powf(2.0) + (point.y - mpy as f32).powf(2.0)).sqrt() < 0.1 {
                    selected_point = i as i32;
                    break;
                }
            }

            if selected_point != -1 {
                if window.get_mouse_button(cgl_rs::MouseButton::Left) == cgl_rs::Action::Press {
                    points[selected_point as usize] = Vector2::new(mpx as f32, mpy as f32);
                }
            }

            widgets::end();

            window.swap_buffers();
            window.poll_events();

        }

        widgets::shutdown();
        cgl_rs::graphics::shutdown();
    }

    cgl_rs::shutdown();
}