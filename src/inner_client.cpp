/*************************************************************************
  > File Name: test1.cpp
  > Author: rebot
  > Mail: 327767852@qq.com 
  > Created Time: Thu 02 Jun 2016 01:02:56 PM CST
 ************************************************************************/

#include <iostream>
#include "mutex_module.h"

#include "thread_module.h"
#include "event_module.h"
#include <signal.h>
#include <string.h>
#include <stdlib.h>

void handle_sigint(int signo)
{

}

using namespace std;
using namespace event_module;
using namespace thread_module;
int main(int argc, char **argv)
{

	if(argc < 2)
	{
		printf("error for args num\n");
		exit(0);
	}
	socket_module::CSocketClient client;
	IP ip = inet_addr("127.0.0.1");
	PORT port  = 9999;
	PROTOCOL protocol = SOCK_STREAM;

	client.create_sock(ip, port, protocol);
	client.connect_sock();

	st_message_header msg_header = {std::pair<int, int>(ip, port),
		std::pair<int, int>(ip, 6666),
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
