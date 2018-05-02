build:
	cc -lm -lncurses src/main.c -o bin/main
run:
	bin/main
clean:
	rm bin/*
