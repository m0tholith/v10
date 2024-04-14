# opengl-engine
## cloning and building
only tested for linux
### dependencies
- opengl
- glfw (installed into os)
- valgrind (debugging)
- gdb (debugging)
### how to build and run
```bash
git clone https://github.com/Motholith/opengl-engine
cd opengl-engine
git submodule update --init
make build  # builds project
make run    # runs ./game
```
### how to build and run (for debugging)
```bash
make build-debug  # builds the project with gcc -g flag
make run-debug    # runs valgrind
make debug        # runs gdb
```
