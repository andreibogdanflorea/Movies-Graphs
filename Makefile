build: 
	gcc -Wall -g main.c util.c -o movies

clean:
	rm -rf movies *.o