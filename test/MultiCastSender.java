package test;
//import sun.net.*;    // This will move to java.net soon.
import java.net.*;   // Import package names used.
import java.lang.*;
import java.io.*;
/**
 * This is a program which sends data from the command
 * line to a particular multicast group.
 * @author David W. Baker
 * @version 1.1
 */
class MultiCastSender {
   // The number of Internet routers through which this
   // message should be passed. Keep this low. 1 is good
   // for local LAN communications.
   private static final byte TTL = 1;
   // The size of the data sent - basically the maximum
   // length of each line typed in at a time.
   private static final int DATAGRAM_BYTES = 256;
   private int mcastPort;
   private InetAddress mcastIP;
   private DataInputStream input;
   private MulticastSocket mcastSocket;
   /**
    * This starts up the application.
    * @param args Program arguments - <ip> <port>
    */
   public static void main(String[] args) {
      // This must be the same port and IP address used
      // by the receivers.
      if (args.length != 2) {
         System.out.print("Usage: MultiCastSender <IP addr>"
            + " <port>\n\t<IP addr> can be one of 224.x.x.x "
            + "- 239.x.x.x\n");
         System.exit(1);
      }
      MultiCastSender send = new MultiCastSender(args);
      System.exit(0);
   }
   /**
    * The constructor does all of the work of opening
    * the socket and sending datagrams through it.
    * @param args Program arguments - <ip> <port>
    */
   public MultiCastSender(String[] args) {
      DatagramPacket mcastPacket;   // UDP datagram.
      String nextLine;              // Line from STDIN.
      byte[] mcastBuffer;           // Buffer for datagram.
      int sendLength;               // Length of line.
      input = new DataInputStream(System.in);
      try {
         // Create a multicasting socket.
         mcastIP = InetAddress.getByName(args[0]);
         mcastPort = Integer.parseInt(args[1]);
         mcastSocket = new MulticastSocket();
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
      try { 
         // Loop and read lines from standard input.
         while ((nextLine = input.readLine()) != null) {
            mcastBuffer = new byte[DATAGRAM_BYTES];
            // If line is longer than our buffer, use the
            // length of the buffer available.
            if (nextLine.length() > mcastBuffer.length) {
               sendLength = mcastBuffer.length;
            // Otherwise, use the line's length.
            } else {
               sendLength = nextLine.length();
            }
            // Create the datagram.
            nextLine.getBytes(0,nextLine.length(),
                              mcastBuffer,0);
            mcastPacket = new DatagramPacket(mcastBuffer,
                mcastBuffer.length,mcastIP,mcastPort);
            // Send the datagram.
            try {
               System.out.println("Sending:\t" + nextLine);
               mcastSocket.send(mcastPacket,TTL);
            } catch(IOException excpt) {
               System.err.println("Unable to send packet: "
                                     + excpt);
            }
         }
      } catch(IOException excpt) {
         System.err.println("Failed I/O: " + excpt);
      }
      mcastSocket.close(); // Close the socket.
   }
}
