/*
 * $Id$
 *
 * PSITest.java - A Test program for java.net.PlainsocketImpl.
 * Copyright (C) 1999 Matthew Flanagan. All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Change Log:
 *
 * $Log$
 * Revision 1.1  1999/12/04 09:23:32  mpf
 * - Added license.
 *
 *
 */

package test;
import java.io.*;
import java.net.*;

public class PSITest {
	public static void main(String[] args) throws IOException {
		Socket s = null;
		PrintWriter out = null;
		BufferedReader in = null;
		int i;


		for (i = 0; i < 2; i++) {
			try {
				System.out.println("Connecting to: " + args[i] + " port 7");
				s = new Socket(args[i], 7);
				out = new PrintWriter(s.getOutputStream(), true);
				in = new BufferedReader(new InputStreamReader(
											s.getInputStream()));
			} catch (UnknownHostException e) {
				System.err.println("EchoClient: Couldn't get I/O for the connection to " + args[i]);
				e.printStackTrace();
				System.exit(1);
			}
	
			BufferedReader stdIn = new BufferedReader(
										new InputStreamReader(System.in));
			String userInput;
	
			try {
				while ((userInput = stdIn.readLine()) != null) {
					out.println(userInput);
					System.out.println("echo: " + in.readLine());
				}
			} catch (IOException ioe) {
				out.close();
				in.close();
				stdIn.close();
				s.close();
			}
		}
		for (i = 0; i < 2; i++) {
			try {
				System.out.println("Connecting to InetAddress(): " + args[i] + " port 7");
				s = new Socket(InetAddress.getByName(args[i]), 7);
				out = new PrintWriter(s.getOutputStream(), true);
				in = new BufferedReader(new InputStreamReader(
											s.getInputStream()));
			} catch (UnknownHostException e) {
				System.err.println("EchoClient: Couldn't get I/O for the connection to " + args[i]);
				e.printStackTrace();
				System.exit(1);
			}
	
			BufferedReader stdIn = new BufferedReader(
										new InputStreamReader(System.in));
			String userInput;
	
			try {
				while ((userInput = stdIn.readLine()) != null) {
					out.println(userInput);
					System.out.println("echo: " + in.readLine());
				}
			} catch (IOException ioe) {
				out.close();
				in.close();
				stdIn.close();
				s.close();
			}
		}

		for (i = 0; i < 2; i++) {
			try {
				System.out.println("Connecting to " + args[i] + " port 7 from "
									+ args[i+2] + " port " + args[i+4] );
				s = new Socket(args[i],7, InetAddress.getByName(args[i+2]), Integer.parseInt(args[i+4]));
				out = new PrintWriter(s.getOutputStream(), true);
				in = new BufferedReader(new InputStreamReader(
											s.getInputStream()));
			} catch (UnknownHostException e) {
				System.err.println("EchoClient: Couldn't get I/O for the connection to " + args[i]);
				e.printStackTrace();
				System.exit(1);
			}
	
			BufferedReader stdIn = new BufferedReader(
										new InputStreamReader(System.in));
			String userInput;
	
			try {
				while ((userInput = stdIn.readLine()) != null) {
					out.println(userInput);
					System.out.println("echo: " + in.readLine());
				}
			} catch (IOException ioe) {
				out.close();
				in.close();
				stdIn.close();
				s.close();
			}
		}
		for (i = 0; i < 2; i++) {
			try {
				System.out.println("Connecting to InetAddress()" + args[i] + " port 7 from "
									+ args[i+2] + " port " + args[i+4] );
				s = new Socket(InetAddress.getByName(args[i]),7, InetAddress.getByName(args[i+2]), Integer.parseInt(args[i+4])+2);
				out = new PrintWriter(s.getOutputStream(), true);
				in = new BufferedReader(new InputStreamReader(
											s.getInputStream()));
			} catch (UnknownHostException e) {
				System.err.println("EchoClient: Couldn't get I/O for the connection to " + args[i]);
				e.printStackTrace();
				System.exit(1);
			}
	
			BufferedReader stdIn = new BufferedReader(
										new InputStreamReader(System.in));
			String userInput;
	
			try {
				while ((userInput = stdIn.readLine()) != null) {
					out.println(userInput);
					System.out.println("echo: " + in.readLine());
				}
			} catch (IOException ioe) {
				out.close();
				in.close();
				stdIn.close();
				s.close();
			}
		}
		
	}
}
