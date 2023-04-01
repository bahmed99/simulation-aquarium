CC=gcc
PATH_TO_FX=/home/bully/Desktop/javafx-sdk-20/lib
all: utils test  view

utils: 
	$(CC) -c Controller/utils.c -o Build/utils.o

test: utils
	$(CC) -c Controller/test.c -o Build/test.o
	$(CC) Build/test.o Build/utils.o -o Build/test
server: utils
	$(CC) -c Controller/server.c -o Build/server.o
	$(CC) Build/server.o Build/utils.o -o Build/server

view: 
	javac -d Build View/Client.java 
	java -cp Build Client
interface:
	javac --module-path $(PATH_TO_FX) --add-modules javafx.controls  -d Interface Interface/Aquarium.java
	java  --module-path $(PATH_TO_FX) --add-modules javafx.controls  -cp Interface Aquarium
clean:
	rm -rf Build/*