# opengl-engine
## cloning and building
only tested for linux
### dependencies
- opengl
- glfw
- cglm
- assimp
### how to build and run
```bash
git clone https://github.com/Motholith/opengl-engine
cd opengl-engine
cmake CMakeLists.txt
cmake --build .
```
### how to build and run (for debugging)
this enables the `-g` option when running the compiler
```bash
cmake CMakeLists.txt -D CMAKE_BUILD_TYPE=DEBUG
cmake --build .
```
## nix-shell
if you use nix you can use the `shell.nix` file to enter a devshell with all the required dependencies:
```bash
nix-shell
```
if you use `direnv` then you probably don't need me to tell you that the devshell will automatically be enabled
