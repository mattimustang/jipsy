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
	int timeout = 0;
	
	// save some headaches with the native code by keeping our
	// own copy of the file descriptor
	private int fdnum = -1;
	

	static {
		System.loadLibrary("net6");

		/* still need this for things like socketWrite() */
		System.loadLibrary("net");
	}


	protected native void create(boolean stream) throws IOException;

	protected void connect(String host, int port)
		throws UnknownHostException, IOException {

		IOException e = null;
		try {
			address = InetAddress.getByName(host);
	
			try {
				sockConnect(address, port);
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

		sockConnect(host, port);
	}

	protected native void bind(InetAddress host, int port) throws IOException;

	protected native void listen(int backlog) throws IOException;

	private native void accept(PlainSocketImpl s) throws IOException;

	protected void accept(SocketImpl s) throws IOException {
		accept((PlainSocketImpl)s);
	}

	protected InputStream getInputStream() throws IOException {
			return new SocketInputStream(this);
	}

	protected OutputStream getOutputStream() throws IOException {
			return new SocketOutputStream(this);
	}

	protected int available() throws IOException {
		return sockAvailable();
	}

	protected void close() throws IOException {
		if (fd != null) {
			sockClose();
			fd = null;
		}
			
	}

	protected void finalize() throws IOException {
		close();
	}

	public Object getOption(int opt) throws SocketException {

		return null;
	}

	public void setOption(int opt, Object val) throws SocketException {

	}
	private native void sockConnect(InetAddress host, int port) throws IOException;
	private native void sockClose() throws IOException;
	private native int sockAvailable() throws IOException;

}

