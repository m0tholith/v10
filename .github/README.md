# v10

https://github.com/user-attachments/assets/b5e7abf0-5e9b-47e2-a90e-0e9e199b3b05

an opengl game engine/framework
## cloning and building
(only tested for linux)
### dependencies
- opengl 4.6
- glfw
- cglm
- assimp
### build and run
```bash
git clone https://github.com/m0tholith/v10
cd v10
cmake CMakeLists.txt
cmake --build .
```
### build and run (debug)
(this is already enabled by default)
edit `CMakeLists.txt` and enable `BUILD_DEBUG`, then compile the project again
## nix shell
if you use nix you can use the `flake.nix` file to enter a devshell with all the required dependencies:
```bash
nix develop
```
if you use `nix-direnv` then you can call `direnv allow` to automatically enter a devshell when entering the project directory
## goals
- [X] model loading
- [X] shader cache
- [X] texture cache
- [X] input system
- [X] lighting
    - [X] point light
    - [X] directional light
    - [X] spot light
    - [X] dedicated lighting system
    - [ ] shadows `<------ WE ARE HERE`
- [X] skinning
- [X] material importing
    - [X] color importing (diffuse, ambient, etc.)
    - [X] texture importing
- [ ] post processing
    - [ ] multi-aliasing
    - [ ] bloom
    - [ ] post processing pipeline
- [ ] physics engine
- [ ] sound engine
- [ ] bindings for some other language (maybe lua)
