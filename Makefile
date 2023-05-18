CC=gcc
PATH_TO_FX=/home/bully/Desktop/javafx-sdk-20/lib
BUILD_DIR=Build
CFALGS=-Wall 

all: utils test view

utils: logger_controller
	$(CC) $(CFLAGS) -c Controller/utils.c -o $(BUILD_DIR)/utils.o

test: utils
	$(CC) $(CFLAGS) -c Controller/test.c -o $(BUILD_DIR)/test.o
	$(CC) $(CFLAGS) $(BUILD_DIR)/test.o $(BUILD_DIR)/utils.o $(BUILD_DIR)/logger_controller.o -o $(BUILD_DIR)/test

server: utils logger_controller
	$(CC) $(CFLAGS) -c Controller/server.c -o $(BUILD_DIR)/server.o
	$(CC) $(CFLAGS) $(BUILD_DIR)/server.o $(BUILD_DIR)/utils.o $(BUILD_DIR)/logger_controller.o -o $(BUILD_DIR)/server

view: 
	javac --module-path /usr/share/openjfx/lib --add-modules javafx.controls,javafx.fxml -d $(BUILD_DIR) View/Client.java 
	java  --module-path /usr/share/openjfx/lib --add-modules javafx.controls,javafx.fxml -cp $(BUILD_DIR) Client


interface:
	javac --module-path $(PATH_TO_FX) --add-modules javafx.controls  -d Interface Interface/Aquarium.java
	java  --module-path $(PATH_TO_FX) --add-modules javafx.controls  -cp Interface Aquarium


logger_controller:
	$(CC) $(CFLAGS) -c Controller/logger.c -o $(BUILD_DIR)/logger_controller.o

clean:
	find Build/ \( -iname '*.class' -o -iname '*.o' \) -type f -delete