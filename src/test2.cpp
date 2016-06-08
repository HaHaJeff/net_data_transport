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

using namespace std;
using namespace event_module;
using namespace thread_module;
int main(void)
{
	
	socket_module::CSocketServer client;
	IP ip = inet_addr("127.0.0.1");
	PORT port  = 6666;
	PROTOCOL protocol = SOCK_STREAM;

	st_event event;
	client.create_sock(ip, port, protocol);
		
	client.listen_sock();

	while(1)
	{
		client.accept_sock();
		
		SOCKET_T conn = client.get_conn_sock();
		perror("accept_sock");
	
		bzero(&event, sizeof(event));
		std::cout << "peer socket" << conn << std::endl;
		int ret = recv(conn, &event, sizeof(event), 0);
		if(ret == -1)
		{
			if(errno == EAGAIN)
				continue;
			else
			{
				perror("recv");
			}
		}
		
		std::cout << "recv bytes" << ret << std::endl;
		std::cout << event.m_msg.m_data.m_content << std::endl;
		
		send(conn, &event, sizeof(st_event), 0);
	}
}
