/*
 * $Id$
 *
 * InetAddress.java - An IPv6 capable version of the InetAddress class.
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
 * Revision 1.7  2000/08/28 05:45:05  mpf
 * - Modifed to load libnet6.so the "new" way.
 *
 * Revision 1.6  2000/05/13 05:55:00  mpf
 * - Changed pton() native method to stringToAddress()
 *
 * Revision 1.5  1999/11/30 14:39:49  mpf
 * - removed some old unused and commented out code.
 *
 * Revision 1.4  1999/11/07 05:51:46  mpf
 * - Added DEBUG variable.
 *
 * Revision 1.3  1999/10/27 14:15:37  mpf
 * - Changed anyLocalAddress field from private to public as it is used
 *   by Socket.java when binding.
 *
 * Revision 1.2  1999/10/26 17:14:54  mpf
 * - Fixed bug in equals() where it comparing addresses correctly.
 * - Added native call to getAnyLocalAddress() and anyLocalAddress field.
 *
 * Revision 1.1.1.1  1999/10/18 18:47:43  mpf
 * Initial Import
 *
 *
 */

package java.net;
import java.security.AccessController;
import sun.security.action.*;


/**
 * This class represents an Internet Protocol (IP) address. 
 * <p>
 * Applications should use the methods <code>getLocalHost</code>, 
 * <code>getByName</code>, or <code>getAllByName</code> to 
 * create a new <code>InetAddress</code> instance. 
 *
 * @author  Matthew Flanagan
 * @version $Revision$ $Date$
 * @see     java.net.InetAddress#getAllByName(java.lang.String)
 * @see     java.net.InetAddress#getByName(java.lang.String)
 * @see     java.net.InetAddress#getLocalHost()
 */
public final 
class InetAddress implements java.io.Serializable {
    String hostName;
    byte address[];
    int family;

	// set this to true to enable debugging output
	private final static boolean DEBUG = true;

    /*
     * Load my net6 library into runtime.
     */
    static {
        AccessController.doPrivileged(new LoadLibraryAction("net6"));
    }

    /** 
     * Constructor for the Socket.accept() method.
     */
    InetAddress() {
		if (DEBUG) System.out.println("CLASS: InetAddress.InetAddress()");
    }

    /**
     * Creates an InetAddress with the specified host name and IP address.
     * @param hostName the specified host name
     * @param address the specified IP address.  The address is expected in 
     *	      network byte order.
     * @exception UnknownHostException If the address is unknown.
     */
    InetAddress(String hostName, byte[] address) {
		this.hostName = hostName;
		this.address  = address;
		if (DEBUG) System.out.println("CLASS: InetAddress.InetAddress("+hostName+",byte[])");
    }

    /**
     * Utility routine to check if the InetAddress is a 
     * IP multicast address.
     */
    public boolean isMulticastAddress() {
		if (DEBUG) System.out.println("CLASS: InetAddress.isMulticastAddress()");
		int len = address.length;

		/* IPv4 address check  for class D address */
		if (len == 4)
			return ((address[0] & 0xF0) == 0xE0);
		
		/* IPv6 address check */
		if (len == 16)
			return (address[0] == (byte) 0xFF);

		/* address is not a Multicast Address */
		return false;
    }


    /**
     * Returns the fully qualified host name for this address.
     * If the host is equal to null, then this address refers to any
     * of the local machine's available network addresses.
     *
     * @return  the fully qualified host name for this address.
     */
    public String getHostName() {
		if (DEBUG) System.out.println("CLASS: InetAddress.getHostName()");
		if (hostName == null) {
			try {
				hostName = getHostByAddress(address);
			} catch (UnknownHostException he) {
				hostName = getHostAddress();
			}
		}
		return hostName;
    }

    /**
     * Returns the raw IP address of this <code>InetAddress</code> 
     * object. The result is in network byte order: the highest order 
     * byte of the address is in <code>getAddress()[0]</code>. 
     *
     * @return  the raw IP address of this object.
     */
    public byte[] getAddress() {	
		if (DEBUG) System.out.println("CLASS: InetAddress.getAddress()");
		return address;
    }

    /**
     * Returns the IPv4 address string "%d.%d.%d.%d"
	 * OR the IPv6 colon separated format "%x:%x:..."
     * @return raw IP address in a string format
     */
    public String getHostAddress() {	
		int len = address.length;

		/* allocate room for result */
		/* defined in <netinet/in.h> INET6_ADDRSTRLEN = 46 */
		StringBuffer addrBuffer = new StringBuffer(46);

		if (DEBUG) System.out.println("CLASS: InetAddress.getHostAddress()");

		/* We have an IPv6 address.
		 * Return full address for now and don't worry about
		 * special cases like:
 		 * - IPv4-mapped IPv6 addresses
		 * - IPv4 compatible addresses
		 * - :: substitution for zeros
		 */
		if (len == 16) {
			for (int i = 0; i < len; i += 2) {
				int x = ((address[i] & 0xFF) << 8) | (address[i+1] & 0xFF);
				addrBuffer.append(Integer.toHexString(x).toUpperCase());
				if (i < 14)
					addrBuffer.append(":");
			}
		}

		/* we have an IPv4 address */
		if (len == 4) {
			for (int i = 0; i < len; i++) {
				addrBuffer.append(address[i] & 0xFF);
				if (i < 3)
					addrBuffer.append(".");
			}
		}
		return addrBuffer.toString();
     }
 

    /**
     * Returns a hashcode for this IP address.
	 * (a 32 bit integer)
     *
     * @return  a hash code value for this IP address. 
     */
    public int hashCode() {
		if (DEBUG) System.out.println("CLASS: InetAddress.hashCode()");
		int hash = 0;
		int i = 0;
		int len = address.length;

		if (len == 16) {
			/* only want the last 4 bytes of IPv6 address */
			i = len - 4;	
		}
		for ( ; i < len; i++)
			hash = (hash << 8) | (address[i] & 0xFF);

		return hash;
    }

    /**
     * Compares this object against the specified object.
     * The result is <code>true</code> if and only if the argument is 
     * not <code>null</code> and it represents the same IP address as 
     * this object. 
     * <p>
     * Two instances of <code>InetAddress</code> represent the same IP 
     * address if the length of the byte arrays returned by 
     * <code>getAddress</code> is the same for both, and each of the 
     * array components is the same for the byte arrays. 
     *
     * @param   obj   the object to compare against.
     * @return  <code>true</code> if the objects are the same;
     *          <code>false</code> otherwise.
     * @see     java.net.InetAddress#getAddress()
     */
    public boolean equals(Object obj) {
		if (DEBUG) System.out.println("CLASS: InetAddress.equals()");
		if ( (obj != null) && (obj instanceof InetAddress) )
		{
			if (DEBUG) System.out.println("CLASS: InetAddress.equals(): obj.getAddress()");
			byte addr[] = ((InetAddress)obj).getAddress();
			if ( addr.length == this.address.length ) {
			if (DEBUG) System.out.println("CLASS: InetAddress.equals: length equal");
				for (int i = 0; i < this.address.length; i++) {
					if (this.address[i] != addr[i]) {
			if (DEBUG) System.out.println("CLASS: InetAddress.equals: returning false");
						return false;
					}
				}
			if (DEBUG) System.out.println("CLASS: InetAddress.equals: returning true");
			return true;
			}
    	}
			if (DEBUG) System.out.println("CLASS: InetAddress.equals: returning false");
		return false;
	}

    /**
     * Converts this IP address to a <code>String</code>.
     *
     * @return  a string representation of this IP address.
     */
    public String toString() {
		if (DEBUG) System.out.println("CLASS: InetAddress.toString()");
		return getHostName() + "/" + getHostAddress();
    }

    private static InetAddress		localHost = null;
    private static InetAddress		loopbackHost;
    static InetAddress		anyLocalAddress;

	static {
		loopbackHost = new InetAddress("localhost", getLoopbackAddress());
		anyLocalAddress = new InetAddress(null, getAnyLocalAddress());
	}
    /**
     * Determines the IP address of a host, given the host's name. The 
     * host name can either be a machine name, such as 
     * "<code>www.gnu.org</code>", or a string representing its IP 
     * address, such as "<code>192.168.1.1</code>". 
     *
     * @param      host   the specified host, or <code>null</code> for the
     *                    local host.
     * @return     an IP address for the given host name.
     * @exception  UnknownHostException  if no IP address for the
     *               <code>host</code> could be found.
     */
    public static InetAddress getByName(String host)
	throws UnknownHostException {
		if (DEBUG) System.out.println("CLASS: InetAddress.getByName("+host+")");
		if ( (host == null) || (host.length() == 0) ) {
			return loopbackHost;
		} else {
			byte[] addr = stringToAddress(host);

			if (addr != null) {
				return new InetAddress(null, addr);
			} else {
				return getAllByName(host)[0];
			}
			

		}
    }


    /** 
     * Determines all the IP addresses of a host, given the host's name. 
     * The host name can either be a machine name, such as 
     * "<code>www.gnu.org</code>", or a string representing 
     * its IP address, such as "<code>192.168.1.1</code>". 
     *
     * @param      host   the name of the host.
     * @return     an array of all the IP addresses for a given host name.
     * @exception  UnknownHostException  if no IP address for the
     *               <code>host</code> could be found.
     */
    public static InetAddress getAllByName(String host)[]
	throws UnknownHostException {

		if (DEBUG) System.out.println("CLASS: InetAddress.getAllByName("+host+")[]");
		if (host == null || host.length() == 0) {
	    	throw new UnknownHostException(host == null ? "Null string" : host);
		}

		/* check if host is IP address */
		byte[] addr = stringToAddress(host);
		
		if (addr != null) {
			InetAddress[] address = new InetAddress[1];
			address[0] = getByName(host);
			return address;
		} else {
			byte[][] addresses = getAllHostAddresses(host);
			int len = addresses.length;
			int i = 0;

			InetAddress iAddresses[] = new InetAddress[len];
			for ( ; i < len; i++) {
				iAddresses[i] = new InetAddress(host, addresses[i]);
			}
			return iAddresses;
		}
    }

    /**
     * Returns the local host.
     *
     * @return     the IP address of the local host.
     * @exception  UnknownHostException  if no IP address for the
     *               <code>host</code> could be found.
     */
    public static InetAddress getLocalHost() throws UnknownHostException {
		if (DEBUG) System.out.println("CLASS: InetAddress.getLocalHost()");

		SecurityManager s = System.getSecurityManager();
		
		/* need to check if they are allowed to to do this */
		if (localHost == null) {

			String host = getLocalHostName();
			if (s != null) {
				try {
					s.checkConnect(host, -1);
				} catch (SecurityException se) {
					localHost = loopbackHost;
					return localHost;
				}
			}
			localHost = getByName(host);
		}
		return localHost;
	}

	/**
	 * Checks if the host String is an IPv4 or IPv6 address
	 * and returns it as a byte array otherwise it returns null
	 * indicating that it was an ordinary host name.
	 * @param host the host name to check
	 * @returns a byte array of the address or null
	 */
	private static native byte[] stringToAddress(String host);
	private static native byte[] getLoopbackAddress();
	private static native byte[] getAnyLocalAddress();
    private static native String getLocalHostName();
	private static native byte[][] getAllHostAddresses(String host) throws UnknownHostException;
	private static native String getHostByAddress(byte[] addr) throws UnknownHostException;
}

