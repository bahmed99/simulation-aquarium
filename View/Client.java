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
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Client extends Application{
    private static String CONFIG_FILE_PATH = "./affichage.cfg";
    private Socket clientSocket;
    private PrintWriter out;
    private BufferedReader in;
    private Group fishGroup;
    private ImageView backgroundImageView;
    private String id ;
    private String controller_port;
    private String display_timeout_value;

    public static void main(String[] args) {
            launch(args);
        }

    @Override
    public void start(Stage stage) {
        Properties props = new Properties();
        try (FileInputStream fis = new FileInputStream(CONFIG_FILE_PATH)) {
            props.load(fis);

            controller_port = props.getProperty("controller-port");
            String server = props.getProperty("controller-address");
            id = props.getProperty("id");
            display_timeout_value = props.getProperty("display-timeout-value");
            
            int port = Integer.parseInt(controller_port);

            clientSocket = new Socket(server, port);
            out = new PrintWriter(clientSocket.getOutputStream(), true);
            in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            if(id != null)
                out.println("hello in as " + id);
            else 
                out.println("hello");

        } catch (Exception e) {
            System.err.println("Error loading config file: " + e.getMessage());
        }

        Image backgroundImage = new Image("Images/aquarium.jpg");
        backgroundImageView = new ImageView(backgroundImage);
        fishGroup = new Group();
        Scanner scanner = new Scanner(System.in);
        String userInput;
        Group root = new Group(backgroundImageView, fishGroup);
            
        Scene scene = new Scene(root, 1000, 1000);
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

        Thread pingThread = new Thread(new PingRunnable());
        pingThread.setDaemon(true);
        pingThread.start();
    }

    private class PingRunnable implements Runnable{
        @Override
        public void run() {
            while(true){
                try {
                    Thread.sleep(Integer.parseInt(display_timeout_value));
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                    out.println("ping "+controller_port);
            }
        }
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
                    //fishTransition.setCycleCount(Animation.INDEFINITE);
                    Point2D debut_fishPosition = fishPositions.get(fishImageView);
                    fishTransition.setFromX(debut_fishPosition.getX());
                    fishTransition.setFromY(debut_fishPosition.getY());
                    double gotoX = 0;
                    double gotoY = 0;
                    RandomWayPoint(gotoX, gotoY);
                    fishTransition.setToX(gotoX);
                    fishTransition.setToY(gotoY);
                    if (gotoX < debut_fishPosition.getX()) {
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
                    ExtractFish(userInput);
                    out.println(userInput);
                }
            } catch (IOException e) {
                System.err.println("Error sending message: " + e.getMessage());
            }
        }
    }

    public void RandomWayPoint(double gotoX, double gotoY) {
        gotoX = Math.random()*500; 
        gotoY = Math.random()*400;
    }
    public void ExtractFish(String userInput) {
        Pattern pattern = Pattern.compile("addFish\\s+(\\w+)\\s+at\\s+(\\d+x\\d+),\\s*(\\d+x\\d+)(?:,\\s*(\\w+))?");
        Matcher matcher = pattern.matcher(userInput);
        if (matcher.find()) {
            String fishName = matcher.group(1);
            String StartingPosition = matcher.group(2);
            String FinalPosition = matcher.group(2);
            String movement = matcher.group(4);
            Pattern coordinatePattern = Pattern.compile("(\\d+)x(\\d+)");

            Matcher startCoordinateMatcher = coordinatePattern.matcher(StartingPosition);
            int startX = 0;
            int startY = 0;
            if (startCoordinateMatcher.find()) {
                startX = Integer.parseInt(startCoordinateMatcher.group(1));
                startY = Integer.parseInt(startCoordinateMatcher.group(2));
            }
            Matcher finalCoordinateMatcher = coordinatePattern.matcher(FinalPosition);
            int finalX = 0;
            int finalY = 0;
            if (finalCoordinateMatcher.find()) {
                finalX = Integer.parseInt(finalCoordinateMatcher.group(1));
                finalY = Integer.parseInt(finalCoordinateMatcher.group(2));
            }
            String FishPath = "Images/Fish/"+fishName+".png";
            Image fishImage = new Image(FishPath, 100, 100, false, false);
            ImageView fishImageView = new ImageView(fishImage);
            fishImageView.setX(startX);
            fishImageView.setY(startY);
            Platform.runLater(() -> {fishGroup.getChildren().add(fishImageView);});
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