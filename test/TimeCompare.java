package test;
import java.io.*;    // Import the package names used.
import java.net.*;
import java.util.*;
/**
 * This is an application to obtain the times from 
 * various remote systems via UDP and then report
 * a comparison.
 * @author David W. Baker
 * @version 1.1
 */
public class TimeCompare extends Thread {
   private static final int TIME_PORT = 13;  // Daytime port.
   private static final int TIMEOUT = 10000; // UDP timeout.
   DatagramSocket timeSocket = null;
   private InetAddress[] remoteMachines;
   private String arguments;
   private Date localTime;
   // This is the size of the datagram data to send
   // for the query - intentially small.
   private static final int SMALL_ARRAY = 1;
   /**
    * This method starts the application.
    * @param args Command line arguments - remote hosts.
    */
   public static void main(String[] args) {
      if (args.length < 1) {
         System.out.println(
            "Usage: TimeCompare host1 (host2 ... hostn)");
         System.exit(1);
      }
      // Create an instance.
      TimeCompare runCompare = new TimeCompare(args);
      // Start the thread running.
      runCompare.start();
   }
   /**
    * The constructor looks up the remote hosts and 
    * creates a UDP socket.
    * @param hosts The hosts to contact.
    */
   public TimeCompare(String[] hosts) {
      remoteMachines = new InetAddress[hosts.length];
      // Look up all hosts and place in InetAddress[] array.
      for(int hostsFound = 0; hostsFound < hosts.length; 
            hostsFound++) {
         try {
            remoteMachines[hostsFound] = 
               InetAddress.getByName(hosts[hostsFound]);
         } catch(UnknownHostException excpt) {
            remoteMachines[hostsFound] = null;
            System.err.println("Unknown host " + 
               hosts[hostsFound] + ": " + excpt);
         }
      }
      try {
         timeSocket = new DatagramSocket();
      } catch(SocketException excpt) {
         System.err.println("Unable to bind UDP socket: " + 
                              excpt);
         System.exit(1);
      }
   }
   /**
    * This method is the thread of execution where we 
    * send out requests for times, control the object
    * which will return the responses, and then print
    * out the report.
    */
   public void run() {
      DatagramPacket timeQuery;
      DatagramPacket[] timeResponses;
      int datagramsSent = 0;
      // Send out an empty UDP packet to each machine,
      // asking it to respond with its time.
      for(int ips = 0;ips < remoteMachines.length; ips++) {
   if (remoteMachines[ips] != null) {
     try {
       byte[] empty = new byte[SMALL_ARRAY];
       timeQuery = new DatagramPacket(empty,
          empty.length,remoteMachines[ips],TIME_PORT);
       timeSocket.send(timeQuery);
            datagramsSent++;
     } catch(IOException excpt) {
            System.err.println("Unable to send to " + 
                        remoteMachines[ips] + ": " + excpt);
     }
   }
      }
      // Create an array in which to place responses.
      timeResponses = new DatagramPacket[datagramsSent];
      // Create a listener thread.
      ReceiveDatagrams listener = 
         new ReceiveDatagrams(timeSocket,timeResponses);
      // Get current time to base the comparisons.
      localTime = new Date();
      // Calculate the timeout time and start listener.
      long endWait = System.currentTimeMillis() + TIMEOUT;
      listener.start();
      do {
         yield();
         if (System.currentTimeMillis() > endWait) {
            listener.stop();
            yield();
            break;
         }
      } while(listener.isAlive());
      printTimes(timeResponses); // Print the comparison.
      System.exit(0);            // Exit.
   }
   /**
    * This prints out a report comparing the times
    * sent from the remote hosts with the local
    * time.
    * @param times The datagram responses from the hosts.
    */
   protected void printTimes(DatagramPacket[] times) {
      Date remoteTime;
      String timeString;
      long secondsOff;
      InetAddress dgAddr;
      System.out.print("TIME COMPARISON\n\tCurrent time " +
                       "is: " + localTime + "\n\n"); 
      // Iterate through each host.
      for(int hosts = 0; 
          hosts < remoteMachines.length; hosts++) {
         if (remoteMachines[hosts] != null) {
            boolean found = false;
            int dataIndex;
            // Iterate through each datagram received.
            for(dataIndex = 0; dataIndex < times.length; 
                dataIndex++) {
               // If the datagram element isn't null:
               if (times[dataIndex] != null) {
                  dgAddr = times[dataIndex].getAddress();
                  // See if there's a match.
                  if(dgAddr.equals(remoteMachines[hosts])) {
                     found = true;
                     break;
                  }
               }
            }
            System.out.println("Host: " + 
                                 remoteMachines[hosts]);
            // If there was a match, print comparison.
            if (found) {
               timeString = 
                  new String(times[dataIndex].getData(), 0);
               int endOfLine = timeString.indexOf("\n");
               if (endOfLine != -1) {
                  timeString = 
                     timeString.substring(0,endOfLine);
               }
               remoteTime = new Date(timeString);
               secondsOff = (localTime.getTime() - 
                              remoteTime.getTime()) / 1000;
               secondsOff = Math.abs(secondsOff);
               System.out.println("Time: " + timeString);
               System.out.println("Difference: " + 
                  secondsOff + " seconds\n");
            } else {
               System.out.println("Time: NO RESPONSE FROM "
                                    + "HOST\n");
            }
         }
      }
   }
   /**
    * This method performs any necessary cleanup.
    */
   protected void finalize() {
      // If the socket is still open, close it.
      if (timeSocket != null) {
         timeSocket.close();
      }
   }
}
/**
 * This class is used to receive a number of incoming
 * datagrams.
 */
class ReceiveDatagrams extends Thread {
   private DatagramSocket receiveSocket;
   private DatagramPacket[] receivePackets;
   private DatagramPacket newPacket;
   private static final int TIME_ARRAY = 50;
   /**
    * This constructor sets the socket to listen on and
    * the array to store the datagrams in.
    * @param s The socket to use.
    * @param p The array for the storage of datagrams.
    */
   public ReceiveDatagrams(DatagramSocket s, 
                           DatagramPacket[] p) {
      receiveSocket = s;
      receivePackets = p;
   }
   
   /**
    * This is the thread of execution for this class, 
    * where is goes through the number of datagrams
    * expected and waits to receive each.
    */
   public void run() {
      for(int got = 0; got < receivePackets.length; got++) {
         byte[] emptyBuffer = new byte[TIME_ARRAY];
         try {
            // Create a new DatagramPacket.
            newPacket = new DatagramPacket(emptyBuffer,
                                       emptyBuffer.length);
            // Obtain a datagram.
            receiveSocket.receive(newPacket);
            // Now that its been received, add it to the
            // array of received datagrams.
            receivePackets[got] = newPacket;
         } catch(IOException excpt) {
            System.err.println("Failed I/O: " + excpt);
         }
         yield();
      }
      receiveSocket.close();
   }
}
