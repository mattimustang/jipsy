/**
 * A test harness for the InetAddress class.
 *
 */
package test;
import java.net.*;
//import java.io.*;
public class InetAddressTest extends Object {
	public static void main(String[] args) {
		try {
			int i;

			if (args.length < 4) {
				System.out.println("Not enough arguments!");
				System.exit(1);
			}
			for (i = 0; i < 4; i++) {
				int j = 0;
				System.out.println("\nTESTING: getAllByName("+ args[i]+ ")");
				InetAddress[] inetArray = InetAddress.getAllByName(args[i]);
				System.out.println(inetArray.length + " addresses returned");
				for (; j < inetArray.length; j++) {
					System.out.println("RESULT: " + inetArray[j].toString());
				}
			}

			for (i = 0; i < 4; i++) {
				System.out.println("\nTESTING: getByName("+ args[i]+ ")");
				InetAddress inet = InetAddress.getByName(args[i]);
				System.out.println("RESULT: " + inet.toString());
			}
				

			System.out.println("\nTESTING: calling getLocalHost()");
			InetAddress localhost = InetAddress.getLocalHost();
			System.out.println("RESULT: " + localhost.toString());

			for (i = 0; i < 4; i++) {
				System.out.println("\nTESTING: getHostName()");
				InetAddress ip = InetAddress.getByName(args[i]);
				System.out.println("TESTING: result getHostName(): " + ip.getHostName());
			}

			/* test the isMulticastAddress() method */
			System.out.println("\nTESTING: IPv4 isMulticastAddress() expecting TRUE: ");
			InetAddress ip4m = InetAddress.getByName("224.0.0.1");
			System.out.println("RESULT: "
								+ (ip4m.isMulticastAddress()?"TRUE":"FALSE"));

			System.out.println("\nTESTING: IPv4 isMulticastAddress() expecting FALSE");
			InetAddress ip4m2 = InetAddress.getByName("192.168.1.1");
			System.out.println("RESULT: "
								+ (ip4m2.isMulticastAddress()?"TRUE":"FALSE"));

			System.out.println("\nTESTING: IPv6 isMulticastAddress() expecting TRUE");
			InetAddress ip6m = InetAddress.getByName("FFFF:1111::");
			System.out.println("RESULT: "
								+ (ip6m.isMulticastAddress()?"TRUE":"FALSE"));


			System.out.println("\nTESTING: IPv6 isMulticastAddress() expecting FALSE");
			InetAddress ip6m2 = InetAddress.getByName("F7::1");
			System.out.println("RESULT: "
								+ (ip6m2.isMulticastAddress()?"TRUE":"FALSE"));

			/* test the equals() method */
			System.out.println("\nTESTING: IPv4 equals() expecting TRUE: ");
			InetAddress ip4e1 = InetAddress.getByName("192.168.1.1");
			InetAddress ip4e2 = InetAddress.getByName("192.168.1.1");
			System.out.println("RESULT: "
								+ (ip4e1.equals(ip4e2) ? "TRUE":"FALSE"));

			System.out.println("\nTESTING: IPv4 equals() expecting FALSE");
			InetAddress ip4ne = InetAddress.getByName("192.168.1.2");
			System.out.println("RESULT: "
								+ (ip4e1.equals(ip4ne) ?"TRUE":"FALSE"));

			System.out.println("\nTESTING: IPv6 equals() expecting TRUE");
			InetAddress ip6e1 = InetAddress.getByName("FFFF:1111::");
			InetAddress ip6e2 = InetAddress.getByName("FFFF:1111::");
			System.out.println("RESULT: "
								+ (ip6e1.equals(ip6e2) ?"TRUE":"FALSE"));


			System.out.println("\nTESTING: IPv6 equals() expecting FALSE ");
			InetAddress ip6ne = InetAddress.getByName("F7::1");
			System.out.println("RESULT: "
								+ (ip6e1.equals(ip6ne) ?"TRUE":"FALSE"));
			System.out.println("\nTESTING: IPv6 equals() IPv4 expecting FALSE ");
			System.out.println("RESULT: "
								+ (ip6e1.equals(ip4e1) ?"TRUE":"FALSE"));

			// create a new URL object pointing at local web server
			//URL url = new URL("http://www.uts.edu.au/");
			//System.out.println("url: " + url.toString());
			//url.getContent();
		} catch (UnknownHostException uhe) {
			uhe.printStackTrace();
		}
	}
}  
