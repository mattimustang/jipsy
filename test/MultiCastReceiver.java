package test;
//import sun.net.*;    // This will move to java.net soon.
import java.net.*;   // Import package names used.
import java.lang.*;
import java.io.*;
/**
* This is a program which allows you to listen
* at a particular multicast IP address/port and
* print out incoming UDP datagrams.
* @author David W. Baker
* @version 1.1
*/
class MultiCastReceiver {
	// The length of the data portion of incoming
	// datagrams.
	private static final int DATAGRAM_BYTES = 256;
	private int mcastPort;
	private InetAddress mcastIP;
	private MulticastSocket mcastSocket;
	// Boolean to tell the client to keep looping for
	// new datagrams.
	private boolean keepReceiving = true;
	/**
	* This starts up the application
	* @param args Program arguments - <ip> <port>
	*/
	public static void main(String[] args) {
		// This must be the same port and IP address
		// used by the sender.
		if (args.length != 2) {
			System.out.print("Usage: MultiCastReceiver <IP "
			+ "addr> <port>\n\t<IP addr> can be one of "
			+ "224.x.x.x - 239.x.x.x\n");
			System.exit(1);
		}
		MultiCastReceiver send = new MultiCastReceiver(args);
		System.exit(0);
	}
	/**
	* The constructor does the work of opening a socket,
	* joining the multicast group, and printing out
	* incoming data.
	* @param args Program arguments - <ip> <port>
	*/
	public MultiCastReceiver(String[] args) {
		DatagramPacket mcastPacket;   // Packet to receive.
		byte[] mcastBuffer;           // byte[] array buffer
		InetAddress fromIP;           // Sender address.
		int fromPort;                 // Sender port.
		String mcastMsg;              // String of message.
		try {
			// First, set up our receiving socket.
			mcastIP = InetAddress.getByName(args[0]);
			mcastPort = Integer.parseInt(args[1]);
			mcastSocket = new MulticastSocket(mcastPort);
			// Join the multicast group.
			mcastSocket.joinGroup(mcastIP);
		} catch(UnknownHostException excpt) {
			System.err.println("Unknown address: " + excpt);
			System.exit(1);
		} catch(SocketException excpt) {
			System.err.println("Unable to obtain socket: " 
			+ excpt);
			System.exit(1);
		} catch (IOException excpt) {
			System.err.println("Unable to obtain socket: " 
			+ excpt);
			System.exit(1);
		}
		while (keepReceiving) {
				try { 
				// Create a new datagram.
				mcastBuffer = new byte[DATAGRAM_BYTES];
				mcastPacket = new DatagramPacket(mcastBuffer,
				mcastBuffer.length);
				// Receive the datagram.
				mcastSocket.receive(mcastPacket);
				fromIP = mcastPacket.getAddress();
				fromPort = mcastPacket.getPort();
				mcastMsg = new String(mcastPacket.getData());
				// Print out the data.
				System.out.println("Received from " + fromIP + 
				" on port " + fromPort + ": " + mcastMsg);
			} catch(IOException excpt) {
				System.err.println("Failed I/O: " + excpt);
			}
		}
		try {
			mcastSocket.leaveGroup(mcastIP); // Leave the group.
		} catch(IOException excpt) {
			System.err.println("Socket problem leaving group: "
			+ excpt);
		}
		mcastSocket.close(); // Close the socket.
	}
	/**
	* This method provides a way to stop the program.
	*/
	public void stop() {
		if (keepReceiving) {
			keepReceiving = false;
		}
	}
}
