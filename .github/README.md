# v10
![engine](./.github/engine.gif)

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
- [X] basic lighting
- [X] skinning
- [ ] material importing `<------ WE ARE HERE`
- [ ] post processing
- [ ] physics engine
- [ ] sound engine
- [ ] bindings for some other language (maybe lua)
