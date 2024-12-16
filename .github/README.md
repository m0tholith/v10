# v10

https://github.com/user-attachments/assets/32c6f81a-cf73-45a3-ac15-49bd188f7093

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
make
./v10
```
### build and run (debug)
(this is already enabled by default)

edit `Makefile` and change the value of `DEBUG` (any value means enabling, no value means disabling), then compile the project again:
```bash
make clean
make
```
alternatively, you can momentarily change the value of the `DEBUG` variable:
```bash
make clean
make DEBUG=yes
```
## nix shell
if you use nix you can use the `flake.nix` file to enter a devshell with all the required dependencies:
```bash
nix develop
```
if you use `nix-direnv` then you can call `direnv allow` to automatically enter a devshell when entering the project directory
## goals
- [ ] 3D renderer
    - [X] model loading
    - [X] shader cache
    - [X] texture cache
    - [X] input system
    - [X] lighting
        - [X] point light
        - [X] directional light
        - [X] spot light
        - [X] dedicated lighting system
        - [X] shadows
            - [X] directional light shadows
            - [X] point light shadows
            - [X] spot light shadows
    - [X] skinning
    - [X] material importing
        - [X] color importing (diffuse, ambient, etc.)
        - [X] texture importing
    - [X] skybox
    - [X] refactoring
        - [X] optimizations
            - [X] improved window/input systems
            - [X] camera UBO
            - [X] light scene UBO
            - [X] shaders
                - [X] shader cache
                - [X] uniform cache
            - [X] texture cache
            - [X] cubemap cache
        - [X] documentation
    - [ ] text rendering
    - [ ] sprites
    - [ ] post processing
        - [ ] multi-aliasing
        - [ ] bloom
        - [ ] post processing pipeline
    - [ ] convert to vulkan
- [ ] serialization
- [ ] physics engine
- [ ] sound engine
- [ ] particle system
- [ ] visual effects system
