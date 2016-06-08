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

void handle_sigint(int signo)
{

}

using namespace event_module;
using namespace thread_module;
int main(void)
{
	CThreadManger thmag(100);
	
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
	event_module::st_event event = {client.get_sock(), msg};
//	thmag.run(&event);
	CFactory fac(event);	
	fac.generate();
	fac.handle();
	
	char buf[30] = "";
	recv(client.get_sock(), buf, sizeof(buf), 0);
	
	if(strcmp(buf, STR_OK.c_str()) == 0)
	{
		bzero(&msg, sizeof(st_message));
		memcpy(msg.m_data.m_content, "welcome to application net transport protocol's world!", 55);	
		msg.m_txt = 1;
		event = {client.get_sock(), msg};
		std::cout << client.get_sock();			
		CFactory fac1(event);
		fac1.generate();
		fac1.handle();
	}

	st_event event1;
	bzero(&event1, sizeof(st_event));
	recv(client.get_sock(), &event1, sizeof(st_event), 0);
	fputs(event1.m_msg.m_data.m_content, stdout);
	//	while(1);
	return 0;
}
