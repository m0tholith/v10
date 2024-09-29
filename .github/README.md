# v10

https://github.com/user-attachments/assets/332aae46-85f4-4ef9-bcb2-15f34ef7c5d5

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
    - [ ] spot light `<------ WE ARE HERE`
    - [X] dedicated lighting system
- [X] skinning
- [X] material importing
    - [X] color importing (diffuse, ambient, etc.)
    - [ ] texture importing
- [ ] post processing
    - [ ] multi-aliasing
    - [ ] bloom
    - [ ] post processing pipeline
- [ ] physics engine
- [ ] sound engine
- [ ] bindings for some other language (maybe lua)
