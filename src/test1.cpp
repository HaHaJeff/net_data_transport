/*************************************************************************
  > File Name: test1.cpp
  > Author: rebot
  > Mail: 327767852@qq.com 
  > Created Time: Thu 02 Jun 2016 01:02:56 PM CST
 ************************************************************************/

#include <iostream>
#include "mutex_module.h"
using namespace std;

/*
   int main(void)
   {
   socket_module::CSocketClient client;
   IP ip = inet_addr("127.0.0.1");
   PORT port  = 9999;
   PROTOCOL protocol = SOCK_STREAM;

   client.create_sock(ip, port, protocol);
   client.connect_sock();

   st_message_header msg_header = {std::pair<int, int>(ip, port),
   std::pair<int, int>(ip, port),
   protocol};


   st_message msg;
   msg.m_syn = 1;
   memcpy(&msg.m_header, &msg_header, sizeof(st_message_header));
   event_module::st_event event = {msg,client.get_sock()};

   event_module::CFactory fac(event);
   fac.generate();
   fac.handle();

   return 0;
   }
   */

/*
   int main(void)
   {
   mutex_module::CMutex mutex;
//	mutex.lock();

mutex_module::CCondMutex cond(mutex);
cond.wait();
mutex.lock();
cond.signal();
mutex.unlock();
cout << "mutex_lock" << endl;

mutex_module::CMutex mutex1;

//	mutex1.lock();

cout << "mutex1_lock" << endl;
}
*/

#include "thread_module.h"
#include "event_module.h"
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
void handle_sigint(int signo)
{

}

void do_fork(int num_loop)
{
	socket_module::CSocketClient client;

	IP ip_dst = inet_addr("121.42.183.3");
	PORT port_dst = 6666;
	
	IP ip_mid = inet_addr("127.0.0.1");
	PORT port_mid  = 9999;
	
	PROTOCOL protocol = SOCK_STREAM;

	client.create_sock(ip_mid, port_mid, protocol);
	client.connect_sock();

	st_message_header msg_header = {std::pair<int, int>(ip_mid, port_mid),
		std::pair<int, int>(ip_dst, port_dst),
		protocol};

	signal(SIGINT, handle_sigint);

	st_message msg;
	msg.m_syn = 1;
	//	msg.m_txt = 1;
	memcpy(&msg.m_header, &msg_header, sizeof(st_message_header));
	st_event event = {client.get_sock(), msg};
	//	thmag.run(&event);
	std::cout << "syn start" << endl;

	CSynEvent syn(event);
	syn.handle_event();

//	sleep(2);
	
	char buf[30] = "";
	recv(client.get_sock(), buf, sizeof(buf), 0);
	printf("recv: %s\n", buf);
	for(int i = 0; i < num_loop; i++)
	{
		if(strcmp(buf, STR_OK.c_str()) == 0)
		{
			bzero(&msg, sizeof(st_message));
			memcpy(msg.m_data.m_content, "welcome to application net transport protocol's world!", 55);	
			msg.m_txt = 1;
			event = {client.get_sock(), msg};
			std::cout << "txt start" << std::endl;
		
			CTxtEvent txt(event);
			txt.handle_event();

			bzero(&event, sizeof(st_event));
			recv(client.get_sock(), &event, sizeof(st_event), 0);
			printf("recv msg:%s\n", event.m_msg.m_data.m_content);
		}
	}

	
//	syn.handle_event();
	st_event event2;
	bzero(&event2, sizeof(st_event));
	event2.m_msg.m_finc = 1;
	event2.m_sock = client.get_sock();	
	CFinEventClient fin(event2);
	fin.handle_event();
}

using namespace event_module;
using namespace thread_module;
int main(int argc, char **argv)
{
//	CThreadManger thmag(100);

	if(argc < 3)
	{
		printf("error for args num\n");
		exit(0);
	}

	for(int i = atoi(argv[2]); i > 0; --i)
	{
		int ret;

		if((ret = fork()) == 0)
		{
			/*child*/
			do_fork(atoi(argv[1]));
			return 0;
		}
		else
		{
			continue;
		}
	}

	int status;

//	while(waitpid(0, &status, WNOHANG) > 0)
//	{
//		;
//	}

	
	socket_module::CSocketClient client;

	IP ip_dst = inet_addr("121.42.183.3");
	PORT port_dst = 6666;
	
	IP ip_mid = inet_addr("127.0.0.1");
	PORT port_mid  = 9999;
	
	PROTOCOL protocol = SOCK_STREAM;

	client.create_sock(ip_mid, port_mid, protocol);
	client.connect_sock();

	st_message_header msg_header = {std::pair<int, int>(ip_mid, port_mid),
		std::pair<int, int>(ip_dst, port_dst),
		protocol};

	signal(SIGINT, handle_sigint);

	st_message msg;
	msg.m_syn = 1;
	//	msg.m_txt = 1;
	memcpy(&msg.m_header, &msg_header, sizeof(st_message_header));
	st_event event = {client.get_sock(), msg};
	//	thmag.run(&event);
	std::cout << "syn start" << endl;

	CSynEvent syn(event);
	syn.handle_event();

//	sleep(2);
	
	char buf[30] = "";
	recv(client.get_sock(), buf, sizeof(buf), 0);
	printf("recv: %s\n", buf);
	for(int i = 0; i < atoi(argv[1]); i++)
	{
		if(strcmp(buf, STR_OK.c_str()) == 0)
		{
			bzero(&msg, sizeof(st_message));
			memcpy(msg.m_data.m_content, "welcome to application net transport protocol's world!", 55);	
			msg.m_txt = 1;
			event = {client.get_sock(), msg};
			std::cout << "txt start" << std::endl;
		
			CTxtEvent txt(event);
			txt.handle_event();

			bzero(&event, sizeof(st_event));
			recv(client.get_sock(), &event, sizeof(st_event), 0);
			printf("recv msg:%s\n", event.m_msg.m_data.m_content);
		}
	}

	
//	syn.handle_event();
	st_event event2;
	bzero(&event2, sizeof(st_event));
	event2.m_msg.m_finc = 1;
	event2.m_sock = client.get_sock();	
	CFinEventClient fin(event2);
	fin.handle_event();

	return 0;
}
