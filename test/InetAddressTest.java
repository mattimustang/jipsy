package test;
import java.net.*;
//import java.io.*;
public class InetAddressTest extends Object {
	public static void main(String[] args) {
		try {
    		// create a new InetAddress object
			String addr1 = new String(args[0]);
			String addr2 = new String(args[1]);
			System.out.println("TEST: calling getByName(" + addr1 + ")");
			InetAddress iAddr = InetAddress.getByName(addr1);
			System.out.println("TEST: result getByName(" + addr1 + "): " + iAddr.toString());

			System.out.println("\nTEST: calling getLocalHost()");
			InetAddress localhost = InetAddress.getLocalHost();
			System.out.println("TEST: result getLocalHost(): " + localhost.toString());

			System.out.println("\nTEST: calling getByName(" + addr2 + ")");
			InetAddress ip = InetAddress.getByName(addr2);
			System.out.println("TEST: result getHostName(): " + ip.getHostName());

			/* test the isMulticastAddress() method */
			System.out.println("\nTEST: testing IPv4 isMulticastAddress() expecting TRUE: ");
			InetAddress ip4m = InetAddress.getByName("224.0.0.1");
			System.out.println("RESULT: "
								+ (ip4m.isMulticastAddress()?"TRUE":"FALSE"));

			System.out.println("\nTEST: testing IPv4 isMulticastAddress() expecting FALSE");
			InetAddress ip4m2 = InetAddress.getByName("192.168.1.1");
			System.out.println("RESULT: "
								+ (ip4m2.isMulticastAddress()?"TRUE":"FALSE"));

			System.out.println("\nTEST: Testing IPv6 isMulticastAddress() expecting TRUE");
			InetAddress ip6m = InetAddress.getByName("FFFF:1111::");
			System.out.println("RESULT: "
								+ (ip6m.isMulticastAddress()?"TRUE":"FALSE"));


			System.out.println("\nTEST: Testing IPv6 isMulticastAddress() expecting FALSE");
			InetAddress ip6m2 = InetAddress.getByName("F7::1");
			System.out.println("RESULT: "
								+ (ip6m2.isMulticastAddress()?"TRUE":"FALSE"));

			/* test the equals() method */
			System.out.println("\nTEST: testing IPv4 equals() expecting TRUE: ");
			InetAddress ip4e1 = InetAddress.getByName("192.168.1.1");
			InetAddress ip4e2 = InetAddress.getByName("192.168.1.1");
			System.out.println("RESULT: "
								+ (ip4e1.equals(ip4e2) ? "TRUE":"FALSE"));

			System.out.println("\nTEST: testing IPv4 equals() expecting FALSE");
			InetAddress ip4ne = InetAddress.getByName("192.168.1.2");
			System.out.println("RESULT: "
								+ (ip4e1.equals(ip4ne) ?"TRUE":"FALSE"));

			System.out.println("\nTEST: Testing IPv6 equals() expecting TRUE");
			InetAddress ip6e1 = InetAddress.getByName("FFFF:1111::");
			InetAddress ip6e2 = InetAddress.getByName("FFFF:1111::");
			System.out.println("RESULT: "
								+ (ip6e1.equals(ip6e2) ?"TRUE":"FALSE"));


			System.out.println("\nTEST: Testing IPv6 equals() expecting FALSE ");
			InetAddress ip6ne = InetAddress.getByName("F7::1");
			System.out.println("RESULT: "
								+ (ip6e1.equals(ip6ne) ?"TRUE":"FALSE"));

			// create a new URL object pointing at local web server
			//URL url = new URL("http://www.uts.edu.au/");
			//System.out.println("url: " + url.toString());
			//url.getContent();
		} catch (UnknownHostException uhe) {
			uhe.printStackTrace();
		}
	}
}  
