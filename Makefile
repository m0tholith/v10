# set to anything and it'll still work
DEBUG?=yes
CREATE_SO?=

CFILES=src/v10/model.c \
       src/v10/cubemap.c \
       src/v10/armature.c \
       src/v10/light.c \
       src/v10/node.c \
       src/v10/rendering.c \
       src/v10/camera.c \
       src/v10/glad.c \
       src/v10/input.c \
       src/v10/window.c \
       src/v10/error.c \
       src/v10/texture.c \
       src/v10/mesh.c \
       src/v10/animation.c \
       src/v10/shader.c \
       src/v10/stb_image.c \
       src/v10/entity.c \
       src/v10/framebuffer.c \
       src/v10/skybox.c \
       src/v10/material.c
BUILD_DIR=build
OBJECTS=$(patsubst %.c,$(BUILD_DIR)/%.o,$(CFILES))
DEPFILES=$(patsubst %.c,$(BUILD_DIR)/%.d,$(CFILES))
BINARY=v10

CC=clang
INCLUDE_DIRS=include
LIBS=gl glfw3 cglm assimp
OPTIONS=
ifeq ($(DEBUG), )
else
	OPTIONS+=-g
endif
CFLAGS=-Wall -Werror $(foreach DIR,$(INCLUDE_DIRS),-I$(DIR)) $(OPTIONS) \
	   $(shell pkg-config --cflags $(LIBS)) -MMD
LDFLAGS=-lm $(shell pkg-config --libs $(LIBS))
ifeq ($(CREATE_SO), )
	CFILES+=src/main.c
else
	BINARY:::=lib$(BINARY).so
	OPTIONS+=-fpic
	LDFLAGS+=-shared
endif

# colors for printing
COUT_NORMAL=\e[0m
COUT_GREEN=\e[32m
COUT_YELLOW=\e[33m

all: $(BINARY)

$(BINARY): $(OBJECTS)
	@printf "Linking $(COUT_GREEN)$@$(COUT_NORMAL)\n"
	@$(CC) -o $@ $^ $(LDFLAGS)
	@printf "Linked $(COUT_GREEN)$@$(COUT_NORMAL)\n"

$(BUILD_DIR)/%.o: %.c
	@printf "$(COUT_GREEN)Compiling $<$(COUT_NORMAL) ($(COUT_YELLOW)$@$(COUT_NORMAL))\n"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(BINARY) lib$(BINARY).so $(BUILD_DIR)

-include $(DEPFILES)

.PHONY: all clean
