import java.io.*;
import java.net.*;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.Properties;

public class Client {
    private static final String CONFIG_FILE_PATH = "./affichage.cfg";
    private Socket clientSocket;
    private PrintWriter out;
    private BufferedReader in;
   
    public Client(String host, int port) {
        try {
            clientSocket = new Socket(host, port);
            out = new PrintWriter(clientSocket.getOutputStream(), true);
            in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
        } catch (UnknownHostException e) {
            System.err.println("Unknown host: " + host);
            System.exit(1);
        } catch (IOException e) {
            System.err.println("Couldn't get I/O for the connection to " + host);
            System.exit(1);
        }

        BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));
        String userInput;

        System.out.println("Type your message:");
        try {
            while ((userInput = stdIn.readLine()) != null) {
                out.println(userInput);
                System.out.println("Server response: " + in.readLine());
                System.out.println("Type your message:");
            }
        } catch (IOException e) {
            System.err.println("Error reading/writing to socket: " + e.getMessage());
        }

        try {
            out.close();
            in.close();
            clientSocket.close();
        } catch (IOException e) {
            System.err.println("Error closing resources: " + e.getMessage());
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
            //instanciation d'un client
             Client client = new Client(server, port);

            System.out.println("Controller Port: " + controller_port);
            System.out.println("Controller Address " + server);

        } catch (IOException e) {
            System.err.println("Error loading config file: " + e.getMessage());
        }



      
       

    
}
}
