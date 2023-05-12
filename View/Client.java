import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Properties;
import java.util.Scanner;
import java.util.Map;
import java.util.HashMap;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.TimeUnit;

import javafx.scene.layout.Pane;
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
import javafx.util.Pair;
import javafx.scene.Node;
import javafx.geometry.Point2D;



public class Client extends Application {
    private static String CONFIG_FILE_PATH = "./Build/affichage.cfg";
    private Socket clientSocket;
    private PrintWriter out;
    private BufferedReader in;
    private Group fishGroup;
    private ImageView backgroundImageView;
    private Group extractedImageViews;
    private String id ;
    private String controller_port;
    private String display_timeout_value;
    private boolean gFC = false;
    private String resources;
    
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
            resources= props.getProperty("resources");
            
            int port = Integer.parseInt(controller_port);

            clientSocket = new Socket(server, port);
            out = new PrintWriter(clientSocket.getOutputStream(), true);
            in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            if(id != null)
                out.println("hello in as " + id);
            else 
                out.println("hello");

            out.println("getFishesContinuously");

        } catch (Exception e) {
            System.err.println("Error loading config file: " + e.getMessage());
        }

        Image backgroundImage = new Image("Images/aquarium.jpg");
        backgroundImageView = new ImageView(backgroundImage);
        fishGroup = new Group();
        extractedImageViews = new Group();
        Scanner scanner = new Scanner(System.in);
        String userInput;
        Group root = new Group(backgroundImageView, extractedImageViews);
            
        Scene scene = new Scene(root, 500, 500);
        // stage.setResizable(false);
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
            while (true) {
                
                for (Node node : fishGroup.getChildren()) {
                    Pane container = (Pane) node;
                    List<Object> fish = (List<Object>) container.getUserData();
                    ImageView fishImageView = (ImageView) fish.get(0);
                    List<Integer> destinationsX = (List<Integer>) fish.get(2);
                    List<Integer> destinationsY = (List<Integer>) fish.get(3);
                    boolean started = (boolean) fish.get(4);
                    if (started == true && destinationsX.size() >= 2 && destinationsY.size() >= 2) {
                        System.out.println("LOOL");
                        TranslateTransition fishTransition = new TranslateTransition(Duration.seconds(5), fishImageView);
                        fishTransition.setFromX(destinationsX.get(0));
                        fishTransition.setFromY(destinationsY.get(0));
                        fishTransition.setToX(destinationsX.get(1));
                        fishTransition.setToY(destinationsY.get(1));
                        
                        if (destinationsX.get(1) < destinationsX.get(0)) {
                            fishImageView.setScaleX(-1);
                        } else {
                            fishImageView.setScaleX(1);
                        }
                        fishTransition.play();


                        List<Object> newFish = new ArrayList<>();

                        destinationsX.remove(0);
                        destinationsY.remove(0);
                        
                        newFish.add(fish.get(0));
                        newFish.add(fish.get(1));
                        newFish.add(destinationsX);
                        newFish.add(destinationsY);
                        newFish.add(true);

                        Pane newContainer = new Pane();
                        newContainer.setUserData(newFish);
                        Platform.runLater(() -> {
                            int index = fishGroup.getChildren().indexOf(container);
                            fishGroup.getChildren().set(index, newContainer);
                            UpdateImageViewsGroup();
                        });
                        
                    
                    }
                    
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
                    if(!response.equals("list "))
                  {      System.out.print("   \n-> ");
                        System.out.println(response.trim());
                        // if (gFC == true) {
                            getFishesContinuously(response);
                 }
                    // }

                    
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
                    addFish(userInput);
                    StartFish(userInput);
                    delFish(userInput);
                    // if (userInput.equals("getFishesContinuously")) {
                    //     gFC = true;
                    // }
                    out.println(userInput);
                }
            } catch (IOException e) {
                System.err.println("Error sending message: " + e.getMessage());
            }
        }
    }
    private void printExtractedImageViews(Group extractedImageViews) {
        for (Node node : extractedImageViews.getChildren()) {
            ImageView imageView = (ImageView) node;
            Image image = imageView.getImage();
            String imageUrl = image.getUrl();
            
            System.out.println("Image URL: " + imageUrl);
            System.out.println("Position: " + imageView.getLayoutX() + ", " + imageView.getLayoutY());
        }
    }


    private void printFishGroup(Group fishGroup) {
        for (Node node : fishGroup.getChildren()) {
            Pane container = (Pane) node;
            List<Object> fish = (List<Object>) container.getUserData();
            ImageView fishImageView = (ImageView) fish.get(0);
            Image fishImage = fishImageView.getImage();
            String fishImageUrl = fishImage.getUrl();
            boolean started = (boolean) fish.get(1);
            
            System.out.println("Fish image URL: " + fishImageUrl);
            System.out.println("Started: " + started);
            System.out.println("Position: " + container.getLayoutX() + ", " + container.getLayoutY());
        }
    }

    public void RandomWayPoint(double gotoX, double gotoY) {
        gotoX = Math.random()*500; 
        gotoY = Math.random()*400;
    }

    public void UpdateImageViewsGroup() {
        extractedImageViews.getChildren().clear();
        for (Node node : fishGroup.getChildren()) {
            Pane container = (Pane) node;
            List<Object> fishData = (List<Object>) container.getUserData();
            ImageView fishImageView = (ImageView) fishData.get(0);
            extractedImageViews.getChildren().add(fishImageView);
        }
    }

    public void StartFish(String userInput) {
        Pattern pattern = Pattern.compile("startFish (\\w+)");
        Matcher matcher = pattern.matcher(userInput);
        if (matcher.find()) {
            String fishName = matcher.group(1);
            for (Node node : fishGroup.getChildren()) {
                Pane container = (Pane) node;
                List<Object> fish = (List<Object>) container.getUserData();
                if (fishName.equals(fish.get(1))) { 
                    Platform.runLater(() -> {
                        fishGroup.getChildren().remove(container);
                        List<Object> newFish = new ArrayList<>();
                        newFish.add(fish.get(0));
                        newFish.add(fish.get(1));
                        newFish.add(fish.get(2));
                        newFish.add(fish.get(3));
                        newFish.add(true);
                        Pane new_container = new Pane();
                        new_container.setUserData(newFish);
                        fishGroup.getChildren().add(new_container);
                        UpdateImageViewsGroup();
                    });
                    break;
                }
            }
        }
    }

    public void addFish(String userInput) {
        Pattern pattern = Pattern.compile("addFish\\s+(\\w+)\\s+at\\s+(-?\\d+x-?\\d+),\\s*(\\d+x\\d+)(?:,\\s*(\\w+))?");
        Matcher matcher = pattern.matcher(userInput);
        if (matcher.find()) {
            String fishName = matcher.group(1);
            String StartingPosition = matcher.group(2);
            String FinalPosition = matcher.group(2);
            String movement = matcher.group(4);
            Pattern coordinatePattern = Pattern.compile("(-?\\d+)x(-?\\d+)");

            Matcher startCoordinateMatcher = coordinatePattern.matcher(StartingPosition);
            int startX = 0;
            int startY = 0;
            if (startCoordinateMatcher.find()) {
                startX = Integer.parseInt(startCoordinateMatcher.group(1));
                startY = Integer.parseInt(startCoordinateMatcher.group(2));
            }
            Matcher finalCoordinateMatcher = coordinatePattern.matcher(FinalPosition);
            int sizeX = 0;
            int sizeY = 0;
            if (finalCoordinateMatcher.find()) {
                sizeX = Integer.parseInt(finalCoordinateMatcher.group(1));
                sizeY = Integer.parseInt(finalCoordinateMatcher.group(2));
            }
            String FishPath = "Images/Fish/"+fishName+".png";
            Image fishImage = new Image(FishPath, 100, 100, false, false);
            ImageView fishImageView = new ImageView(fishImage);
            fishImageView.setX(startX);
            fishImageView.setY(startY);
            List<Object> fish = new ArrayList<>();
            List<Integer> destinationsX = new ArrayList<>();
            List<Integer> destinationsY = new ArrayList<>();
            fish.add(fishImageView);
            fish.add(fishName);
            fish.add(destinationsX);
            fish.add(destinationsY);
            fish.add(false);
            Pane container = new Pane();
            container.setUserData(fish);
            Platform.runLater(() -> {
                fishGroup.getChildren().add(container);
                UpdateImageViewsGroup();
            });
        }
    }

    public void delFish(String userInput){

        Pattern pattern = Pattern.compile("delFish (\\w+)");
        Matcher matcher = pattern.matcher(userInput);
        if (matcher.find()) {
            String fishName = matcher.group(1);
            for (Node node : fishGroup.getChildren()) {
                Pane container = (Pane) node;
                List<Object> fish = (List<Object>) container.getUserData();
                if (fishName.equals(fish.get(1))) { 
                    Platform.runLater(() -> {
                        fishGroup.getChildren().remove(container);
                        UpdateImageViewsGroup();
                    });
                    break;
                }
            }
        }
    }

    public void getFishesContinuously(String userInput) {
        Pattern pattern = Pattern.compile("\\[(.*?) at (-?\\d+)x(-?\\d+), ?(\\d+)x(\\d+), ?(\\d+)(?:, ?(\\d+))?\\]");
        Matcher matcher = pattern.matcher(userInput);
        boolean created = true;
        while (matcher.find()) {
            String fishName = matcher.group(1);
            int posX = Integer.parseInt(matcher.group(2));
            int posY = Integer.parseInt(matcher.group(3));
            System.out.println("posX: " + posX);
            System.out.println("posY: " + posY);

            int sizeX = Integer.parseInt(matcher.group(4));
            int sizeY = Integer.parseInt(matcher.group(5));
            int time = Integer.parseInt(matcher.group(6));
            for (Node node : fishGroup.getChildren()) {
                Pane container = (Pane) node;
                List<Object> fish = (List<Object>) container.getUserData();
                if (fishName.equals(fish.get(1))) {
                    List<Integer> destinationsX = (List<Integer>) fish.get(2);
                    List<Integer> destinationsY = (List<Integer>) fish.get(3);
                    if(posX<0 && posY<0){
                        destinationsX.add(500);
                        destinationsY.add(500);
                    }
                    else if(posX<0 && posY>0){
                        destinationsX.add(500);
                        destinationsY.add(posY);
                    }
                    else if(posX>=0 && posY<0){
                        destinationsX.add(posX);
                        destinationsY.add(500);
                    }
                    else{
                        destinationsX.add(posX);
                        destinationsY.add(posY);
                    }
                    List<Object> newfish = new ArrayList<>();
                    newfish.add(fish.get(0));
                    newfish.add(fish.get(1));
                    newfish.add(destinationsX);
                    newfish.add(destinationsY);
                    newfish.add(fish.get(4));

                    Pane newContainer = new Pane();
                    newContainer.setUserData(newfish);
                    created = false;
                    Platform.runLater(() -> {
                        int index = fishGroup.getChildren().indexOf(container);
                        fishGroup.getChildren().set(index, newContainer);
                        UpdateImageViewsGroup();
                    });
            }
        }
        if (created) {
            String FishPath = resources + "/Fish/" + fishName + ".png";
            Image fishImage = new Image(FishPath, 100, 100, false, false);
            ImageView fishImageView = new ImageView(fishImage);
            List<Object> newfish = new ArrayList<>();
            List<Integer> destinationsX = new ArrayList<>(List.of(posX));
            List<Integer> destinationsY = new ArrayList<>(List.of(posY));
            
            fishImageView.setX(destinationsX.get(0));
            fishImageView.setY(destinationsY.get(0));

            newfish.add(fishImageView);
            newfish.add(fishName);
            newfish.add(destinationsX);
            newfish.add(destinationsY);
            newfish.add(true);

            Pane container = new Pane();
            container.setUserData(newfish);
            Platform.runLater(() -> {
                fishGroup.getChildren().add(container);
                UpdateImageViewsGroup();
            });
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

