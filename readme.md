# Particle Physics Engine

Simple particle physics simulation program using the SFML library. The aim of this program is for me to learn how to utilize the library, and optimize the code to reach faster/less updates with the same output. Simulating the movement of particles with random velocities within the window. When particles reach the edges of the window or other particles, they bounce off, creating a visually interesting simulation.

* `Simulation.cpp` This file encompasses the code for a straightforward physics particle simulation. In this simulation, particles exhibit bouncing behavior off the edges of the window and interactions with other particles.

* `Gravity.cpp` This file encompasses the code for a physics particle simulation with a gravitational element. In this simulation, a gravitational force centered at the window's midpoint influences the particles. Tweaking the gravitational constant in this scenario yields intriguing outcomes.

## Prerequisites
Before running this code, make sure you have the SFML library installed. On MacOS you can install [homebrew](https://brew.sh/) and follow their install guide then run:

```shell
brew install sfml
```

#### You need to download [glm](https://github.com/g-truc/glm/releases) unzip it and put it in this directory for Simulation to work.

## Building and Running
To compile and then run the program you need to use an [IDE](https://en.wikipedia.org/wiki/Integrated_development_environment) like [Visual Studio Code](https://code.visualstudio.com/) then go into terminal and run this command and replace `<file.name>` to the file name:

```shell
g++ <file.name> -o build -std=c++20 -O2 -I. -I/opt/homebrew/include -L/opt/homebrew/opt/sfml/lib -lsfml-graphics -lsfml-window -lsfml-system
```

Then to compile it run:

```shell
./build
```
