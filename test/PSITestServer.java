/*
 * $Id$
 *
 * PSITest2.java - A Test Harness for the "server" side of a TCP client/server.
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
 * Revision 1.1  1999/12/07 08:19:23  mpf
 * - Initial import.
 *
 *
 */

import java.net.*;
import java.io.*;

public class PSITest2 {
    public static void main(String[] args) throws IOException {

		int port = 2007;
		int backlog = 5;
		InetAddress localAddress = null;
		boolean doLocalBind = false;

		if (args.length != 0) {
			localAddress = InetAddress.getByName(args[0]);
			port = Integer.parseInt(args[1]);
			backlog = Integer.parseInt(args[2]);
			doLocalBind = true;
		}
        ServerSocket serverSocket = null;
        try {
			if (doLocalBind) {
            	serverSocket = new ServerSocket(port,backlog, localAddress);
			} else {
            	serverSocket = new ServerSocket(port);
			}
        } catch (IOException e) {
			e.printStackTrace();
            System.exit(1);
        }

		while (true) {
			HandleClient(serverSocket);
		}

    }

	public static void HandleClient(ServerSocket s) {
		Socket clientSocket = null;
		PrintWriter out = null;
		BufferedReader in = null;
        try {
            clientSocket = s.accept();
        } catch (IOException e) {
            System.err.println("Accept failed.");
			e.printStackTrace();
            System.exit(1);
        }

		try {
        	out = new PrintWriter(clientSocket.getOutputStream(), true);
        	in = new BufferedReader(
				new InputStreamReader(
				clientSocket.getInputStream()));
        	String inputLine;

        	while ((inputLine = in.readLine()) != null) {
           	  out.println(inputLine);
        	}
        	in.close();
        	out.close();
        	clientSocket.close();
		} catch (IOException ioe) {
			ioe.printStackTrace();
			System.exit(1);
		}
	}
		
}
