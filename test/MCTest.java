import java.net.*;
import java.lang.*;
import java.io.*;

class MCTest {
	private int mcastPort;
	private InetAddress mcastIP;
	private MulticastSocket mcastSocket;
	// Boolean to tell the client to keep looping for
	// new datagrams.
	private long sleep = 15000;

	public static void main(String[] args) {
		// This must be the same port and IP address
		// used by the sender.
		if (args.length != 2) {
			System.out.print("Usage: MultiCastReceiver <IP "
			+ "addr> <port>\n\t<IP addr> can be one of "
			+ "224.x.x.x - 239.x.x.x\n");
			System.exit(1);
		}
		MCTest send = new MCTest(args);
		System.exit(0);
	}

	public MCTest(String[] args) {
		DatagramPacket mcastPacket;   // Packet to receive.
		byte[] mcastBuffer;           // byte[] array buffer
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
			System.err.println("SE: Unable to obtain socket: " 
			+ excpt);
			System.exit(1);
		} catch (IOException excpt) {
			System.err.println("IOE: Unable to obtain socket: " 
			+ excpt);
			System.exit(1);
		}
		
		long then = System.currentTimeMillis();
		long now = System.currentTimeMillis();
		while ((now - then) < sleep) {
			now = System.currentTimeMillis();
		}
		try {
			mcastSocket.leaveGroup(mcastIP); // Leave the group.
		} catch(IOException excpt) {
			System.err.println("Socket problem leaving group: "
			+ excpt);
		}
		mcastSocket.close(); // Close the socket.
	}
}
