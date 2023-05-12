export PATH_TO_FX=$path_to/javafx-sdk-20/lib
javac --module-path $PATH_TO_FX --add-modules javafx.controls,javafx.fxml Aquarium.java
java --module-path $PATH_TO_FX --add-modules javafx.controls Aquarium