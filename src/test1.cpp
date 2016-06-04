/*************************************************************************
	> File Name: test1.cpp
	> Author: rebot
	> Mail: 327767852@qq.com 
	> Created Time: Thu 02 Jun 2016 01:02:56 PM CST
 ************************************************************************/
#include <iostream>
//#include "event_module.h"
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
									std::pair<int, int>(ip, port),
									protocol};


	st_message msg;
	msg.m_syn = 1;
	memcpy(&msg.m_header, &msg_header, sizeof(st_message_header));
	event_module::st_event event = {msg,client.get_sock()};
	thmag.run(&event);

	return 0;
}
