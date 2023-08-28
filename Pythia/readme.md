# PYTHIA8 simulation

This is the PYTHIA8 script used to generate data of jets in simulation.

## Installation

- PYTHIA8 and Fastjet3 are required to compile.
- Makefile should be copied from example_files (generated with compilation of PYTHIA8)
- Makefile.inc gives an example of how tom link the Fastjet3 lib to the PYTHIA8 script.

## Execution

- If the Makefile and Makefile.inc are properly set, type in "make main00" and then an executable file "main00" will be generated.
- Proton-proton collisions are simulated in this script. See details in the report.
- Type in "./main00" to run the simulation. The program will read the settings from "input.dat". Note that the "thread" option only changes the suffix of the generated data files. If you need multithreading, just duplicate several executable files and run them each with different number in the "thread" option.

## Simulation Data

See an example from trainingData_thread4.txt. Data of a jet are organized as:  

(Number of constituents) (particle ID of flavor parton) (px of jet) (py of jet) (pz of jet) (E of jet)
(Constituent 1st) (particle ID of this constituent) (px) (py) (pz) (E) (vertex_x) (vertex_y) (vertex_z) (vertex_t)  
(Constituent 2nd) (particle ID of this constituent) (px) (py) (pz) (E) (vertex_x) (vertex_y) (vertex_z) (vertex_t)  
(Constituent 3rd) (particle ID of this constituent) (px) (py) (pz) (E) (vertex_x) (vertex_y) (vertex_z) (vertex_t)  
...  
(Number of constituents) (particle ID of flavor parton) (px of jet) (py of jet) (pz of jet) (E of jet)  
(Constituent 1st) (particle ID of this constituent) (px) (py) (pz) (E) (vertex_x) (vertex_y) (vertex_z) (vertex_t)  
(Constituent 2nd) (particle ID of this constituent) (px) (py) (pz) (E) (vertex_x) (vertex_y) (vertex_z) (vertex_t)  
(Constituent 3rd) (particle ID of this constituent) (px) (py) (pz) (E) (vertex_x) (vertex_y) (vertex_z) (vertex_t)  
(Constituent 4th) (particle ID of this constituent) (px) (py) (pz) (E) (vertex_x) (vertex_y) (vertex_z) (vertex_t)  
...  
