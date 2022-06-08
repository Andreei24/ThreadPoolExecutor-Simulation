build:
	gcc -Wall -std=c99 main.c Helpers.c functiiCoada-V.c functiiStiva-V.c -o tema2

run:
	./tema2

clean:
	rm tema2