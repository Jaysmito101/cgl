# 2D Collision Detection Demo with CGL

This post explores a 2D collision detection demonstration application implemented using the CGL library. The source code provided is a self-contained C program that creates a window, draws multiple shapes, and performs collision detection between a user-controlled “cursor” shape and several other shapes. Two collision detection algorithms are available: the **Gilbert–Johnson–Keerthi (GJK)** algorithm and the **Separating Axis Theorem (SAT)**. In this post, we delve into what the application is, how to control it, the concepts behind the collision detection algorithms, detailed implementation notes, and ideas for extending the project.

---

## What It Is

This application is a graphical demonstration of 2D collision detection algorithms implemented using the CGL (C Graphics Library) framework. The code initializes a window and renders multiple polygonal shapes on the screen. One shape, termed the “cursor,” is controlled by the user and can be moved, rotated, and scaled interactively. The program continuously checks for collisions between the cursor and the other shapes using either the GJK or SAT method. When a collision is detected, the application provides visual feedback by changing stroke colors and drawing collision normals.

---

## How to Use / Controls

The application offers a set of keyboard controls for interacting with the cursor shape and toggling settings:

- **Movement:**  
  - **Arrow Keys:** Move the cursor shape up, down, left, or right.
- **Rotation:**  
  - **Z Key:** Rotate the cursor shape.
- **Scaling:**  
  - **W Key:** Increase the scale (enlarge the cursor).  
  - **S Key:** Decrease the scale (shrink the cursor).
- **Algorithm Toggle:**  
  - **G Key:** Switch to using the GJK collision detection algorithm.  
  - **T Key:** Switch to using the SAT collision detection algorithm.
- **Collision Resolution:**  
  - **R Key:** If a collision is detected, the cursor is moved away from colliding shapes by subtracting the computed normal vector.
- **Exit:**  
  - **Escape Key:** Close the application.

These controls allow the user to interact with the simulation in real time and observe the behavior of the collision detection algorithms under various conditions.

---

## Concepts / Algorithms Used in Details

### Gilbert–Johnson–Keerthi (GJK) Algorithm

The GJK algorithm is a popular method for collision detection between convex shapes. In this application:
- **Simplex Construction:** A simplex (a set of up to three points in 2D) is built iteratively.
- **Support Function:** The algorithm uses a support function to find extreme points in a given direction.
- **Collision Check:** Once the simplex is constructed, the algorithm checks whether the origin lies inside the simplex, indicating a collision.
- **EPA (Expanding Polytope Algorithm):** When a collision is detected, the EPA is used to compute the collision normal, which is then used to resolve or visualize the collision.

### Separating Axis Theorem (SAT)

The SAT is another method for collision detection between convex shapes:
- **Axis Generation:** The theorem checks for a potential separating axis between two convex shapes by projecting their vertices onto various axes (often normals of the shape edges).
- **Overlap Test:** If there exists any axis along which the projections of the two shapes do not overlap, then the shapes are not colliding.
- **Collision Normal:** When the shapes do overlap along all tested axes, a collision is detected, and the minimum overlap vector is computed as the collision normal.

Both algorithms provide robust means to determine collisions, though GJK (with EPA) is more generally used in physics engines for its efficiency and accuracy with convex objects.

---

## Implementation Details (Nitty Gritty)

The implementation leverages several aspects of the CGL library:

1. **Initialization:**
   - The program begins with library initialization (`CGL_init`, `CGL_gl_init`, etc.) and creates a window of size 700×700 pixels.
   - A framebuffer is set up to render the shapes.
   - Random shapes are generated with varying vertex counts and positioned on a grid layout, while one extra shape (the cursor) is set up separately for user control.

2. **Shape Setup:**
   - Shapes are initialized using `CGL_shape_init` where the vertices are computed based on cosine and sine functions to form regular polygons.
   - The cursor shape is differentiated by its unique scale and central position at `(0, 0)`.

3. **Rendering and Collision Detection Loop:**
   - The main loop (or the WASM loop if compiled for WebAssembly) handles frame updates.
   - For each frame, the cursor is compared against each of the other shapes:
     - **GJK or SAT:** Depending on the user-selected mode (`g_State.use_gjk`), the appropriate collision detection function is invoked.
     - **Visual Feedback:** If a collision is detected, the stroke color changes, and additional lines are drawn to indicate the direction of the collision normal.
   - The code also accumulates normal vectors from multiple collisions to adjust the cursor position when the `R` key is pressed, thus providing a simple collision resolution mechanism.

4. **User Input Handling:**
   - Keyboard inputs are polled each frame to update the cursor’s position, rotation, and scale.
   - The application uses a time delta (`g_State.delta_time`) to ensure smooth and consistent movement across different frame rates.

5. **Cleanup:**
   - Upon exit (or when the window is closed), resources such as shapes, framebuffer, and context are properly released.

---

## Conclusion

This 2D collision detection demo showcases how to integrate interactive graphics, physics-based collision detection, and user input within a single application using the CGL library. The use of two different algorithms (GJK with EPA and SAT) not only demonstrates the versatility of collision detection techniques but also provides insights into their practical application. The clean separation of initialization, rendering, input handling, and cleanup phases makes this code an excellent example of modular design in graphics programming.

---

## More Ideas to Extend This

There are several potential extensions and improvements to this project:

- **Enhanced Collision Response:**  
  Implement more advanced collision response techniques (e.g., realistic bouncing, friction, or energy conservation) to simulate physical interactions between objects.

- **Additional Shapes and Dynamics:**  
  Introduce different types of shapes, including concave polygons or even circle primitives. Adding dynamic behaviors like velocity and acceleration can create a richer simulation.

- **3D Collision Detection:**  
  Extend the concept to 3D collision detection using GJK and EPA for 3D shapes, providing a basis for a simple 3D physics engine.

- **Graphical User Interface (GUI):**  
  Develop an interactive GUI that allows users to dynamically add, remove, or modify shapes during runtime, as well as switch between different collision detection algorithms.

- **Optimizations:**  
  Investigate spatial partitioning techniques (like quad-trees) to improve performance when dealing with a large number of shapes.

- **Integration with Game Engines:**  
  Integrate the collision detection module into a more extensive game engine framework to handle more complex interactions and scenarios.

---

This demonstration serves as a practical introduction to collision detection techniques and interactive graphics programming. It invites further exploration and experimentation, whether you are interested in developing game mechanics or building physics simulations from scratch.
