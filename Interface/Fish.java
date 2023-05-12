import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

public class Fish { 

    private String pathImage;
    private double startX;
    private double startY;
    private String mobilityPattern;

    public Fish(String pathImage, double startX, double startY, String mobilityPattern) {
        this.pathImage = pathImage;
        this.startX = startX;
        this.startY = startY;
        this.mobilityPattern = mobilityPattern;
        
    }
    public ImageView CreateFish() {
        Image fishImage = new Image(pathImage,100, 100, false, false);
        ImageView fishImageView = new ImageView(fishImage);
        fishImageView.setX(startX);
        fishImageView.setY(startY);
        return fishImageView;
    }

    public double getStartX() {
        return this.startX;
    }

    public double getStartY() {
        return this.startY;
    }

    public String getMobilityPattern() {
        return mobilityPattern;
    }
}   