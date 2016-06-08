/*************************************************************************
  > File Name: test.c
  > Author: rebot
  > Mail: 327767852@qq.com 
  > Created Time: Tue 31 May 2016 10:38:43 AM CST
 ************************************************************************/


#include <iostream>
#include <string>
#include <cstdlib>
#include <string.h>
#include <memory.h>
#include "epoll_module.h"
#include "logfile_module.h"
#include "thread_module.h"

using namespace std;
using namespace logfile_module;
using namespace epoll_module;

void* f_log(void *args)
{
	CLogFile *p = (CLogFile *)args;
	p->write_log();
}


/*
void* f_thread(void *args)
{

	std::queue<int> *q = (std::queue<int> *)args;
	while(1)
	{
		while(.size() == 0)
			(p_epoll->m_cond).wait();

		std::queue<int> q = p_epoll->m_queue;
		while(q.size() != 0)
		{
			SOCKET_T sock = q.front();
			p_epoll->m_mutex.lock();
			q.pop();
			p_epoll->m_mutex.unlock();
			st_event *p_event = new st_event;
			bzero(p_event, sizeof(st_event));
			p_event->m_sock = sock;
			p_epoll->m_thread->run(p_event);
		}
	}
}
*/

int main(void)
{
	U_INT_16 port1 = 8800;
	U_INT_16 port2 = 9900;
	pthread_t system_pid;
	pthread_t event_pid;
	pthread_t thread_pid;
//	pthread_t control_pid;
	pthread_create(&system_pid, NULL, f_log, &g_system_log);
	pthread_create(&event_pid, NULL, f_log, &g_event_log);
	pthread_create(&thread_pid, NULL, f_log, &g_thread_log);
//	pthread_create(&control_pid, NULL, f_thread, &g_ready_queue);

	unsigned long ip1 = 1234567;

	struct message_header header = {std::pair<IP, PORT>(ip1, port1), 
		std::pair<IP, PORT>(ip1, port2), 
		0};

	st_message_data data;
	//	bzero(&data, sizeof(data));
	st_type tp;
	memcpy(&data, "abcdefghijklmnopqrstuvwxyz", 27);
	st_message msg = {tp, header, data};
	socket_module::CSocketServer listenfd(data.m_content);

	IP ip = inet_addr("127.0.0.1");
	listenfd.create_sock(ip , 9999, SOCK_STREAM);

	listenfd.listen_sock();

	epoll_module::CEpoll epoll_sock(listenfd);


	epoll_sock.initial_epoll();

	g_system_log.push_log("ANTP start");

	//	sleep(1);

	while(1)
	{
		epoll_sock.wait_epoll();
	}
	//	int fd = open("1.txt", O_RDWR | O_CREAT, 0666);


	/*
	 * test for inner_message::map<int,int>
	 server_module::g_sock_map.insert(std::pair<int, int>(1,2));
	 st_message_data data;
	 st_type tp;
	 memcpy(&data, "abcdefghijklmnopqrstuvwxyz", 27);
	 st_message msg = {tp, header, data};
	 event_module::SOCKET_T sock = socket(AF_INET, SOCK_STREAM, 0);
	 event_module::st_event event = {msg, sock};

	 user_module::CConcreteHandle *pHandle = new user_module::CConcreteHandle();
	 event_module::CTxtEvent txt(event);
	 txt.load_handle(pHandle);
	 txt.handle_event();
	 std::cout << pHandle->get_buf() << std::endl;
	 */

	/*
	 * test for CFinEventClient and CFinEventServer
	 server_module::g_sock_map.insert(std::pair<int, int>(1,2));
	 st_message_data data;
	 st_type tp;
	 st_message msg = {tp, header, data};
	 event_module::SOCKET_T sock = socket(AF_INET, SOCK_STREAM, 0);
	 event_module::st_event event = {msg, sock};
	 event_module::CFinEventClient fin_client(event);
	 fin_client.handle_event();

	 event_module::CFinEventServer fin_server(event);
	 fin_server.handle_event();
	 */

	/*
	 * test for CSynEvent and CAckEvent
	 st_message_data data;
	 st_type tp;
	 st_message msg = {tp, header, data};
	 event_module::st_event event = {msg, 0};
	 event_module::CSynEvent syn(event);
	 syn.handle_event();

	 event_module::CAckEvent ack(syn.get_event());
	 ack.handle_event();
	 */

	/*
	 * test for CSynEvent
	 st_message_data data;
	 st_type tp;
	 st_message msg = {tp, header, data};
	 event_module::st_event event = {msg, 0};
	 event_module::CSynEvent syn(event);
	 syn.handle_event();
	 */

	/*
	 * test for inner_message.h
	 st_message_data data;
	 st_type tp;
	 bzero(&tp, sizeof(tp));
	 tp.syn = 1;
	 tp.ack = 1;
	 st_message msg = {tp, header, data};

	 cout << msg.m_syn  << endl;	
	 cout << msg.m_ack  << endl;
	 cout << msg.m_data << endl;
	 cout << msg.m_fin  << endl;
	 */

	/*
	 * test for encrpty_module version_01
	 string str;

	 encrypt_module::construct_transport_message_header(header, "aes_encrypt_cpp", "encrypt", str);
	 cout << str << endl;

	 struct message_header tmp;

	 encrypt_module::parse_transport_message_header(tmp, "aes_encrypt_cpp", "decrypt", str);

	 cout << tmp.m_src.first << endl;
	 */


	/*
	 * test for version_00
	 string str = Serialization::Serialize(header);

	 CPython p("aes_encrypt_cpp");
	 p.initial();
	 p.exec("encrypt", str.c_str());
	 const char *estr = p.get_result();

	 p.exec("decrypt", estr);
	 const char *dstr = p.get_result();
	 cout << dstr << endl;
	 struct message_header tmp = Serialization::DeSerialize(string(dstr));

	 cout << tmp.m_src.first << endl;
	 */
}
