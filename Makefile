build:
	gcc -Wall -ogame -lm -lGL -lglfw -lX11 -lpthread -lXrandr -lXi -ldl src/*.c src/glad/glad.c

build-debug:
	gcc -Wall -ogame -lm -lGL -lglfw -lX11 -lpthread -lXrandr -lXi -ldl -ggdb3 src/*.c src/glad/glad.c

run:
	./game

clean:
	rm ./game
