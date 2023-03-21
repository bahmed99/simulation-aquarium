
all: utils test server view

utils: 
	gcc -c Controller/utils.c -o Build/utils.o

test: utils
	gcc -c Controller/test.c -o Build/test.o
	gcc Build/test.o Build/utils.o -o Build/test
server: utils
	gcc -c Controller/server.c -o Build/server.o
	gcc Build/server.o Build/utils.o -o Build/server

view: 
	javac -d Build View/Client.java 
	java -cp Build Client
	
clean:
	rm -rf Build/*