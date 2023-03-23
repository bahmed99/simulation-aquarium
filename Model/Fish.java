public class Fish {
    private String name;
    private int weight;
    private String mobilityPattern;

    public Fish(String name, int weight, String mobilityPattern) {
        this.name = name;
        this.weight = weight;
        this.mobilityPattern = mobilityPattern;
    }

    public String getName() {
        return name;
    }

    public int getWeight() {
        return weight;
    }

    public String getMobilityPattern() {
        return mobilityPattern;
    }

    public void setName(String name) {
        this.name = name;
    }

    public void setWeight(int weight) {
        this.weight = weight;
    }

    public void setMobilityPattern(String mobilityPattern) {
        this.mobilityPattern = mobilityPattern;
    }
}
