# opengl-engine
## cloning and building
only tested for linux
### dependencies
- opengl
- glfw (installed into os)
### how to build and run
```bash
git clone https://github.com/Motholith/opengl-engine
cd opengl-engine
cmake CMakeLists.txt
cmake --build .
```
### how to build and run (for debugging)
```bash
cmake CMakeLists.txt -D CMAKE_BUILD_TYPE=DEBUG
cmake --build .
```
