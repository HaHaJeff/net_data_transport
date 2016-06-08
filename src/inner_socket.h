/*************************************************************************
  > File Name: inner_socket.h
  > Author: rebot
  > Mail: 327767852@qq.com 
  > Created Time: Thu 02 Jun 2016 03:26:27 PM CST
 ************************************************************************/

#ifndef __INNER_SOCKET_H
#define __INNER_SOCKET_H
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>      
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include "inner_message.h"

namespace socket_module
{
	class CSocket
	{
		public:
			CSocket() : m_buff(""){}

			CSocket(std::string buff) : m_buff(buff){bzero(&m_saddr, sizeof(m_saddr));}

			struct sockaddr_in& get_saddr(){return m_saddr;}

			int get_sock(){return m_sock;}

		protected:
			int m_sock;
			std::string m_buff;
			struct sockaddr_in m_saddr;
	};

	class CSocketClient : public CSocket
	{
		public:
			CSocketClient(){}
			CSocketClient(std::string buf) : CSocket(buf){}

			bool create_sock(IP ip, PORT port, PROTOCOL protocol)
			{
				struct sockaddr_in saddr;
				int len = sizeof(struct sockaddr_in);

				bzero(&saddr, len);
				saddr.sin_addr.s_addr = ip;
				saddr.sin_port = htons(port);
				saddr.sin_family = AF_INET;

				memcpy(&m_saddr, &saddr, len);

				m_sock = socket(AF_INET, protocol, 0);

				if(-1 != m_sock)
					return true;
				perror("socket client");
				return false;
			}

			bool connect_sock()
			{
				if(0 == connect(get_sock(), (struct sockaddr *)&(get_saddr()), sizeof(struct sockaddr_in)))
					return  true;
				perror("connect");
				return false;
			}
	};

	class CSocketServer : public CSocket
	{
		public:
			CSocketServer(){}
			CSocketServer(std::string buf) : CSocket(buf){}

			bool listen_sock()
			{
				if(0 == listen(m_sock, 1024))
					return true;
				perror("listen");
				return false;
			}	

			int accept_sock()
			{
				bzero(&m_conn_addr, sizeof(struct sockaddr_in));
				socklen_t len;	
				m_conn_sock = accept(m_sock, (struct sockaddr *)&m_conn_addr, &len);
				if(-1 != m_conn_sock)
					return true;

				perror("accept");
				return false;
			}

			bool create_sock(IP ip, PORT port, PROTOCOL protocol)
			{
				struct sockaddr_in saddr;
				int len = sizeof(struct sockaddr_in);

				bzero(&saddr, len);
				saddr.sin_addr.s_addr = ip;
				saddr.sin_port = htons(port);
				saddr.sin_family = AF_INET;

				memcpy(&m_saddr, &saddr, len);

				m_sock = socket(AF_INET, protocol, 0);

				if(-1 != bind(m_sock, (struct sockaddr *)&saddr, len))
					return true;

				perror("bind");
				return false;
			}

			int get_conn_sock(){return m_conn_sock;}

			struct sockaddr_in get_conn_addr(){return m_conn_addr;}

		private:
			int m_conn_sock;
			struct sockaddr_in m_conn_addr;
	};

	void splice_sock(std::pair<int, int> psock)
	{
		int pipe_fd[2];
		int ret = pipe(pipe_fd);
		std::cout << "pipe ret :" << ret << std::endl;
		ret = splice(psock.first, NULL, pipe_fd[1], NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
		std::cout << "splice1 ret :" << ret << std::endl;
		ret = splice(pipe_fd[0], NULL, psock.second, NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
		std::cout << "splice2 ret :" << ret << std::endl;
	}

	void setnonblock_sock(int sock)
	{
		int old_opt = fcntl(sock, F_GETFL);
		fcntl(sock, F_SETFL, old_opt | O_NONBLOCK);
	}
}
#endif
