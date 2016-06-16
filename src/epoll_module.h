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
#include <sys/types.h>
#include <sys/socket.h>
#include "logfile_module.h"
#include "thread_module.h"

using namespace thread_module;
using namespace logfile_module;
using namespace socket_module;
using namespace server_module;

namespace epoll_module
{
	static const int MAX_EVENTS = 1024;

	using namespace event_module;
	class CEpoll : public boost::noncopyable
	{
		public:
			CEpoll(socket_module::CSocketServer listen) : m_listen(listen),m_epollfd(0),m_map(server_module::g_sock_map), m_queue(server_module::g_leave_queue), m_cond(m_mutex)
		{
			m_thread = new CThreadManger(20);
		}	

			bool initial_epoll()
			{
				m_epollfd = epoll_create(1024);
				if(-1 == m_epollfd)
				{
					g_system_log.push_log("epoll create error, please exam epoll_module.h");
					return false;
				}

				/*add listensock*/
				struct epoll_event ev;
				ev.events = EPOLLIN;
				ev.data.fd = m_listen.get_sock();
			
				if(epoll_ctl(m_epollfd, EPOLL_CTL_ADD, m_listen.get_sock(), &ev) == -1)
				{
					g_system_log.push_log("add listen error, please exam epoll_module.h");
					return false;
				}

				return true;
			}
			
			bool add_epoll(SOCKET_T sock)
			{
				const int on = 1;
				struct epoll_event ev;
				ev.events  = EPOLLIN | EPOLLET;
				ev.data.fd = sock;
				
				/*1 for TCP_NODELAY*/
				setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof(on));
				setsockopt(sock, IPPROTO_TCP, 1, &on, sizeof(on));
				
				socket_module::setnonblock_sock(sock);
				
	
				if(epoll_ctl(m_epollfd, EPOLL_CTL_ADD, sock, &ev) == -1)
				{
					g_system_log.push_log("add sockfd error, please exam epoll_module.h");
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
					g_system_log.push_log("delete sockfd error, please exam epoll_module.h");
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
					}
					else
					{
						SOCKET_T ready_sock = m_events[n].data.fd;
						std::map<int, int>::iterator iter = g_sock_map.find(ready_sock);							
						printf("come sockfd:%d\n", ready_sock);
						
						std::list<int>::iterator iter_list = find(g_leave_queue.begin(), g_leave_queue.end(), ready_sock);
						if(iter_list != g_leave_queue.end())
						{
							while(iter_list != g_leave_queue.end())
							{
								iter_list = g_leave_queue.begin();
								shutdown(*iter_list, SHUT_RDWR);
								delete_epoll(*iter_list);
								g_leave_queue.pop_front();
							}
							continue;
						}
					
						if(iter != g_sock_map.end())
						{
							add_epoll((*iter).second);
							printf("insert sockfd:%d\n", (*iter).second);
						}
						__dispatch_event(ready_sock);
					}
				}
			}
		
		private:
			void __dispatch_event(SOCKET_T sock)
			{	
	//			m_mutex.lock();
	//			m_queue.push(sock);
	//			cout << "__dispatch_event queue for size:" << m_queue.size() << endl;
	//			m_mutex.unlock();
	//			m_cond.signal();
				
				st_event *p_event = new st_event;
				bzero(p_event, sizeof(st_event));
				p_event->m_sock   = sock;
				m_thread->run(p_event);
			}

		private:	
			std::map<int, int>&           m_map;
			SOCKET_T					  m_epollfd;
			socket_module::CSocketServer& m_listen;
			struct epoll_event			  m_events[MAX_EVENTS];
		
		public:
			std::list<int>&              m_queue;
			mutex_module::CMutex m_mutex;
			mutex_module::CCondMutex m_cond;
			CThreadManger*                m_thread;
	};
}
#endif
