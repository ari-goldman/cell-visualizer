### To run:
navigate into the `src` folder and run `make && ./Cell_Visualizer` in the terminal

### To use:
- Press the numbers 1-3 to see the different organelles
- Press ESC to exit the program.
- Click and drag mouse to rotate camera
- Space to pause/unpause time
- W/S to move time (+shift to move by a full second)

### Current state:
- 1: mitochondrion - this is by far the simplest thing drawn in the project, as it's just half a pill with flat area in it. This *is* supposed to be cartoony, so I drew lightning coming off of it every second or so
- 2: golgi apparatus - since I refrained from using any sort of 3D modelling program for this, I figured the best way to represent this funky structure is using a raymarched scene, which this is. It also allows for easy blending of the vesicles into the cisternae
- 3: ribosome - this infinitely shows a ribosome transcribing each set of 3 bases in mRNA and creating a protein chain out of it. This was particularly challenging, as I had no clue how to do the animation portion allowing to loop infinitely, but since I've figured out, it's become pretty clear and easy to udnerstand how it works!

<img width="400" alt="mitochondrion" src="https://github.com/ari-goldman/cell-visualizer/assets/112101394/72008ccb-2512-42bf-a894-19d5cba942fb">
<img width="400" alt="golgi_app" src="https://github.com/ari-goldman/cell-visualizer/assets/112101394/dcf053ac-b831-4e4a-947a-9d79d5f467c7">
<img width="400" alt="ribosome" src="https://github.com/ari-goldman/cell-visualizer/assets/112101394/dd3eb9bf-b043-4f41-a745-0258fba2aec4">

