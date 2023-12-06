### To run:
navigate into the `src` folder and run `make && ./Cell_Visualizer` in the terminal

### To use:
- Press the numbers 0-8 to see the different organelles
- Press ESC to get to the cell view (unimplemented), and then again to exit the program.
- Click and drag mouse to rotate camera
- Space to pause/unpause time
- W/S to move time (+shift to move by a full second)

### Current state:
- the ribosome and mitochondrion are the most complete organelelles (2 and 7).
- for the golgi apparatus (8), I implemented raymarching to account for its unique shape and transport of vesicles throughout. Using raymarching allows for the smooth blending of the vesicles entering/exiting the golgi, and made it much easier to create from simple primitives, since I did not want to use modelling programs for this project.