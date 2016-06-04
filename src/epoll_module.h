/*************************************************************************
	> File Name: epoll_module.h
	> Author: rebot
	> Mail: 327767852@qq.com 
	> Created Time: Fri 03 Jun 2016 01:07:28 PM CST
 ************************************************************************/
#ifndef __EPOLL_MODULE_H
#define __EPOLL_MODULE_H

#include "event_module.h"
#include <sys/epoll.h>


namespace epoll_module
{
	static const int MAX_EVENTS = 1024;

	using namespace event_module;
	class CEpoll : public boost::noncopyable
	{
		public:
			CEpoll(socket_module::CSocketServer listen) : m_listen(listen),m_epollfd(0),m_map(server_module::g_sock_map), m_queue(server_module::g_ready_queue){}
			
			bool initial_epoll()
			{
				m_epollfd = epoll_create(1024);
				if(-1 == m_epollfd)
				{
					perror("epoll_create");
					return false;
				}

				/*add listensock*/
				struct epoll_event ev;
				ev.events = EPOLLIN;
				ev.data.fd = m_listen.get_sock();
				if(epoll_ctl(m_epollfd, EPOLL_CTL_ADD, m_listen.get_sock(), &ev) == -1)
				{
					perror("add_epoll listensock");
					return false;
				}
				return true;
			}
			
			bool add_epoll(SOCKET_T sock)
			{
				struct epoll_event ev;
				ev.events  = EPOLLIN | EPOLLET;
				ev.data.fd = sock;
				
				socket_module::setnonblock_sock(sock);

				if(epoll_ctl(m_epollfd, EPOLL_CTL_ADD, sock, &ev) == -1)
				{
					perror("add_epoll connsock");
					return false;
				}
				return true;
			}

			bool delete_epoll(SOCKET_T sock)
			{
				struct epoll_event ev;
				ev.data.fd = sock;

				if(epoll_ctl(m_epollfd, EPOLL_CTL_DEL, sock, &ev) == -1)
				{
					perror("delete_epoll");
					return false;
				}
				return true;
			}
			
			bool wait_epoll()
			{
				int nfds = epoll_wait(m_epollfd, m_events, MAX_EVENTS, -1);
				
				if(nfds == -1 && errno != EINTR)
				{
					perror("wait_epoll");
					return false;	
				}
				
				for(int n = 0; n < nfds; ++n)
				{
					if(m_events[n].data.fd == m_listen.get_sock())
					{
						m_listen.accept_sock();
						SOCKET_T conn_sock = m_listen.get_conn_sock();
						add_epoll(conn_sock);
			//			m_mutex.lock();
			//			m_map.insert(std::pair<int, int>(conn_sock, 0));
			//			m_mutex.unlock();
					}
					else
					{
						SOCKET_T ready_sock = m_events[n].data.fd;
						__dispatch_event(ready_sock);
					}
				}
			}

		private:
			void __dispatch_event(SOCKET_T sock)
			{	
				m_mutex.lock();
				m_queue.push(sock);
				m_mutex.unlock();
				
				st_message msg;
				bzero(&msg, sizeof(msg));
				recv(sock, &msg, sizeof(msg), 0);
				st_event event = {msg, sock};
				event.m_msg.m_syn = 0;
				event.m_msg.m_ack = 1;
				CFactory fac(event);
				fac.generate();
				fac.handle();
			}

		private:	
			std::map<int, int>&                       m_map;
			SOCKET_T					  m_epollfd;
			socket_module::CSocketServer& m_listen;
			struct epoll_event			  m_events[MAX_EVENTS];
			std::queue<int>&              m_queue;
			mutex_module::CMutex m_mutex;
	};
}

#endif
