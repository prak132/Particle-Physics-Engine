# Particle Physics Engine

Simple particle physics simulation program using the SFML library. The aim of this program is for me to learn how to utilize the library. Simulating the movement of particles with random velocities within the window. When particles reach the edges of the window, they bounce off, creating a visually interesting simulation.

## Prerequisites
Before running this code, make sure you have the SFML library installed. On MacOS you can install [homebrew](https://brew.sh/) and follow their install guide then run:

```shell
brew install sfml
```

## Building and Running
To compile and then run the program you need to use an [IDE](https://en.wikipedia.org/wiki/Integrated_development_environment) like [Visual Studio Code](https://code.visualstudio.com/) then go into terminal and run this command:

```shell
g++ main.cpp -o build -std=c++17 -I/opt/homebrew/include -L/opt/homebrew/opt/sfml/lib -lsfml-graphics -lsfml-window -lsfml-system
```

Then to compile it run:

```shell
./build
```
