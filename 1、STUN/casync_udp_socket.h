

#ifndef _C_ASYNC_UDP_SOCKET_H_
#define _C_ASYNC_UDP_SOCKET_H_


namespace chen {
	
	
	class casync_udp_socket
	{
	public:
		casync_udp_socket();
		~casync_udp_socket();
		
	public:
		bool init(const char * ip, uint16_t port);
		bool startup();
		
		
		void stop();
		void destroy();
		
	private:
		
		
	};
}

#endif // _C_ASYNC_UDP_SOCKET_H_