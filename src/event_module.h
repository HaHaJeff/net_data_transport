/*************************************************************************
  > File Name: event_module.h
  > Author: rebot
  > Mail: 327767852@qq.com 
  > Created Time: Wed 01 Jun 2016 03:06:08 PM CST
 ************************************************************************/

#ifndef __EVENT_MODULE_H
#define __EVENT_MODULE_H
#include "mutex_module.h"
#include "concrete_handle.h"
#include "inner_socket.h"
#include "encrypt_module.h"
#include "logfile_module.h"
#include <stdio.h>

using namespace logfile_module;

CLogFile& event_log = g_event_log;

namespace event_module
{

	mutex_module::CMutex g_mutex;

	std::string get_str_ip(IP ip)
	{
		struct in_addr in_ip;
		in_ip.s_addr = ip;
		std::string str_ip = inet_ntoa(in_ip);
		return str_ip;
	}

	typedef int SOCKET_T;


	const std::string STR_OK	= "OK FOR CONNECT";
	const std::string STR_ERROR = "ERROR FOR CONNECT";

	/*abstract event*/
	class CAbstractEvent
	{
		public:
			virtual void handle_event() = 0;
		protected:
			st_event m_event;
		public:
			virtual st_event& get_event(){return m_event;}
	};

	/*concrete event for SYN*/
	class CSynEvent : public CAbstractEvent
	{
		public:
			CSynEvent()
			{
				bzero(&m_event, sizeof(st_event));
			}

			CSynEvent(st_event &event)
			{
				CSynEvent();
				bzero(&m_event, sizeof(st_event));
				memcpy(&m_event, &event, sizeof(st_event));
			}

			/*the handle for SYN is encrypt message_header, the encrypt text as message_data to NATServer*/
			virtual void handle_event()
			{
				st_message &msg = m_event.m_msg;
				SOCKET_T sock = m_event.m_sock;

				/*request is ack*/
				msg.m_syn = 0;
				msg.m_ack = 1;
				std::string trans_msg;
				encrypt_module::construct_transport_message_header(msg.m_header, encrypt_module::STR_PYTHON,
						encrypt_module::STR_ENCRYPT, trans_msg);
				memcpy(&msg.m_header, "0xccefccefccefccefccefccefccefccefccefccefccefccefccefccefccef", sizeof(st_message_header));
				memcpy(&msg.m_data, trans_msg.c_str(), strlen(trans_msg.c_str()));

				int ret = send(sock, &m_event, sizeof(st_event), 0);

				printf("syn send ret:%d\n", ret);
			}
	};

	/*concrete event for ACK*/
	class CAckEvent : public CAbstractEvent
	{
		public:
			CAckEvent() : m_sock_map(server_module::g_sock_map)
		{
			bzero(&m_event, sizeof(st_event));
		}

			CAckEvent(st_event &event) : m_sock_map(server_module::g_sock_map)
		{
			CAckEvent();
			memcpy(&m_event, &event, sizeof(st_event));
		}

			virtual void handle_event()
			{
				SOCKET_T sock = m_event.m_sock;

				int ret = recv(sock, &m_event, sizeof(st_event), 0);

				printf("ack send ret:%d\n", ret);
				/*
				   if(ret == -1)
				   {

				   if(errno == EAGAIN)
				   {	
				   cout << "EAGIN" << endl;
				   return;
				   }	
				   if(errno == EINTR)
				   {	
				   cout << "EINTR" << endl;
				   return;
				   }	

				   }
				*/

				st_message_header &msg_header = m_event.m_msg.m_header;
				std::string trans_msg = m_event.m_msg.m_data.m_content;				
				printf("trans msg:%s\n", trans_msg.c_str());

				encrypt_module::parse_transport_message_header(msg_header, encrypt_module::STR_PYTHON,encrypt_module::STR_DECRYPT, trans_msg);					

				std::string str_ip = get_str_ip(msg_header.m_src.first);

				if(__is_ok_for_connect(sock, str_ip))
				{
					/*construct peer sock and insert into map*/
					socket_module::CSocketClient sock_out;
					sock_out.create_sock(msg_header.m_dst.first, msg_header.m_dst.second, msg_header.m_protocol);
					sock_out.connect_sock();

					printf("in ack moudle peer sock_out:%d\n", sock_out.get_sock());

					g_mutex.lock();
					m_sock_map.insert(std::pair<int, int>(sock, sock_out.get_sock()));
					m_sock_map.insert(std::pair<int, int>(sock_out.get_sock(), sock));
					g_mutex.unlock();

					send(sock, STR_OK.c_str(), STR_OK.size(), 0);
					printf("%s\n", STR_OK.c_str());
				}
				else
				{
					send(sock, STR_ERROR.c_str(), STR_ERROR.size(), 0);
				}

			}

		private:
			bool __is_ok_for_connect(SOCKET_T sock, const std::string str_ip)
			{
				struct sockaddr_in conn;
				
				socklen_t len = sizeof(struct sockaddr);
				
				bzero(&conn, len);
				
				getpeername(sock, (struct sockaddr *)&conn, &len);

				if(server_module::g_allow_map.find(str_ip) != server_module::g_allow_map.end())
				{
					cout << "TRUE" << endl;
					return true;
				}
				else
					return false;

//				return !strcmp(str_ip.c_str(), inet_ntoa(conn.sin_addr));
			}

			std::map<int, int>& m_sock_map;
	};

	/*concrete event for DATA*/
	class CTxtEvent : public CAbstractEvent
	{
		public:
			CTxtEvent()
			{
				//	m_user_handle = NULL;
				bzero(&m_event, sizeof(st_event));
			}

			CTxtEvent(st_event &event)
			{
				CTxtEvent();
				memcpy(&m_event, &event, sizeof(st_event));
			}

			virtual void handle_event()
			{
				//	if(m_user_handle == NULL)
				//		return;
				__set_buf();
				m_event.m_msg.m_txt = 0;
				m_event.m_msg.m_trans = 1;
				int ret = send(m_event.m_sock, &m_event, sizeof(st_event), 0);
				printf("txt send ret:%d\n", ret);
			}

			//	void load_handle(user_module::CConcreteHandle *handle)
			//	{
			//		m_user_handle = handle;
			//	}

			~CTxtEvent()
			{
				//	if(m_user_handle != NULL)
				//		delete m_user_handle;
				//	m_user_handle = NULL;
			}

			CTxtEvent(const CTxtEvent &event)=delete;
			CTxtEvent operator=(const CTxtEvent event)=delete;

		private:
			//	user_module::CConcreteHandle *m_user_handle;	
			void __set_buf()
			{
				std::string str = m_event.m_msg.m_data.m_content;
			}
	};

	/*concrete event for FIN for client*/
	class CFinEventClient : public CAbstractEvent
	{
		public:
			CFinEventClient()
			{
				bzero(&m_event, sizeof(st_event));
			}

			CFinEventClient(st_event &event)
			{
				CFinEventClient();
				memcpy(&m_event, &event, sizeof(st_event));
			}

			virtual void handle_event()
			{
				SOCKET_T sock = m_event.m_sock;
				m_event.m_msg.m_finc = 0;
				m_event.m_msg.m_fins = 1;
loop:
				int ret = send(sock, &m_event, sizeof(st_event), 0);

				perror("finclient send\n");
				if(ret == -1 && errno == EAGAIN)
				{
					printf("fin client errno is EAGAIN\n");
					goto loop;
				}

				printf("finclient send ret:%d\n", ret);
				close(sock);
			}
		private:
	};

	/*concrete event for FIN for server*/
	class CFinEventServer : public CAbstractEvent
	{
		public:
			CFinEventServer() : m_sock_map(server_module::g_sock_map)
		{
			bzero(&m_event, sizeof(st_event));
		}

			CFinEventServer(st_event &event) : m_sock_map(server_module::g_sock_map)
		{
			CFinEventServer();
			memcpy(&m_event, &event, sizeof(st_event));
		}

			virtual void handle_event()
			{
				SOCKET_T sock = m_event.m_sock;
				recv(sock, &m_event, sizeof(st_event), 0);

				g_mutex.lock();

				std::map<int, int>::iterator iter = __find_sock_map();

				if(iter != m_sock_map.end())
				{	
					server_module::g_leave_queue.push_back(iter->first);
					server_module::g_leave_queue.push_back(iter->second);
				}

				std::map<int, int>::iterator iter_out = m_sock_map.find((*iter).second);
				if(iter_out != iter && iter_out != m_sock_map.end())
				{
					m_sock_map.erase(iter_out);
				}

				iter = __find_sock_map();
				if(iter != m_sock_map.end())
				{	
					m_sock_map.erase(iter);
				}

				g_mutex.unlock();
			}

		private:	
			std::map<int, int> &m_sock_map;

		private:
			std::map<int, int>::iterator __find_sock_map()
			{
				std::map<int, int>::iterator iter = m_sock_map.find(m_event.m_sock);
				return iter;
			}
	};

	class CTransEvent : public CAbstractEvent
	{
		public:
			CTransEvent() : m_sock_map(server_module::g_sock_map)
		{
			bzero(&m_event, sizeof(st_event));
		}

			CTransEvent(st_event &event) : m_sock_map(server_module::g_sock_map)
		{
			CTransEvent();
			memcpy(&m_event, &event, sizeof(st_event));
		}
		public:
			virtual void handle_event()
			{	
				m_mutex.lock();

				std::map<int, int>::iterator iter = __find_sock_map();

				if(iter != m_sock_map.end())
				{
					socket_module::splice_sock(*iter);
				}

				m_mutex.unlock();
			}

		private:
			mutex_module::CMutex m_mutex;
		private:	
			std::map<int, int> &m_sock_map;

		private:
			std::map<int, int>::iterator __find_sock_map()
			{
				std::map<int, int>::iterator iter = m_sock_map.find(m_event.m_sock);
				if(iter != m_sock_map.end())
					printf("in trans sock find to splice:%d\n", (*iter).second);
				return iter;
			}
	};
	/*strategy module */
	class CHandleEvent
	{
		public:
			//		CHandleEvent() : m_abs_event(CFinEvent()){}
			CHandleEvent(CAbstractEvent *abs_event) : m_abs_event(abs_event){}

			/*invoke m_abs_event handle_event*/
			void handle_event()
			{
				m_abs_event->handle_event();
			}

			~CHandleEvent()
			{
				if(m_abs_event != NULL)
				{
					delete m_abs_event;
					m_abs_event = NULL;
				}
			}
		private:
			CAbstractEvent *m_abs_event;
	};

	class CFactory
	{
		public:
			CFactory(st_event event) : m_handle(new CHandleEvent(new CSynEvent(event)))
		{
			//	recv(event.m_sock, &event.m_msg, sizeof(st_message), 0);
			memcpy(&m_event, &event, sizeof(st_event));
		}

			void generate()
			{
				unsigned int type = m_event.m_msg.m_syn * 1+ m_event.m_msg.m_ack * 2 + m_event.m_msg.m_txt * 4 + m_event.m_msg.m_finc * 8 + m_event.m_msg.m_fins * 16 + m_event.m_msg.m_trans * 32;
				printf("factory event type:%d\n", type);
				switch(type)
				{
					case 1:
						m_handle = new CHandleEvent(new CSynEvent(m_event));
						break;
					case 2:
						m_handle = new CHandleEvent(new CAckEvent(m_event));
						break;
					case 4:
						m_handle = new CHandleEvent(new CTxtEvent(m_event));
						break;
					case 8:
						m_handle = new CHandleEvent(new CFinEventClient(m_event));
						break;
					case 16:
						m_handle = new CHandleEvent(new CFinEventServer(m_event));
						break;
					case 32:
						m_handle = new CHandleEvent(new CTransEvent(m_event));
						break;
					default:
						m_handle = NULL;
						break;
				}	
			}

			void handle()
			{
				logfile();
				if(m_handle == NULL)
					return;
				m_handle->handle_event();
			}

			~CFactory()
			{
				if(m_handle != NULL)
				{
					delete m_handle;
					m_handle = NULL;
				}
			}

			void logfile()
			{
				IP ip = m_event.m_msg.m_header.m_src.first;
				std::string str_log = get_str_ip(ip);

				unsigned int type = m_event.m_msg.m_syn * 1+ m_event.m_msg.m_ack * 2 + m_event.m_msg.m_txt * 4 + m_event.m_msg.m_finc * 8 + m_event.m_msg.m_fins * 16 + m_event.m_msg.m_trans * 32;
				switch(type)
				{
					case 1:
						str_log += "\tstart to SYN event";
						break;
					case 2:
						str_log += "\tstart to ACK event";
						break;
					case 4:
						str_log += "\tstart to TXT event";
						break;
					case 8:
						str_log += "\tstart to FINCLIENT event";
						break;
					case 16:
						str_log += "\tstart to FINSERVER event";
						break;
					case 32:
						str_log += "\tstart to TRANS event";
						break;
					default:
						str_log += "\tstart to non event";
						break;
				}
				event_log.push_log(str_log);
			}
		private:
			st_event m_event;
			CHandleEvent* m_handle;
		protected:
	};
}

#endif
