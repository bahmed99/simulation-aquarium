public class View {
    private String name;
    private Coordinates coordinates;
    private int width;
    private int height;
    private int numFishes;
    private Fish[] fishes;

    public View(String name, Coordinates coordinates, int width, int height, int numFishes, Fish[] fishes) {
        this.name = name;
        this.coordinates = coordinates;
        this.width = width;
        this.height = height;
        this.numFishes = numFishes;
        this.fishes = fishes;
    }

    public String getName() {
        return name;
    }

    public Coordinates getCoordinates() {
        return coordinates;
    }

    public int getWidth() {
        return width;
    }

    public int getHeight() {
        return height;
    }

    public int getNumFishes() {
        return numFishes;
    }

    public Fish[] getFishes() {
        return fishes;
    }

    public void setName(String name) {
        this.name = name;
    }

    public void setCoordinates(Coordinates coordinates) {
        this.coordinates = coordinates;
    }

    public void setWidth(int width) {
        this.width = width;
    }

    public void setHeight(int height) {
        this.height = height;
    }

    public void setNumFishes(int numFishes) {
        this.numFishes = numFishes;
    }

    public void setFishes(Fish[] fishes) {
        this.fishes = fishes;
    }
}
    