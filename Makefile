CC=gcc
PATH_TO_FX=/home/bully/Desktop/javafx-sdk-20/lib
BUILD_DIR=Build
CFALGS=-Wall 

all: utils test view

utils: 
	$(CC) $(CFLAGS) -c Controller/utils.c -o $(BUILD_DIR)/utils.o

test: utils
	$(CC) $(CFLAGS) -c Controller/test.c -o $(BUILD_DIR)/test.o
	$(CC) $(CFLAGS) $(BUILD_DIR)/test.o $(BUILD_DIR)/utils.o -o $(BUILD_DIR)/test

server: utils
	$(CC) $(CFLAGS) -c Controller/server.c -o $(BUILD_DIR)/server.o
	$(CC) $(CFLAGS) $(BUILD_DIR)/server.o $(BUILD_DIR)/utils.o -o $(BUILD_DIR)/server

view: 
	javac -d $(BUILD_DIR) View/Client.java 
	java -cp $(BUILD_DIR) Client

interface:
	javac --module-path $(PATH_TO_FX) --add-modules javafx.controls  -d Interface Interface/Aquarium.java
	java  --module-path $(PATH_TO_FX) --add-modules javafx.controls  -cp Interface Aquarium

clean:
	find Build/ ! \( -name '*.txt' -o -name '*.cfg' \) -type f -delete