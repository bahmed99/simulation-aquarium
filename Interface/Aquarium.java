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
        Image backgroundImage = new Image("aquarium.jpg");
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
            for (Node node : fishGroup.getChildren()) {
                ImageView fishImageView = (ImageView) node;
                Point2D fishPosition = new Point2D(0, 200);
                fishPositions.put(fishImageView, fishPosition);
            }
        
            while (true) {
                for (Node node : fishGroup.getChildren()) {
                    ImageView fishImageView = (ImageView) node;
                    TranslateTransition fishTransition = new TranslateTransition(Duration.seconds(5), fishImageView);
                    fishTransition.setAutoReverse(true);
                    fishTransition.setCycleCount(Animation.INDEFINITE);
                    Point2D fishPosition = fishPositions.get(fishImageView);
                    fishTransition.setFromX(0);//fishPosition.getX());
                    fishTransition.setFromY(0);//fishPosition.getY());
                    double gotoX = Math.random()*500;
                    double gotoY = Math.random()*400;
                    fishTransition.setToX(gotoX);
                    fishTransition.setToY(gotoY);
                    fishPosition = new Point2D(gotoX, gotoY);
                    fishPositions.put(fishImageView, fishPosition);
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
            //String userInput;
            while (true) {
                    System.out.print("Enter path to fish image (or 'quit' to exit): ");
                    String userInput = scanner.nextLine();

                    if (!userInput.equalsIgnoreCase("quit")) {
                        // add fish to aquarium
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
    /*    do {
            System.out.print("Enter path to fish image (or 'quit' to exit): ");
            userInput = scanner.nextLine();
            
            if (!userInput.equalsIgnoreCase("quit")) {
                // add fish to aquarium
                Image fishImage = new Image(userInput,100, 100, false, false);
                ImageView fishImageView = new ImageView(fishImage);
                fishImageView.setX(0); // random x coordinate
                fishImageView.setY(0); // random y coordinate
                fishGroup.getChildren().add(fishImageView);
                TranslateTransition fish2Transition = new TranslateTransition(Duration.seconds(5), fishImageView);
                fish2Transition.setAutoReverse(true);
                fish2Transition.setCycleCount(Animation.INDEFINITE);
                fish2Transition.setFromX(0);
                fish2Transition.setFromY(200);
                fish2Transition.setToX(500);
                fish2Transition.setToY(300);
                fish2Transition.play();
            }
        } while (!userInput.equalsIgnoreCase("quit"));
    }*/
}
