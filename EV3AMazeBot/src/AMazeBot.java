
import lejos.hardware.motor.Motor;
import lejos.robotics.RegulatedMotor;
import lejos.utility.Delay;
import lejos.hardware.Button;
import lejos.hardware.Sound;
import lejos.hardware.ev3.LocalEV3;
import lejos.hardware.lcd.Font;
import lejos.hardware.lcd.GraphicsLCD;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.*;

/**
 * Program that runs the AMazeBot
 * Simple locomotion testing
 * @author Austin
 *
 */
public class AMazeBot
{
    static RegulatedMotor leftMotor = Motor.A;
    static RegulatedMotor rightMotor = Motor.B;
    static final int oneRotation = 5;

    
    public static final String SERVER_IP = "192.168.137.32";
	public static final Integer SERVER_PORT = 6789;
    
	@SuppressWarnings("resource")
	public static void main(String[] args) throws IOException
    {
	    leftMotor.resetTachoCount();
	    rightMotor.resetTachoCount();
	    leftMotor.rotateTo(0);
	    rightMotor.rotateTo(0);
	    leftMotor.setSpeed(400);
	    rightMotor.setSpeed(400);
	    leftMotor.setAcceleration(800);
	    rightMotor.setAcceleration(800);

	    String msgFromServer;
	    Integer direction;
		Socket clientSocket = null;
		
        GraphicsLCD g = LocalEV3.get().getGraphicsLCD();
        final int SW = g.getWidth();
        final int SH = g.getHeight();
        

		// open a New Connection with Server
		try {
			clientSocket = new Socket(SERVER_IP, SERVER_PORT);
		} catch (Exception e) {
			// TODO: handle exception
			System.out.println("Cannot connect to Server " + SERVER_IP + ":" + SERVER_PORT);
			e.printStackTrace();
		}
		
		if(clientSocket !=null){

			System.out.println("Connected to Server " + SERVER_IP + ":" + SERVER_PORT);

			// Send first message to Server 
			DataOutputStream outToServer = new DataOutputStream(clientSocket.getOutputStream());
			outToServer.writeBytes("Hi.! I'm EV3.." + '\n');

		}

		// Receive Acknowledgment from Server
		BufferedReader inFromServer = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
		msgFromServer = inFromServer.readLine();
		Button.LEDPattern(5);
        Delay.msDelay(100);
    
        DataOutputStream outToServer = new DataOutputStream(clientSocket.getOutputStream());
		while(true){

			// Receive Instruction from Server
			direction = inFromServer.read();

			//direction = Integer.parseInt(inFromServer.readLine());
			//System.out.println("FROM SERVER Raw: " + msgFromServer);
			System.out.println("FROM SERVER Int: " + direction);
			

			// ----------------------------------------------------
			// Write Instruction Mapping for Robot Movement Here
			// ----------------------------------------------------
			if(direction <= 90)
			{
				//we are expecting values from 0 to 90
				//turn function turns in degrees
				//0 to 45 is mapped to -180 to 0
				//46 to 90 is mapped to 1 to 180
				System.out.println("Turning: "+(direction-45)*4+"degrees");
				turn((direction-45)*4,false);
				Delay.msDelay(100);
				outToServer.writeBytes("action done" + '\n');
			}else
			if(direction == 92)
			{
				forward(100,false);
				outToServer.writeBytes("action done" + '\n');
			}
			else
			if(direction == 93)
			{
				reverse(100,false);
				outToServer.writeBytes("action done" + '\n');
			}
			else
			{
				outToServer.writeBytes("action not done - unexpected value" + '\n');
			}

			// Instruction to Abort Connection
			if(direction==91){
				System.out.println("Received Instruction to Abort..");
				break;
			}

		}	

		// Close Client Socket
		clientSocket.close();
        
        //turn(0.5,false); //turns bot 180 degrees right
        //turn(-0.5,false); //turns bot 180 degress left
        //forward(1,false);
        //reverse(1,false);
        
	    //leftMotor.rotateTo(0, true);// start Motor.A rotating backward
	    //rightMotor.rotateTo(0);

    }
	
    /**
     * degrees 	: negative turns left
     * 			: positive turns right
     * 
     * returnNow :true returns immediately
     * 
     * This function resets the TachoCount
     */			
    public static void turn(double degrees, boolean returnNow)
    {
	    leftMotor.resetTachoCount();
	    rightMotor.resetTachoCount();
	    
	    leftMotor.rotateTo((int)(oneRotation*degrees), true);
	    rightMotor.rotateTo((int)(-oneRotation*degrees), returnNow);
    }
    
    public static void forward(double distance, boolean returnNow)
    {
	    leftMotor.resetTachoCount();
	    rightMotor.resetTachoCount();
	    
	    leftMotor.rotateTo((int)(oneRotation*distance), true);
	    rightMotor.rotateTo((int)(oneRotation*distance), returnNow);
    }
    
    public static void reverse(double distance, boolean returnNow)
    {
	    leftMotor.resetTachoCount();
	    rightMotor.resetTachoCount();
	    
	    leftMotor.rotateTo((int)(-oneRotation*distance), true);
	    rightMotor.rotateTo((int)(-oneRotation*distance), returnNow);
    }
    
	
}
