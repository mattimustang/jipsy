/*
 * $Id$
 *
 * PSITestClient.java - A Test Harness for "client" side of TCP client/server.
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
 * Revision 1.3  1999/12/07 12:12:50  mpf
 * - more cleaning up
 *
 * Revision 1.2  1999/12/07 08:18:57  mpf
 * - Moved common code to SendRecvData() method.
 * - Made test non-interactive.
 * - Cleaned up command line argument processing.
 *
 * Revision 1.1  1999/12/04 09:23:32  mpf
 * - Added license.
 *
 *
 */

//package test;
import java.io.*;
import java.net.*;

public class PSITestClient {
	public static void main(String[] args) throws IOException {
		Socket s = null;
		String remoteAddress = null;
		String localAddress = null;
		int localPort = 0;
		int remotePort = 0;

		if (args.length < 4) {
			System.out.println("Usage: PSITestClient <remote address> <remote port> <local address> <local port>");
			System.exit(1);
		} else {
			remoteAddress = new String(args[0]);
			remotePort = Integer.parseInt(args[1]);
			localAddress = new String(args[2]);
			localPort = Integer.parseInt(args[3]);
		}

		try {
			System.out.print("\nTEST: Connecting to: " + remoteAddress + " port "+ remotePort+": ");
			s = new Socket(remoteAddress, remotePort);
			SendRecvData(s);
			s.close();
		} catch (IOException e) {
			e.printStackTrace();
			System.exit(1);
		}
	
		try {
			System.out.print("TEST: Connecting to InetAddress(): " + remoteAddress + " port "+remotePort+": ");
			s = new Socket(InetAddress.getByName(remoteAddress), remotePort);
			SendRecvData(s);
			s.close();
		} catch (UnknownHostException e) {
			e.printStackTrace();
			System.exit(1);
		} catch (IOException ioe) {
			ioe.printStackTrace();
			System.exit(1);
		}

		try {
			System.out.print("TEST: Connecting to " + remoteAddress + " port "+remotePort+" from "
								+ localAddress + " port " + localPort +": ");
			s = new Socket(remoteAddress,remotePort, InetAddress.getByName(localAddress), localPort);
			SendRecvData(s);
			s.close();
		} catch (UnknownHostException e) {
			e.printStackTrace();
			System.exit(1);
		} catch (IOException ioe) {
			ioe.printStackTrace();
			System.exit(1);
		}
		
	
		try {
			System.out.print("TEST: Connecting to InetAddress(): " + remoteAddress + " port "+remotePort+" from "
								+ localAddress + " port " + (localPort+5)+": ");
			s = new Socket(InetAddress.getByName(remoteAddress),remotePort, InetAddress.getByName(localAddress),localPort+5);
			SendRecvData(s);
			s.close();
		} catch (UnknownHostException e) {
			e.printStackTrace();
			System.exit(1);
		} catch (IOException ioe) {
			ioe.printStackTrace();
			System.exit(1);
		}
	}

	public static void SendRecvData(Socket s) {
		
		try {
			PrintWriter out = new PrintWriter(s.getOutputStream(), true);
			BufferedReader in = new BufferedReader(new InputStreamReader(
												s.getInputStream()));
			String input = new String("abcdefghijklmnopqrstuvwxyz");

			out.println(input);
			String rcvd = in.readLine();
			if (input.equals(rcvd)) {
				System.out.println("PASSED");
			} else {
				System.out.println("FAILED");
			}
		} catch (IOException ioe) {
			ioe.printStackTrace();
		}

	}
}
