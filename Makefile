# options
CC=clang
DEBUG?=yes
ENABLE_ERRORCHECKING?=yes
CREATE_SO?=
GENERATE_COMPILE_FLAGS_FILE=yes
OPTIONS=
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

BINARY_NAME=v10
LIBRARY_NAME=libv10.so
PKGCONFIG_NAME=v10.pc
RESULT=$(BINARY_NAME)
INSTALL_DIR=/usr/local

# generated flags
OBJECTS=$(patsubst %.c,$(BUILD_DIR)/%.o,$(CFILES))
DEPFILES=$(patsubst %.c,$(BUILD_DIR)/%.d,$(CFILES))
LIBS=gl glfw3 cglm assimp
INCLUDE_OPTS=$(foreach DIR,$(INCLUDE_DIRS),-I$(DIR))
ifeq ($(DEBUG), )
else
	OPTIONS+=-g
endif
CFLAGS=-Wall -Werror -MMD $(INCLUDE_OPTS) $(OPTIONS) \
	   $(shell pkg-config --cflags $(LIBS))
LDFLAGS=-lm $(shell pkg-config --libs $(LIBS))
ifeq ($(CREATE_SO), )
	CFILES+=src/main.c
else
	RESULT=$(LIBRARY_NAME)
	OPTIONS+=-fpic
	LDFLAGS+=-shared
endif

# colors for printing
COUT_NORMAL=\e[0m
COUT_GREEN=\e[32m
COUT_YELLOW=\e[33m

# misc
COMPILE_FLAGS_FILE=compile_flags.txt

all: $(COMPILE_FLAGS_FILE) $(RESULT)

$(RESULT): $(OBJECTS)
	@printf "$(COUT_GREEN)Linking $@$(COUT_NORMAL)\n"
	@$(CC) -o $@ $^ $(LDFLAGS)
	@printf "$(COUT_GREEN)Linked $@$(COUT_NORMAL)\n"

$(BUILD_DIR)/%.o: %.c
	@printf "$(COUT_GREEN)Compiling $<$(COUT_NORMAL) ($(COUT_YELLOW)$@$(COUT_NORMAL))\n"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c -o $@ $<

$(COMPILE_FLAGS_FILE):
	@if [ "$(GENERATE_COMPILE_FLAGS_FILE)" != "" ]; then \
		printf "$(COUT_GREEN)Generating $(COMPILE_FLAGS_FILE)$(COUT_NORMAL)\n";\
		echo "$(INCLUDE_OPTS)" > $(COMPILE_FLAGS_FILE);\
		if [ "$(DEBUG)" != "" ]; then \
			echo "-D DEBUG" >> $(COMPILE_FLAGS_FILE);\
		fi;\
		if [ "$(ENABLE_ERRORCHECKING)" != "" ]; then \
			echo "-D ENABLE_ERRORCHECKING" >> $(COMPILE_FLAGS_FILE);\
		fi;\
		printf "$(COUT_GREEN)Generated $(COMPILE_FLAGS_FILE)$(COUT_NORMAL)\n";\
	fi

clean:
	rm -rf $(BINARY_NAME) $(LIBRARY_NAME) $(BUILD_DIR) $(COMPILE_FLAGS_FILE)

install: $(LIBRARY_NAME) $(PKGCONFIG_NAME)
	mkdir -p $(INSTALL_DIR)/include/v10 $(INSTALL_DIR)/lib/pkgconfig
	cp $(LIBRARY_NAME) $(INSTALL_DIR)/lib/
	cp -r $(foreach DIR,$(INCLUDE_DIRS),$(INCLUDE_DIRS)/*) $(INSTALL_DIR)/include/
	sed -i "1s~.*~prefix=$(INSTALL_DIR)~" $(PKGCONFIG_NAME)
	cp $(PKGCONFIG_NAME) $(INSTALL_DIR)/lib/pkgconfig

-include $(DEPFILES)

.PHONY: all clean
