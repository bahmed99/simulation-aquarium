import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Properties;
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

public class Client extends Application{
    private static String CONFIG_FILE_PATH = "./affichage.cfg";
    private Socket clientSocket;
    private PrintWriter out;
    private BufferedReader in;
    private Group fishGroup;
    private ImageView backgroundImageView;


    public static void main(String[] args) {
            launch(args);
        }

    @Override
    public void start(Stage stage) {
        Properties props = new Properties();
        try (FileInputStream fis = new FileInputStream(CONFIG_FILE_PATH)) {
            props.load(fis);

            String controller_port = props.getProperty("controller-port");
            String server = props.getProperty("controller-address");
            
            int port = Integer.parseInt(controller_port);

            clientSocket = new Socket(server, port);
            out = new PrintWriter(clientSocket.getOutputStream(), true);
            in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));

        } catch (Exception e) {
            System.err.println("Error loading config file: " + e.getMessage());
        }

        Image backgroundImage = new Image("Images/aquarium.jpg");
        backgroundImageView = new ImageView(backgroundImage);
        fishGroup = new Group();
        Scanner scanner = new Scanner(System.in);
        String userInput;
        Group root = new Group(backgroundImageView, fishGroup);
            
        Scene scene = new Scene(root, 600, 420);
        stage.setScene(scene);
        stage.show();


        Thread aquariumThread = new Thread(new AquariumRunnable());
        aquariumThread.setDaemon(true);
        aquariumThread.start();

        Thread senderThread = new Thread(new SenderRunnable());
        senderThread.setDaemon(true);
        senderThread.start();

        Thread receiverThread = new Thread(new ReceiverRunnable());
        receiverThread.setDaemon(true);
        receiverThread.start();
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
                    TranslateTransition fishTransition = new TranslateTransition(Duration.seconds(2.5), fishImageView);
                    //fishTransition.setCycleCount(Animation.INDEFINITE);
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
    private class ReceiverRunnable implements Runnable {
        @Override
        public void run() {
            try {
                char[] buffer = new char[1024];
                int bytesRead;
                while ((bytesRead = in.read(buffer)) != -1) {
                    String response = new String(buffer, 0, bytesRead);
                    System.out.print("  -> ");
                    System.out.println(response.trim());
        

                    System.out.print("$ ");
                }
            } catch (IOException e) {
                System.err.println("Error receiving message: " + e.getMessage());
            }
        }
    }
    private class SenderRunnable implements Runnable {
        @Override
        public void run() {
            try {
                BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));
                String userInput;
                System.out.print("$ ");
                while ((userInput = stdIn.readLine()) != null) {
                    out.println(userInput);
                }
            } catch (IOException e) {
                System.err.println("Error sending message: " + e.getMessage());
            }
        }
    }

public void close() {
    try {
        out.close();
    } catch (Exception e) {
    System.err.println("Error closing output stream: " + e.getMessage());
    }
    try {
        in.close();
    } catch (Exception e) {
        System.err.println("Error closing input stream: " + e.getMessage());
    }
    try {
        clientSocket.close();
    } catch (Exception e) {
        System.err.println("Error closing socket: " + e.getMessage());
    }
}


}