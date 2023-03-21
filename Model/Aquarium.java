public class Aquarium {
    private String name;
    private int[] dimensions;
    private View[] views;
    private int numViews;

    public Aquarium(String name, int[] dimensions, View[] views, int numViews) {
        this.name = name;
        this.dimensions = dimensions;
        this.views = views;
        this.numViews = numViews;
    }

    public String getName() {
        return name;
    }

    public int[] getDimensions() {
        return dimensions;
    }

    public View[] getViews() {
        return views;
    }

    public int getNumViews() {
        return numViews;
    }

    public void setName(String name) {
        this.name = name;
    }

    public void setDimensions(int[] dimensions) {
        this.dimensions = dimensions;
    }

    public void setViews(View[] views) {
        this.views = views;
    }

    public void setNumViews(int numViews) {
        this.numViews = numViews;
    }
}
