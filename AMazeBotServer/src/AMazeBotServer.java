import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.*;

public class AMazeBotServer {
	public static String FILE = "/home/austin/Git/Robodoro/vision/directions";
	public static final Integer SERVER_PORT = 6789;

	public static void main(String[] args) throws IOException 
	{ 
		String clientMsg;
		ServerSocket welcomeSocket = new ServerSocket(SERVER_PORT);

		BufferedReader reader = null;

		String line = null;
		int input = 0;

		try {
			reader = new BufferedReader(new FileReader(FILE));
			System.out.println("Instruction File found..");
			reader.close();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			System.out.println("Exception in opening file..");
			e.printStackTrace();
		}

		System.out.println("Waiting for Client at port: " + SERVER_PORT);

		// Accept any Incoming Connection from Client
		Socket connectionSocket = welcomeSocket.accept();

		// Get an initial message from Client
		BufferedReader inFromClient = new BufferedReader(new InputStreamReader(connectionSocket.getInputStream()));
		clientMsg = inFromClient.readLine();
		System.out.println("Received from Client: " + clientMsg);

		System.out.println("Acknowledging Client..");

		// Acknowledge to Client
		DataOutputStream outToClient = new DataOutputStream(connectionSocket.getOutputStream());
		outToClient.writeBytes("Hello EV3.!\n");

		System.out.println("Sending Instructions to Client..");

		while(true){             
			// Open File object again if it is NULL / Closed 
			try {
				reader = new BufferedReader(new FileReader(FILE));
			} catch (FileNotFoundException e) {
				System.out.println("Exception in opening file..");
				e.printStackTrace();
			}

			// Read the first line of the Instruction File
			try {
				line = reader.readLine();
			} catch (IOException e) {
				//e.printStackTrace();
			}

			// Convert Instruction to Integer
			try {
				input = Integer.valueOf(line);
			} catch (NumberFormatException e) {
				input = 1;
				//e.printStackTrace();
			}

			// Send Input to Client
			System.out.println("Sending: " + input);
			outToClient.write(input);

			
			//wait for instruction to complete execution
			clientMsg = inFromClient.readLine();
			System.out.println("Bot says: " + clientMsg);

			if(input==0){
				System.out.println("Stopping Instructions to Client..");
				break;
			}

			input = 0;
			line = null;

			try {
				Thread.sleep(500);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}

			// Close File Object
			try {
				reader.close();
			} catch (IOException e) {
				System.out.println("Exception in closing file..");
				e.printStackTrace();
			}

		}

		// Close Client Connection Socket 
		connectionSocket.close();

		// Close Server Socket
		welcomeSocket.close();

	} 

}
