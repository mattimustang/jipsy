/*
 * $Id$
 *
 * PlainDatagramSocketImpl.java - An IPv6 capable Datagram socket
 *                                implementation.
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
 * Revision 1.3  1999/11/08 13:34:25  mpf
 * - Disabled debugging
 *
 * Revision 1.2  1999/11/07 05:51:46  mpf
 * - Added DEBUG variable.
 *
 * Revision 1.1  1999/11/03 22:31:29  mpf
 * Initial Import
 * - Working but not fully implemented.
 *
 *
 */

package java.net;

import java.io.FileDescriptor;
import java.io.IOException;
import java.io.InterruptedIOException;

/**
 * Datagram and multicast socket implementation base class.
 *
 * @author Matthew Flanagan
 * @version $Revision$ $Date$
 */

class PlainDatagramSocketImpl extends DatagramSocketImpl
{

    /* timeout value for receive() */
    private int timeout = 0;

	//set to true to enable debugging output
	private final static boolean DEBUG = false;
    /**
     * Load the IPv6 networking library
     */
    static {
	System.loadLibrary("net6");
    }

    protected  void create() throws SocketException {
		fd = new FileDescriptor();
		datagramSocketCreate();
    }

    protected  native void bind(int lport, InetAddress laddr) throws SocketException;

    protected native void send(DatagramPacket p) throws IOException;

    protected  native int peek(InetAddress i) throws IOException;

    protected  native void receive(DatagramPacket p) throws IOException;

    protected native void setTTL(byte ttl) throws IOException;

    protected native byte getTTL() throws IOException;

    protected native void join(InetAddress inetaddr) throws IOException;

    protected native void leave(InetAddress inetaddr) throws IOException;

    protected void close() {
	if (fd != null) {
	    datagramSocketClose();
	    fd = null;
	}
    }

    protected  void finalize() {
	close();
    }

    public native void setOption(int opt, Object val) throws SocketException;

    public native Object getOption(int opt) throws SocketException;
	
    private native void datagramSocketCreate() throws SocketException;
    private native void datagramSocketClose();
    
}

