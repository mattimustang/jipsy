struct sockaddr_storage {
	u_char ss_len;
	u_char ss_family;
	u_char padding[128 - 2];
};
