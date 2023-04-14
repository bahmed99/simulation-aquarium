import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Properties;

public class Client {
private static final String CONFIG_FILE_PATH = "./affichage.cfg";
private final Socket clientSocket;
private final PrintWriter out;
private final BufferedReader in;
public Client(String host, int port) throws IOException {
    clientSocket = new Socket(host, port);
    out = new PrintWriter(clientSocket.getOutputStream(), true);
    in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
}

public void start() {
    Thread receiveThread = new Thread(() -> {
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
    });

    Thread sendThread = new Thread(() -> {
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
    });

    receiveThread.start();
    sendThread.start();

    try {
        receiveThread.join();
        sendThread.join();
    } catch (InterruptedException e) {
        System.err.println("Error joining threads: " + e.getMessage());
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

public static void main(String[] args) {
    //extraire les informations du fichier affichage.cfg
    Properties props = new Properties();

    try (FileInputStream fis = new FileInputStream(CONFIG_FILE_PATH)) {
        props.load(fis);

        String controller_port = props.getProperty("controller-port");
        String server = props.getProperty("controller-address");
        int port = Integer.parseInt(controller_port);

        Client client = new Client(server, port);
        client.start();

    } catch (Exception e) {
        System.err.println("Error loading config file: " + e.getMessage());
    }
}
}