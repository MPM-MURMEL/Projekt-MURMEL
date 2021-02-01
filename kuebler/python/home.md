# Progress

```mermaid
gantt
dateFormat  YYYY-MM-DD

requirement definition: FK1, 2018-04-23, 6w
research: FK2, after FK1, 8w
actuator selection: FK3, after FK2, 2w
sensor selection: FK4, after FK2, 2w
actuator acquisition: FK5, after FK3, 15w
sensor acquisition: FK6, after FK4, 2w
construction of test bench: FK7, after FK6, 4w

sensor software concept: FK7, after FK6, 18w

actuator software concept:         FK8, after FK5, 12w
implementation of objective function:       FK9, after FK8, 16w
evaluation and bug-fixing:         FK10, after FK9, 6w
documentation: FK11, after FK6, 54w

```

# Software

This repository contains the arm-keyhole coordination that can be divided into two sub categories.

## Controller

Based on the Kinova Jaco2 this module consists of a TCP client,
multiple PID controllers and the Kinova API in order to link between
camera signals and robot motion.
A detailed source code documentation can be found [here](Controller).

### Dependencies

All the instructions needed can be found [here](Dependencies).

1. Kinova API
2. Eigen
3. JsonCpp


```mermaid
graph TD
A[main] --> B[initialize Kinova Jaco 2]
B --> C{is Jaco 2 connected}
C --> |YES|D[initialize tcp client]
C --> |NO|E[error state]
D --> F{is tcp client connected}
F --> |NO|D
F --> |YES|H[read message from server]
H --> I[process json string]
I --> J[calculte trajectory]
J --> K[send trajectory to robot]
K --> F
```


## Camera

Using the open source computer vision library OpenCV this module
calculates information about the keyhole position and orientation.
All information are passed to the Controller via the integrated TCP server.
A detailed source code documentation can be found [here](Camera).

### Dependencies

All the instructions needed can be found [here](Dependencies).

1. OpenCV
3. JsonCpp


```mermaid
graph TD
A[main] --> B[initialize tcp server]
B --> C{is client connected}
C --> |YES|D[read frame from camera]
C --> |NO|B
D --> F[process frame]
F --> G[calculate orientation and position]
G --> H[parse to json string]
H --> I[send to tcp client]
I --> C
```


# Dependencies

In order to build this project successfully a couple
of libraries need to be installed.
All the instructions needed can be found in the [here](Dependencies).


# Tutorials

### Build project with make

To simplify the build process of c/c++ applications a makefile can be used.
This file contains all the instructions necessary to manage and build the project.
Instructions can be found [here](Makeinstructions).

### Code Commenting

Doxygen needs certain style of comments in order to generate a documentation.
This section will demonstrate the documentation convention für c++ header and source files.
More detailed instructions can be found [here](Codecommenting).

### Doxygen

In order to generate a code dokumentation for a given project this
section will provide detailed instructions on all
nesseccary tools and how to use them.
Instructions can be found [here](Doxygeninstructions).

### Markdown

Mermaid can be used in markdown files to generate graphs, time tables and more.
Further instnstructions and examples can be found [here](https://mermaidjs.github.io/).

### Python

For teaching and prototyping purposes a small collection of
python examples can be found [here](Doxygeninstructions)
