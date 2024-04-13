build:
	gcc -Wall -ogame -lm -lGL -lglfw -lX11 -lpthread -lXrandr -lXi -ldl src/*.c src/glad/glad.c

build-debug:
	gcc -Wall -ogame -lm -lGL -lglfw -lX11 -lpthread -lXrandr -lXi -ldl -g src/*.c src/glad/glad.c

run:
	./game

run-debug:
	valgrind --leak-check=full --log-file=valgrind.txt ./game

debug:
	gdb ./game --tui

clean:
	rm ./game
