// This is is just for convenience
use cgl_rs::math::Vector2;

// The main data structure to hold the actual grraph
// and perform the search on it
struct Graph {
    points      : Vec<cgl_rs::math::Vector2>, // The points
    connections : Vec<(usize, usize)>, // The connections
    radius      : f32, // The radius of the points for rendeing and mouse interaction
                       // for now it's a constant, but it could be a parameter
}

impl Graph { 

    // Create a new empty graph
    pub fn new() -> Self {
        Self {
            points          : Vec::new(),
            connections     : Vec::new(),
            radius          : 0.05,
        }
    }


    // Get the index of the point under a given position
    // This is primarily used for mouse interaction
    pub fn get_point_under_cursor(&self, cursor: cgl_rs::math::Vector2) -> Option<usize> {
        for (index, point) in self.points.iter().enumerate() { // loop over all points
            // check if the distance between the point and the cursor is less than 1.5 times the radius
            // this is to make the interaction easier with a bit of a margin
            if (*point - cursor).length() < self.radius * 1.5 { 
                return Some(index);
            }
        }

        // if no point is under the cursor return None
        None
    }


    // Get the point at a given index
    pub fn point(&self, index: usize) -> &cgl_rs::math::Vector2 {
        &self.points[index]
    }

    // clear the graph
    // clear all points and connections
    pub fn clear(&mut self) {
        self.points.clear(); // clear all points
        self.connections.clear(); // clear all connections
    }

    // add a point to the graph
    // for now it just appends the point to the list of points
    // but it can also ddo some other logic like checking if the point already exists
    // it returns the index of the point
    pub fn add_point(&mut self, point: cgl_rs::math::Vector2) -> usize {
        self.points.push(point); // add the point to the list of points
        self.points.len() - 1 // return the index of the point to be used later
    }

    // remove a point from the graph at a given index
    pub fn remove_point(&mut self, index: usize) {
        self.points.remove(index); // remove the point at the given index

        // remove all connections that are connected to the point
        self.connections.retain(|(a, b)| *a != index && *b != index);

        // update the indices of the connections
        // This is necessary because the indices of the points have changed
        // so the indices of the connections should be updated
        for (a, b) in self.connections.iter_mut() {
            if *a > index { // if the index of the first point is greater than the index of the removed point
                *a -= 1;
            }
            if *b > index { // if the index of the second point is greater than the index of the removed point
                *b -= 1;
            }
        }
    }

    // add a connection between two points
    pub fn add_connection(&mut self, a: usize, b: usize) {
        // check if connection already exists
        for (a_, b_) in self.connections.iter() { // loop over all connections
            if (a == *a_ && b == *b_) || (a == *b_ && b == *a_) { // if the connection already exists
                return; // do nothing if it already exists, maybe return an error or something
            }
        }

        // add the connection if it doesn't exist
        self.connections.push((a, b));
    }


    // this is a function to get all the neighbours of a point
    // this is needed as we arent storing the points in a real graph like
    // datastructure with links between them but rather as a list of points
    // and a list of connections so that its easier and faster to render them
    pub fn get_neighbours(&self, index: usize) -> Vec<(usize, f32)> {
        let mut result = Vec::new(); // the result vector to hold the neighbours
        for connection in self.connections.iter() {
            if connection.0 == index  { // if the second point of the connection is the point we are looking for
                result.push((connection.1, (self.points[connection.1] - self.points[index]).length()));
            }
            else if connection.1 == index  { // if the first point of the connection is the point we are looking for
                result.push((connection.0, (self.points[connection.0] - self.points[index]).length()));
            }            
        }

        // return the result
        result
    }

    // The acutal implementation of the Dijkstra algorithm
    pub fn calculate_path(&self, start: usize, end: usize) -> Vec<usize> {

        // Initialize the data structures needed for the algorithm
        let mut path = Vec::new();
        let mut visited = vec![false; self.points.len()];
        let mut distance = vec![std::f32::INFINITY; self.points.len()];
        let mut previous = vec![None; self.points.len()];

        // NOTE: In this implementation we are not really intersted in preprocesing the graph
        //       but rather just to find the shortest path between two points in the graph
        
        // the current is referring to the current point we are visiting
        let mut current = start;
        distance[current] = 0.0; // the distance to the start point is 0 all other points are infinity initially


        loop { // loop until we visit all points (we break the loop when we visit the end point or when no more points are left explicitly)
            // get the neighbours of the current point
            let neighbours = self.get_neighbours(current);

            // loop over all neighbours
            for (neighbour, weight) in neighbours {
                if !visited[neighbour] { // if the neighbour is not visited
                    let new_distance = distance[current] + weight; // calculate the distance to the neighbour from the start point
                    if new_distance < distance[neighbour] { 
                        // if the new distance is less than the current distance to the neighbour
                        // we found a shorter path to the neighbour so we update the distance and the previous point
                        distance[neighbour] = new_distance;
                        previous[neighbour] = Some(current);
                    }
                }
            }

            // mark the current point as visited
            visited[current] = true;

            // if we visited the end point we break the loop
            // as we have found the shortest path
            if current == end {
                break;
            }

            // find the neighbour with min distance that is not visited
            let mut min_distance = std::f32::INFINITY;
            let mut next = None;
            for (index, dist) in distance.iter().enumerate() {
                if !visited[index] && *dist < min_distance {
                    min_distance = *dist;
                    next = Some(index);
                }
            }

            // if there is no more points to visit we break the loop
            if next.is_none() {
                break;
            }

            // set the next point to be the current point
            current = next.unwrap();
        }

        // We have found the shortest path
        // Now we need to reconstruct the path

        // We start from the end point and go back to the start point
        let mut current = end;
        while let Some(prev) = previous[current] {
            path.push(current);
            current = prev;
        }
        // add the start point to the path
        path.push(start);

        path.reverse(); // reverse the path so it goes from the start to the end
        path       
    }

}

fn main() {
    // Initialize the logger
    cgl_rs::init().expect("Failed to initialize logger");

    // Create a window
    let window = cgl_rs::Window::new("Dijkstra Visualizer - Jaysmito Mukherjee", 800, 800).expect("Failed to create window");
    window.make_context_current(); // This must be called before initializing the graphics or it will crash

    // Initialize the graphics and widgets
    cgl_rs::graphics::init().expect("Failed to initialize graphics");
    cgl_rs::graphics::widgets::init().expect("Failed to initialize widgets");

    // get a reference to the window framebuffer
    let main_framebuffer = cgl_rs::graphics::Framebuffer::from_default(&window).expect("Failed to create main framebuffer");

    // Initialize the variables
    let mut graph = Graph::new();
    let mut has_released_since_last_press = true; // A flag to check if the mouse button has been released since the last press
    let mut active_point = None; // The point that is currently being dragged
    let mut last_two_points = (None, None); // The last two points that were clicked (to connect them)
    // The start and end points of the path
    let mut start_point = None;
    let mut end_point = None; 

    while !window.should_close() { 
        window.set_size(800, 800); // Forcing window size to 800x800

        main_framebuffer.bind(); // Bind the main framebuffer
        cgl_rs::graphics::clear(0.2, 0.2, 0.2, 1.0); // Clear the screen with a gray color
        cgl_rs::graphics::widgets::begin(); 

        //  Draw connections first at the bottom layer
        cgl_rs::graphics::widgets::set_stroke_colorf(0.4, 0.4, 0.4, 1.0);
        cgl_rs::graphics::widgets::set_stroke_thickness(0.01);
        for (a, b) in graph.connections.iter() {
            let point_a = graph.point(*a);
            let point_b = graph.point(*b);
            cgl_rs::graphics::widgets::add_line2f(point_a.x, point_a.y, point_b.x, point_b.y);
        }
        
        // now draw the path if a start and end point are selected
        cgl_rs::graphics::widgets::set_stroke_colorf(0.8, 0.4, 0.4, 1.0);
        if start_point.is_some() && end_point.is_some() {
            let path = graph.calculate_path(start_point.unwrap(), end_point.unwrap());
            for i in 0..path.len() - 1 {
                let point_a = graph.point(path[i]);
                let point_b = graph.point(path[i + 1]);
                cgl_rs::graphics::widgets::add_line2f(point_a.x, point_a.y, point_b.x, point_b.y);
            }
        }

        // draw the points
        cgl_rs::graphics::widgets::set_fill_colorf(0.4, 0.4, 0.4, 1.0);
        for point in graph.points() {
            cgl_rs::graphics::widgets::add_circle2f(point.x, point.y, graph.radius);
        }
        
        // highlight the selected points
        cgl_rs::graphics::widgets::set_fill_colorf(0.1, 0.1, 0.1, 1.0);
        if let Some(index) = last_two_points.0 {
            let point = graph.point(index);
            cgl_rs::graphics::widgets::add_circle2f(point.x, point.y, graph.radius * 0.9);
        }
        if let Some(index) = last_two_points.1 {
            let point = graph.point(index);
            cgl_rs::graphics::widgets::add_circle2f(point.x, point.y, graph.radius * 0.9);
        }
        
        // highlight the start and end points
        cgl_rs::graphics::widgets::set_fill_colorf(0.2, 0.6, 0.2, 1.0);
        if let Some(index) = start_point {
            let point = graph.point(index);
            cgl_rs::graphics::widgets::add_circle2f(point.x, point.y, graph.radius * 0.5);
        }
        cgl_rs::graphics::widgets::set_fill_colorf(0.6, 0.2, 0.2, 1.0);
        if let Some(index) = end_point {
            let point = graph.point(index);
            cgl_rs::graphics::widgets::add_circle2f(point.x, point.y, graph.radius * 0.5);
        }


        cgl_rs::graphics::widgets::end();


        
        window.swap_buffers();
        window.poll_events();



        // The input
        if window.is_mouse_button_pressed(cgl_rs::MouseButton::Left) {
            // Transform the mouse position to a eassier to deal with space
            let (x, y) = window.get_mouse_position();
            let (x, y) = (x as f32, y as f32);
            let point = cgl_rs::math::Vector2::new(x, y) / 800.0 * 2.0 - Vector2::new(1.0, 1.0); // here 800 is window size so keep that in mind i changing that
            let point = Vector2::new(point.x, -point.y);
            
            if active_point.is_none() { // if no points being dragged now
                active_point = graph.get_point_under_cursor(point); // check iff clicking a poiunt so pick it up for dagging
                if active_point.is_some() && last_two_points.0 != active_point { // update last 2 selected points appropiately
                    last_two_points.1 = last_two_points.0;
                    last_two_points.0 = active_point;
                }
            }

            if active_point.is_some() { // if there is a point being dragged
                // move the points with the mouse
                graph.point_mut(active_point.unwrap()).x = point.x;
                graph.point_mut(active_point.unwrap()).y = point.y;
            }
            else if has_released_since_last_press {
                graph.add_point(point); // if new click add a new point to graph at the location
            }

            has_released_since_last_press = false;
        } 
        else {
            has_released_since_last_press = true;
            active_point = None;
        }

        // to delete a point
        if window.is_mouse_button_pressed(cgl_rs::MouseButton::Right) && has_released_since_last_press {
            let (x, y) = window.get_mouse_position();
            let (x, y) = (x as f32, y as f32);
            let point = cgl_rs::math::Vector2::new(x, y) / 800.0 * 2.0 - Vector2::new(1.0, 1.0);
            let point = Vector2::new(point.x, -point.y);
            if let Some(index) = graph.get_point_under_cursor(point) {
                graph.remove_point(index);
                last_two_points = (None, None);
                if start_point == Some(index) {
                    start_point = None;
                }
                if end_point == Some(index) {
                    end_point = None;
                }

                if start_point.is_some() && start_point.unwrap() > index {
                    start_point = Some(start_point.unwrap() - 1);
                }
                if end_point.is_some() && end_point.unwrap() > index {
                    end_point = Some(end_point.unwrap() - 1);
                }

            }
        }

        if window.is_key_pressed(cgl_rs::Key::L) && has_released_since_last_press {
            if let (Some(a), Some(b)) = (last_two_points.0, last_two_points.1) {
                graph.add_connection(a, b);
            }
        }

        if window.is_key_pressed(cgl_rs::Key::S) && has_released_since_last_press {
            if let Some(index) = last_two_points.0 {
                start_point = Some(index);
            }
        }

        if window.is_key_pressed(cgl_rs::Key::E) && has_released_since_last_press {
            if let Some(index) = last_two_points.0 {
                end_point = Some(index);
            }
        }

        if window.is_key_pressed(cgl_rs::Key::C) && has_released_since_last_press {
            graph.clear();
            active_point = None;
            last_two_points = (None, None);
            start_point = None;
            end_point = None;
        }

        if window.is_key_pressed(cgl_rs::Key::Escape) {
            break;
        }


    }

    drop(main_framebuffer); // This one is not really necessary, but it's good practice to drop resources before shutting down
    cgl_rs::graphics::widgets::shutdown();
    cgl_rs::graphics::shutdown();
    drop(window); // This one is necessary, because the window must be dropped before shutting down
    cgl_rs::shutdown();
}
