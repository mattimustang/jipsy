/*
 * $Id$
 *
 * PDSITestServer.java - A Test Harness for the "server" side of a UDP/IP
 *                       client/server.
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
 * Revision 1.1  1999/12/07 12:41:59  mpf
 * - Initial import.
 *
 *
 */

import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class PDSITestServer {

	private static final int ECHO_PORT = 28000;
	private DatagramSocket echoSocket = null;
	private static final int ECHO_ARRAY = 256;

	// A boolean to keep the server looping until stopped.
	private boolean keepRunning = true;

	public static void main(String[] args) {
		PDSITestServer server = new PDSITestServer();
		server.startServing();
	}

	public PDSITestServer() {
		try {
			echoSocket = new DatagramSocket(ECHO_PORT);
		} catch(SocketException excpt) {
			System.err.println("Unable to open socket: " + 
			excpt);
		}
	}

	public void startServing() {
		DatagramPacket datagram;   // For a UDP datagram.
		InetAddress clientAddr;    // Address of the client.
		int clientPort;            // Port of the client.
		byte[] dataBuffer;         // To construct a datagram.
		// Keep looping while we have a socket.
		while(keepRunning) {
			try {
				// Create a DatagramPacket to receive query.
				dataBuffer = new byte[ECHO_ARRAY];
				datagram = new DatagramPacket(dataBuffer,
				dataBuffer.length);
				echoSocket.receive(datagram);
				// Get the meta-info on the client.
				clientAddr = datagram.getAddress();
				clientPort = datagram.getPort();
				// Place the time into byte array.
				dataBuffer = getDataString(datagram.getData());
				// Create and send the datagram.
				datagram = new DatagramPacket(dataBuffer,
				dataBuffer.length,clientAddr,clientPort);
				echoSocket.send(datagram);
			} catch(IOException excpt) {
				System.err.println("Failed I/O: " + excpt);
			}
		}
		echoSocket.close();
	}

	public byte[] getDataString(byte[] data) {
		String s = new String(data);
		String ret = new String("\0");
		int i = s.indexOf(ret);
		String ns = s.substring(0, i);
		return ns.getBytes();
	}
		
}
