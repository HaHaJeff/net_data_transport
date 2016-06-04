/*************************************************************************
  > File Name: logfile_module.h
  > Author: rebot
  > Mail: 327767852@qq.com 
  > Created Time: Sat 04 Jun 2016 02:45:58 PM CST
 ************************************************************************/

#ifndef __LOGFILE_MODULE_H
#define __LOGFILE_MODULE_H

#include "mutex_module.h"
#include <queue>
#include <string>
#include <ctime>
#include <time.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace std;
using namespace mutex_module;

namespace logfile_module
{
	static const std::string log_file = "./log/log_file.txt";
	class CLogFile
	{
		public:
			CLogFile() : m_log_write_cond(CCondMutex(m_log_write_mutex)){
							 m_log_file.open(log_file.c_str(), ios::out | ios::app);
						 }
			
			CLogFile(string str) : m_log_write_cond(CCondMutex(m_log_write_mutex)){
							 m_log_file.open(str.c_str(), ios::out | ios::app);
						 }
			
			void push_log(std::string str)
			{
				__get_log_time();
				m_log_push_mutex.lock();
				std::string *pstr = new string(m_log_time + str);
				m_log_buff.push(pstr);
				m_log_push_mutex.unlock();
				m_log_write_cond.signal();
			}
			
			void write_log()
			{
				m_log_write_mutex.lock();
				while(m_log_buff.size() == 0)
					m_log_write_cond.wait();


				while(m_log_buff.size() != 0)
				{
					m_log_write_mutex.unlock();

					m_log_file << *(m_log_buff.front()) << endl;
					
					delete m_log_buff.front();

					m_log_buff.pop();
				
					m_log_write_mutex.unlock();
				}
			}

			~CLogFile()
			{
				if(m_log_buff.size() != 0)
					write_log();

				m_log_file.close();
			}

		private:
			void __get_log_time()
			{
				time_t raw_time;
				struct tm *time_info;
				time_info = localtime(&raw_time);
				m_log_time = asctime(time_info);
				m_log_time = string(m_log_time.begin(), m_log_time.begin() + 20);
				m_log_time += ":";
			}

		private:
			fstream m_log_file;
			string m_log_time;
			queue<std::string *> m_log_buff;
			CMutex m_log_push_mutex;		
			CCondMutex m_log_write_cond;
			CMutex m_log_write_mutex;
	};
}

#endif
