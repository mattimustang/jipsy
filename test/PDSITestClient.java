/*
 * $Id$
 *
 * PDSITest.java - A Test program for java.net.PlainDatagramSocketImpl.
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
 * Revision 1.1  1999/12/07 08:19:52  mpf
 * - Initial import
 *
 *
 */

//package test;
import java.io.*;
import java.net.*;

public class PDSITest {
	public static void main(String[] args) throws IOException {
		String remoteAddress = null;
		String localAddress = null;
		int remotePort = 0;
		int localPort = 0;

		if (args.length < 4) {
			System.out.println("Usage: PSITest <remote address> <remote port> <local address> <local port>");
			System.exit(1);
		} else {
			remoteAddress = new String(args[0]);
			remotePort = Integer.parseInt(args[1]);
			localAddress = new String(args[2]);
			localPort = Integer.parseInt(args[3]);
		}

		try {
			System.out.print("TEST: Connecting to: " + remoteAddress + " port " + remotePort+": ");
			DatagramSocket ds = new DatagramSocket();
			SendRecvDatagrams(ds, remoteAddress, remotePort);
			ds.close();	
		} catch (SocketException e) {
			e.printStackTrace();
			System.exit(1);
		} catch (IOException ioe) {
			ioe.printStackTrace();
			System.exit(1);
		}

		try {
			System.out.print("\nTEST: Connecting to: " + remoteAddress + " port " + remotePort
								+ " from port " + localPort+": ");
			DatagramSocket ds = new DatagramSocket(localPort);
			SendRecvDatagrams(ds, remoteAddress, remotePort);
			ds.close();
				
		} catch (SocketException e) {
			e.printStackTrace();
			System.exit(1);
		} catch (IOException ioe) {
			ioe.printStackTrace();
			System.exit(1);
		}


		try {
			System.out.print("\nTEST: Connecting to: " + remoteAddress + " port " + remotePort
				+ " from " + localAddress + " port " + (localPort+5) +": ");
			DatagramSocket ds = new DatagramSocket(localPort+5, InetAddress.getByName(localAddress));
			SendRecvDatagrams(ds, remoteAddress, remotePort);
			ds.close();
				
		} catch (SocketException e) {
			e.printStackTrace();
			System.exit(1);
		} catch (IOException ioe) {
			ioe.printStackTrace();
			System.exit(1);
		}
	}

	public static void SendRecvDatagrams(DatagramSocket s, String h, int p) {
		try {
			InetAddress addr = InetAddress.getByName(h);

			BufferedReader stdin = new BufferedReader(new InputStreamReader(System.in));
			String input = new String("abcdefghijklmnopqrstuvwxyz");
			DatagramPacket dpSent = new DatagramPacket(input.getBytes(), input.length(), addr, p);
			s.send(dpSent);

			byte[] buffer = new byte[input.length()];
			DatagramPacket dpRcvd = new DatagramPacket(buffer, buffer.length);
			s.receive(dpRcvd);
			String rcvd = new String(dpRcvd.getData());
			if (input.equals(rcvd)) {
				System.out.println("PASSED");
			} else {
				System.out.println("FAILED");
			}
		} catch (UnknownHostException uhe) {
			uhe.printStackTrace();
		} catch (IOException ioe) {
			ioe.printStackTrace();
		}
	}
				
}
