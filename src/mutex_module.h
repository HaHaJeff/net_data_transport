/*************************************************************************
  > File Name: mutex_module.h
  > Author: rebot
  > Mail: 327767852@qq.com 
  > Created Time: Thu 02 Jun 2016 12:38:41 PM CST
 ************************************************************************/
#ifndef __MUTEX_MODULE_H
#define __MUTEX_MODULE_H

#include <map>
#include <boost/noncopyable.hpp>
#include <pthread.h>
/*single instance */
namespace mutex_module
{
	class CMutex 
	{	
		public:
			CMutex()
			{
				pthread_mutex_init(&m_mutex, NULL);
			}

			void lock()
			{
				pthread_mutex_lock(&m_mutex);
			}

			void unlock()
			{
				pthread_mutex_unlock(&m_mutex);
			}
			
			pthread_mutex_t& get_mutex(){return m_mutex;}

			~CMutex()
			{
				pthread_mutex_destroy(&m_mutex);
			}
		private:
			pthread_mutex_t m_mutex;
	};


	class CCondMutex
	{
		public:
			CCondMutex(CMutex &mutex) : m_mutex(mutex)
			{
				pthread_cond_init(&m_cond, NULL);
			}
		
			void wait()
			{
				pthread_cond_wait(&m_cond, &m_mutex.get_mutex());
			}

			void signal()
			{
				pthread_cond_signal(&m_cond);
			}

			~CCondMutex()
			{
				pthread_cond_destroy(&m_cond);
			}
		private:
			pthread_cond_t m_cond;
			CMutex &m_mutex;
	};
}

#endif
