package test;
import java.lang.*;  // Import the package names used.
import java.net.*;
import java.util.*;
import java.io.*;
/**
* This is an application which runs the 
* daytime service.
* @author David W. Baker
* @version 1.1
*/
public class DaytimeServer {
	// The daytime service runs on this well-known port.
	private static final int TIME_PORT = 2013;
	private DatagramSocket timeSocket = null;
	private static final String[] DAY_NAMES = {
		"Sunday","Monday","Tuesday","Wednesday",
		"Thursday","Friday","Saturday","Sunday" };
	private static final String[] MONTH_NAMES = {
		"January","February","March","April","May","June","July",
		"August","September","October","November","December" };
	private static final int SMALL_ARRAY = 1;
	private static final int TIME_ARRAY = 50;
	// A boolean to keep the server looping until stopped.
	private boolean keepRunning = true;
	/**
	* This method starts the application, creating and
	* instance and telling it to start accepting
	* requests.
	* @param args Command line arguments - ignored.
	*/
	public static void main(String[] args) {
		DaytimeServer server = new DaytimeServer();
		server.startServing();
	}
	/**
	* This constructor creates a datagram socket to
	* listen on.
	*/
	public DaytimeServer() {
		try {
			timeSocket = new DatagramSocket(TIME_PORT);
		} catch(SocketException excpt) {
			System.err.println("Unable to open socket: " + 
			excpt);
		}
	}
	/**
	* This method does all of the work of listening for
	* and responding to clients.
	*/
	public void startServing() {
		DatagramPacket datagram;   // For a UDP datagram.
		InetAddress clientAddr;    // Address of the client.
		int clientPort;            // Port of the client.
		byte[] dataBuffer;         // To construct a datagram.
		String timeString;         // The time as a string.
		// Keep looping while we have a socket.
		while(keepRunning) {
			try {
				// Create a DatagramPacket to receive query.
				dataBuffer = new byte[SMALL_ARRAY];
				datagram = new DatagramPacket(dataBuffer,
				dataBuffer.length);
				timeSocket.receive(datagram);
				// Get the meta-info on the client.
				clientAddr = datagram.getAddress();
				clientPort = datagram.getPort();
				// Place the time into byte array.
				dataBuffer = getTimeBuffer();
				// Create and send the datagram.
				datagram = new DatagramPacket(dataBuffer,
				dataBuffer.length,clientAddr,clientPort);
				timeSocket.send(datagram);
			} catch(IOException excpt) {
				System.err.println("Failed I/O: " + excpt);
			}
		}
		timeSocket.close();
	}
	/**
	* This method is used to create a byte array 
	* containing the current time in the special daytime
	* server format.
	* @return The byte array with the time.
	*/
	protected byte[] getTimeBuffer() {
		String timeString;
		// Get the current time.
		Date currentTime = new Date();
		int year, mon, date, day, hours, min, sec;
		byte[] timeBuffer = new byte[TIME_ARRAY];
		// Get the various portions of the current time.
		year = currentTime.getYear();
		mon = currentTime.getMonth();
		date = currentTime.getDate();
		day = currentTime.getDay();
		hours = currentTime.getHours();
		min = currentTime.getMinutes();
		sec = currentTime.getSeconds();
		year = year + 1900;
		// Create the special time format.
		timeString = DAY_NAMES[day] + ", " + MONTH_NAMES[mon] 
		+ " " + date + ", " + year + " " + hours + ":" + 
		min + ":" + sec;
		// Copy it into the byte array and return that array.
		timeString.getBytes(0,timeString.length(),
		timeBuffer,0);
		return timeBuffer;
	}
	/**
	* This method provides an interface to stopping
	* the server.
	*/
	protected void stop() {
		if (keepRunning) {
			keepRunning = false;
		}
	}
}
