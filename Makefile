# options
CC=clang
DEBUG?=yes
CREATE_SO?=
ARGS=
OPTIONS=ENABLE_ERRORCHECKING
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
INCLUDE_DIRS=include

INCLUDE_FILES=$(foreach DIR,$(INCLUDE_DIRS),$(wildcard $(DIR)/**/*.h))

BINARY_NAME=v10
LIBRARY_NAME=libv10.so
PKGCONFIG_NAME=v10.pc
RESULT=$(BINARY_NAME)
INSTALL_DIR=/usr/local

# generated flags
OBJECTS=$(patsubst %.c,$(BUILD_DIR)/%.o,$(CFILES))
DEPFILES=$(patsubst %.c,$(BUILD_DIR)/%.d,$(CFILES))
PCHFILES=$(patsubst %.h,$(BUILD_DIR)/%.pch,$(INCLUDE_FILES))
LIBS=gl glfw3 cglm assimp
INCLUDE_OPTS=$(foreach DIR,$(INCLUDE_DIRS),-I$(DIR))
ifeq ($(DEBUG), )
else
	ARGS+=-g
endif
CFLAGS=-Wall -Werror -MMD $(INCLUDE_OPTS) $(ARGS) \
	   $(shell pkg-config --cflags $(LIBS))
LDFLAGS=-lm $(shell pkg-config --libs $(LIBS))
PCHFLAGS=$(foreach PCH,$(PCHFILES),-include-pch $(PCH))
OPTFLAGS=$(foreach OPT,$(OPTIONS),-D$(OPT))
ifeq ($(CREATE_SO), )
	CFILES+=src/main.c
else
	RESULT=$(LIBRARY_NAME)
	ARGS+=-fpic
	LDFLAGS+=-shared
endif

# colors for printing
COUT_NORMAL=\e[0m
COUT_GREEN=\e[32m
COUT_YELLOW=\e[33m

all: $(RESULT)

$(RESULT): $(OBJECTS) $(PCHFILES)
	@printf "$(COUT_GREEN)Linking $@$(COUT_NORMAL)\n"
	@$(CC) -o $@ $^ $(LDFLAGS)
	@printf "$(COUT_GREEN)Linked $@$(COUT_NORMAL)\n"

$(BUILD_DIR)/%.o: %.c $(PCHFILES)
	@printf "$(COUT_GREEN)Compiling $<$(COUT_NORMAL) ($(COUT_YELLOW)$@$(COUT_NORMAL))\n"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) $(PCHFLAGS) $(OPTFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.pch: %.h
	@printf "$(COUT_GREEN)Compiling $<$(COUT_NORMAL) ($(COUT_YELLOW)$@$(COUT_NORMAL))\n"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) $(OPTFLAGS) -c -o $@ $<

compile_commands.json:
	make clean
	bear -- make $(RESULT)

clean:
	rm -rf $(BINARY_NAME) $(LIBRARY_NAME) $(BUILD_DIR) compile_commands.json

install: $(LIBRARY_NAME) $(PKGCONFIG_NAME)
	mkdir -p $(INSTALL_DIR)/include/v10 $(INSTALL_DIR)/lib/pkgconfig
	cp $(LIBRARY_NAME) $(INSTALL_DIR)/lib/
	cp -r $(foreach DIR,$(INCLUDE_DIRS),$(INCLUDE_DIRS)/*) $(INSTALL_DIR)/include/
	sed -i "1s~.*~prefix=$(INSTALL_DIR)~" $(PKGCONFIG_NAME)
	cp $(PKGCONFIG_NAME) $(INSTALL_DIR)/lib/pkgconfig

-include $(DEPFILES)

.PHONY: all clean
