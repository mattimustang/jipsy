/* a simple TCP client that connects to the echo port */
package test;
import java.io.*;
import java.net.*;

public class EchoClient {
	public static void main(String[] args) throws IOException {
		Socket echoSocket = null;
		PrintWriter out = null;
		BufferedReader in = null;
		String host;

		if (args.length != 0)
			host = new String(args[0]);
		else
			host = new String("arak");

		try {
			echoSocket = new Socket(host, 7);
			out = new PrintWriter(echoSocket.getOutputStream(), true);
			in = new BufferedReader(new InputStreamReader(
										echoSocket.getInputStream()));
		} catch (UnknownHostException e) {
			System.err.println("EchoClient: Couldn't get I/O for the connection to " + host);
			e.printStackTrace();
			System.exit(1);
		}

		BufferedReader stdIn = new BufferedReader(
									new InputStreamReader(System.in));
		String userInput;

		while ((userInput = stdIn.readLine()) != null) {
			out.println(userInput);
			System.out.println("echo: " + in.readLine());
		}
		out.close();
		in.close();
		stdIn.close();
		echoSocket.close();
	}
}
