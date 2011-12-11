
int create_tcp_server(const char* hostname, const char* servname);
int lisen_connection(int socket);

	/* int getaddrinfo(const char *node, 
	 * 							const char *service,
	 * 							const struct addrinfo *hints,
	 * 							struct addrinfo **res);
	 */

	/*      struct addrinfo {
     *          int              ai_flags;		Input flags
     *          int              ai_family;		Protocol family for socket
     *          int              ai_socktype;	Socket type
     *          int              ai_protocol;	Protocol for socket
     *          size_t           ai_addrlen;	Length of socket address
     *          struct sockaddr *ai_addr;		Socket address for socket
     *          char            *ai_canonname;	Canonical name for service location
     *			struct addrinfo *ai_next;		Pointer to next in list
     *      };
	 */
