
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
    static final int oneRotation = 5*360;

    
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
    

		while(true){

			// Receive Instruction from Server
			direction = inFromServer.read();
			System.out.println("FROM SERVER: " + direction);
			
			DataOutputStream outToServer = new DataOutputStream(clientSocket.getOutputStream());
			

			// ----------------------------------------------------
			// Write Instruction Mapping for Robot Movement Here
			// ----------------------------------------------------

			switch(direction)
			{
			case 1:
				turn(0.5,false);
				outToServer.writeBytes("action done" + '\n');
				break;
			case 2:
				turn(-0.5,false);
				outToServer.writeBytes("action done" + '\n');
				break;
			case 3:

				break;
			case 4:

				break;
			case 5:

				break;
			}

			// Instruction to Abort Connection
			if(direction==0){
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
     * numberOfTimes : negative turns left
     * 				 : positive turns right
     * 
     * returnNow :true returns immediately
     * 
     * This function resets the TachoCount
     */			
    public static void turn(double numberOfTimes, boolean returnNow)
    {
	    leftMotor.resetTachoCount();
	    rightMotor.resetTachoCount();
	    
	    leftMotor.rotateTo((int)(oneRotation*numberOfTimes), true);
	    rightMotor.rotateTo((int)(-oneRotation*numberOfTimes), returnNow);
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
