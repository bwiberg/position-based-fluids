# Position-based fluids in OpenCL
Course assignment for CS 348C: Animation and Simulation, Fall 2016. Based on the paper [Position-Based Fluids](http://mmacklin.com/pbf_sig_preprint.pdf) by Macklin and Müller.

## Libraries
* Based on the cl-gl-bootstrap template for C++ project with OpenCL-OpenGL interoperability, written by me.
* Uses my simple OpenGL-wrapper [BWGL](https://github.com/bwiberg/bwgl)
* Uses [NanoGUI](https://github.com/wjakob/nanogui) for the UI, which in turn depends on GLFW, Eigen and NanoVG
* Uses [GLM](http://glm.g-truc.net/0.9.8/index.html) for vector- and matrix operations

## Instructions
The idea with the cl-gl-bootstrap template is to allow for multiple simulation demos to be run in the same executable. This is accomplished by having multiple "scenes" available on startup. To start the PBF-simulation scene, press the "LOAD" button in the "General Controls" UI pane and select the only available scene.

The UI displays the average time for a simulation frame (not the rendering) in the Scene Controls UI, as well as the current average FPS (which takes into account both simulation and rendering).

Recordings created by pressing the button with the record symbol are exported through FFMPEG and saved as .mp4-files in the /output folder. Beware, the average simulation time will be incorrect when recording (don't know why yet).

To load a specific fluid setup, use the buttons in the interface on the left ("Scene Controls"). The "Fluid Parameters" UI to the right can be used to adjust the fluids properties, and parameter configurations can be loaded/saved using the provided buttons. The provided file "dam-break-3.txt" works well for the fluid setups available currently.

### Parameters
* kernelRadius - The radius of influence of a particle (0.1 to 0.2 works good)
* numSubSteps - How many times the position-correction step should be done each frame (1-4 works good)
* restDensity - The density of the fluid (6000-8000 works good)
* deltaTime - The size if the timestep for each frame, in seconds (0.0083s works well, yielding 120 frames per second of simulation)
* epsilon - Constraint Force Mixing (CFM) relaxation parameter
* k - Artificial pressure strength
* delta_q  Artificial pressure radius
* n - Artificial pressure power
* c - Artificial viscosity (should be <0.05)
* k_vc - Vorticity confinement strength
* kBoundsDensity - Contribution of boundaries to a particle's density

### Controls
* SPACE (hold) - spawn particles
* Arrows - move position of spawner on the wall
* WASD - rotate camera constant amount each update (good for recordings)
* Left-click and mouse -  rotate camera freely

## Build instructions
The program has only been tested on MacOS so far, so it will probably not run on Linux or Windows without modifications to the CMake configuration and some parts of the source dode. 

The only dependency which is not included in the source code is GLM (which is found by CMake and must be installed on the machine), the rest are included as git submodules. 

1. Create a subdirectory named build
2. Run `cmake ..` and then `make` from that directory.
3. Run the program using `./pbf`. Optional program flags and arguments are:
    * `-w 1280 720` Opens the window with a resolution of 1280x270.
    * `-f`  Causes the program to run in fullscreen. Overrides the `-w` flag.
    * `-cl 0 1` Automatically selects the OpenCL context as alternative 0 and the OpenCL device as alternative 1.
    