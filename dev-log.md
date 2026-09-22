# Development Log

## 14.09.2026

### Current state

- Basic Mandelbrot rendering
- Window resizing
- Zooming and panning

### Challenges I encountered

- Getting SDL to work
- Figuring out a reliable way to store the current view and map pixels to their actual coordinates
- When I first added window resizing, rendering became very laggy and slow, so I moved the fractal calculation to a separate thread
- This meant I had to learn about mutexes, threading, condition variables, generation numbers, task cancellation, and how to prevent the pixel data from being corrupted
- After adding all these features, the code had become quite messy, so I had to clean it up and reorganize it

### Next up

- Make the fractal calculation multithreaded
- Precompute coordinate arrays more efficiently

## 22.09.2026

### What I did

- Created benchamrk program to measure mean time
- Made fractal calculation multithreaded
- Removed task cancellation because it complicated things too much and was also bad for performance
- Made zooming centered on mouse
- Made coloring use smooth iteration to hide the iteration bands a little bit
- Optimized by removing unnecessary square root when iterating

These changes made the renderer 16× faster in my benchmark.

### Chalenges I encountered

- The image was glitching when resizing the window because of a race condition, so it would display an old image with the new dimensions
- When splitting the image into regions it would leave a black margin on the left and bottom side of the image, because of a rounding error

### Next up

- Rewrite coloring algorithm to use a cycling palette, so it could display shapes to infinity
- Dynamic max iterations, so it would compute more as needed when zooming in
- Automatic worker threads number based on the cpu threads
- Further optimizations