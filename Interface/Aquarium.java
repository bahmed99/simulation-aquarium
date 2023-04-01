import javafx.animation.Animation;
import javafx.animation.TranslateTransition;
import javafx.application.Application;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.application.Platform;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.stage.Stage;
import javafx.util.Duration;
import java.util.Scanner;
import javafx.scene.Node;
import java.util.Map;
import java.util.HashMap;
import javafx.geometry.Point2D;

public class Aquarium extends Application {
    
    private Group fishGroup;
    private ImageView backgroundImageView;

    public static void main(String[] args) {
        launch(args);
    }
    @Override
    public void start(Stage stage) {
        
        // create an Image for the background of the aquarium
        Image backgroundImage = new Image("Images/aquarium.jpg");
        backgroundImageView = new ImageView(backgroundImage);
        fishGroup = new Group();
        Scanner scanner = new Scanner(System.in);
        String userInput;
        // create a Group to hold the background and fishGroup
        Group root = new Group(backgroundImageView, fishGroup);
        
        // create a Scene with the root Group and show it
        Scene scene = new Scene(root, 600, 420);
        stage.setScene(scene);
        stage.show();


        Thread aquariumThread = new Thread(new AquariumRunnable());
        aquariumThread.setDaemon(true);
        aquariumThread.start();

        Thread addFishThread = new Thread(new AddFishRunnable());
        addFishThread.setDaemon(true);
        addFishThread.start();
    }

    private class AquariumRunnable implements Runnable {
        @Override
        public void run() {
            Map<ImageView, Point2D> fishPositions = new HashMap<>();
        
            while (true) {
                for (Node node : fishGroup.getChildren()) {
                ImageView fishImageView = (ImageView) node;
                if (fishPositions.get(fishImageView) == null) {
                    Point2D fishPosition = new Point2D(0, 200);
                    fishPositions.put(fishImageView, fishPosition);
                    }
                }
                for (Node node : fishGroup.getChildren()) {
                    ImageView fishImageView = (ImageView) node;
                    TranslateTransition fishTransition = new TranslateTransition(Duration.seconds(5), fishImageView);
                    fishTransition.setCycleCount(Animation.INDEFINITE);
                    Point2D debut_fishPosition = fishPositions.get(fishImageView);
                    fishTransition.setFromX(debut_fishPosition.getX());
                    fishTransition.setFromY(debut_fishPosition.getY());
                    double gotoX = Math.random()*500; // Random movements
                    double gotoY = Math.random()*400;
                    fishTransition.setToX(gotoX);
                    fishTransition.setToY(gotoY);
                    if (gotoX < debut_fishPosition.getX()) { // Flipping image
                        fishImageView.setScaleX(-1);
                    } else {
                        fishImageView.setScaleX(1);
                    }
                    Point2D new_fishPosition = new Point2D(gotoX, gotoY);
                    fishPositions.put(fishImageView, new_fishPosition);
                    fishTransition.play();
                }
                try {
                    Thread.sleep(5000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    private class AddFishRunnable implements Runnable {
        @Override
        public void run() {
            Scanner scanner = new Scanner(System.in);
            while (true) {
                    System.out.print("Enter path to fish image (or 'quit' to exit): ");
                    String userInput = scanner.nextLine();

                    if (!userInput.equalsIgnoreCase("quit")) {
                        Image fishImage = new Image(userInput, 100, 100, false, false);
                        ImageView fishImageView = new ImageView(fishImage);
                        fishImageView.setX(0);
                        fishImageView.setY(200);
                        Platform.runLater(() -> {fishGroup.getChildren().add(fishImageView);});
                    } else {
                        break;
                    }
            }
        }
    }
}
