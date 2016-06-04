/*************************************************************************
	> File Name: test2.cpp
	> Author: rebot
	> Mail: 327767852@qq.com 
	> Created Time: Sat 04 Jun 2016 03:42:22 PM CST
 ************************************************************************/
#include "logfile_module.h"
#include <pthread.h>

using namespace logfile_module;

void *func(void *args)
{
	CLogFile *log = (CLogFile *)args;
	log->write_log();
}


int main()
{
	CLogFile log;
	pthread_t pid;
	pthread_create(&pid, NULL, func, &log);
	void *ret;
	log.push_log("i have access you 1, you know!");
	log.push_log("i have access you 2, you know!");
	log.push_log("i have access you 3, you know!");
	log.push_log("i have access you 4, you know!");
	log.push_log("i have access you 5, you know!");
	log.push_log("i have access you 6, you know!");
	log.push_log("i have access you 7, you know!");
	log.push_log("i have access you 8, you know!");
	log.push_log("i have access you 9, you know!");
	log.push_log("i have access you a, you know!");
	log.push_log("i have access you b, you know!");
	log.push_log("i have access you c, you know!");
	log.push_log("i have access you d, you know!");
	log.push_log("i have access you e, you know!");
	log.push_log("i have access you f, you know!");
	pthread_join(pid, &ret);
}
