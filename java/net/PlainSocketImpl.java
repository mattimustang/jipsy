/*
 * $Id$
 *
 * PlainSocketImpl.java - An IPv6 capable version of the PlainSocketImpl class.
 * Copyright (C) 1999 Matthew Flanagan.
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
 * Revision 1.5  1999/11/07 05:51:46  mpf
 * - Added DEBUG variable.
 *
 * Revision 1.4  1999/11/03 22:28:50  mpf
 * - Changed getOption() and setOption to completely native methods.
 *
 * Revision 1.3  1999/11/01 17:08:47  mpf
 * - Renamed sockXXXX() native functions to socketXXXX().
 *
 * Revision 1.2  1999/10/27 14:39:13  mpf
 * - Removed private copy of socket file descriptor fdnum.
 * - Changed available() to be a native function
 * - Change create() to java method that now creates a new File Descriptor
 *   and calls the native function sockCreate().
 *
 * Revision 1.1  1999/10/20 23:08:58  mpf
 * Initital Import.
 *
 *
 */

package java.net;
import java.io.*;

/**
 * The "Default" socket implementation.
 * Does not yet implement SOCKS support.
 *
 * @author Matthew Flanagan
 * @version $Revision$
 */

class PlainSocketImpl extends SocketImpl {

	// timeout option get/set by getOption/setOption
	// value is in milliseconds
	int timeout = 0;

	// set this to true to enable debugging output
	private final static boolean DEBUG = true;
	
	static {
		System.loadLibrary("net6");
	}


	protected void create(boolean stream) throws IOException
	{
		fd = new FileDescriptor();
		socketCreate(stream);
	}

	protected void connect(String host, int port)
		throws UnknownHostException, IOException {

		IOException e = null;
		try {
			address = InetAddress.getByName(host);
	
			try {
				socketConnect(address, port);
				return;
			} catch (IOException ioe) {
				e = ioe;
			}
		} catch (UnknownHostException uhe) {
			e = uhe;
		}
		close();
		throw e;
				
				
	}

	protected void connect(InetAddress host, int port) throws IOException {

		this.address = host;
		this.port = port;
		socketConnect(host, port);
	}



	protected InputStream getInputStream() throws IOException {
			return new SocketInputStream(this);
	}

	protected OutputStream getOutputStream() throws IOException {
			return new SocketOutputStream(this);
	}


	protected void close() throws IOException {
		if (fd != null) {
			socketClose();
			fd = null;
		}
			
	}

	protected void finalize() throws IOException {
		close();
	}

	public native void setOption(int opt, Object val) throws SocketException;
	public native Object getOption(int opt) throws SocketException;

	/* Native method declarations */
	protected native void bind(InetAddress host, int port) throws IOException;
	protected native void listen(int backlog) throws IOException;
	protected native void accept(SocketImpl s) throws IOException;
	protected native int available() throws IOException;
	protected native void socketCreate(boolean stream) throws IOException;
	private native void socketConnect(InetAddress host, int port) throws IOException;
	private native void socketClose() throws IOException;

}

