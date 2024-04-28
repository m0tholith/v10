build:
	gcc -Wall -ogame src/*.c -Llib/ -l:libcglm.a -lassimp -lm -lGL -lglfw -lX11 -lpthread -lXrandr -lXi -ldl -Iinclude/

build-debug:
	gcc -Wall -ogame src/*.c -Llib/ -l:libcglm.a -lassimp -lm -lGL -lglfw -lX11 -lpthread -lXrandr -lXi -ldl -Iinclude/ -g

run:
	./game

run-debug:
	valgrind --leak-check=full --log-file=valgrind.txt ./game

debug:
	gdb ./game --tui

clean:
	rm ./game
