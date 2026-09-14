# Development Log

## 2026.09.14

### Current state

* Basic Mandelbrot rendering
* Window resizing
* Zooming and panning

### Challenges I encountered

* Getting SDL to work
* Figuring out a reliable way to store the current view and map pixels to their actual coordinates
* When I first added window resizing, rendering became very laggy and slow, so I moved the fractal calculation to a separate thread
* This meant I had to learn about mutexes, threading, condition variables, generation numbers, task cancellation, and how to prevent the pixel data from being corrupted
* After adding all these features, the code had become quite messy, so I had to clean it up and reorganize it

### Next up

* Make the fractal calculation multithreaded
* Precompute coordinate arrays more efficiently