/*************************************************************************
  > File Name: thread_module.h
  > Author: rebot
  > Mail: 327767852@qq.com 
  > Created Time: Sat 04 Jun 2016 08:29:28 AM CST
 ************************************************************************/

#ifndef __THREAD_MODULE_H
#define __THREAD_MODULE_H
#include "mutex_module.h"
#include "event_module.h"
#include <vector>

using namespace mutex_module;
using namespace event_module;

namespace thread_module
{
	static const bool BUSY = false;
	static const bool IDLE = true;
	static const int MAX_THREADS = 100;

	class CWorkThread
	{
		public:
			CWorkThread() : m_job_cond(CCondMutex(m_job_mutex))
			{
				m_event = NULL;
				m_fac = NULL;
			}
			
			void run()
			{
				while(m_event == NULL)
					m_job_cond.wait();
				m_state = BUSY;
				m_fac = new CFactory(*m_event);
				m_fac->generate();
				m_fac->handle();
				m_state = IDLE;
			}

			void set_event(st_event *event)
			{
				m_job_mutex.lock();
				m_event = new st_event;
				memcpy(m_event, event, sizeof(st_event));
				m_job_mutex.unlock();
				m_job_cond.signal();
			}
			
			~CWorkThread()
			{
				if(m_event != NULL)
				{
					delete m_event;
					m_event = NULL;
				}

				if(m_fac != NULL)
				{
					delete m_fac;
				}
			}
		private:
			CMutex		m_job_mutex;
			CCondMutex  m_job_cond;
			st_event*   m_event;
			CFactory*   m_fac;
		public:
			bool        m_state;
			
	};

	class CThreadPool
	{
		public:
			CThreadPool() : m_threads(1), m_idle_cond(CCondMutex(m_idle_mutex)),m_busy_cond(CCondMutex(m_busy_mutex)){}
			CThreadPool(int num ) : m_idle_cond(CCondMutex(m_idle_mutex)),m_busy_cond(CCondMutex(m_busy_mutex))
			{
				m_threads = (num < MAX_THREADS) ? num : MAX_THREADS;
				__create_idle_thread();
			}
				
			void run(st_event *event)
			{
				while(m_idle_list.size() == 0)
					m_idle_cond.wait();
				
				m_idle_mutex.lock();
				CWorkThread *work = *(m_idle_list.begin());
				__move_to_busy(work);
				work->set_event(event);
				work->run();
				__invoke_move_to_idle();
			}

		private:
			void __invoke_move_to_idle()
			{
				std::vector<CWorkThread *>::iterator iter = m_busy_list.begin();
				while(iter != m_busy_list.end())
				{
					if((*iter)->m_state == IDLE)
					{
						__move_to_idle(*iter);
						/*to begin delete cause iter valid*/
						iter = m_busy_list.begin();
					}
					else
						iter++;
				}
			}

			void __append_to_idle(CWorkThread *work)
			{
				m_idle_mutex.lock();
				m_idle_list.push_back(work);
				m_idle_mutex.unlock();
			}

			void __move_to_idle(CWorkThread *work)
			{
				m_idle_mutex.lock();
				m_idle_list.push_back(work);
				std::vector<CWorkThread *>::iterator iter = find(m_idle_list.begin(), m_idle_list.end(), work);
				if(iter != m_busy_list.end())
					m_busy_list.erase(iter);
				m_idle_mutex.unlock();
				m_idle_cond.signal();
			}

			void __move_to_busy(CWorkThread *work)
			{
				m_busy_mutex.lock();
				m_busy_list.push_back(work);
				std::vector<CWorkThread *>::iterator iter = find(m_busy_list.begin(), m_busy_list.end(), work);
				if(iter != m_busy_list.end())
					m_busy_list.erase(iter);
				m_busy_mutex.unlock();
				m_busy_cond.signal();
			}

			void __create_idle_thread()
			{
				for(int i = 0; i < m_threads; i++)
				{
					CWorkThread *work = new CWorkThread;
					__append_to_idle(work);
				}
			}

		private:
			CMutex m_idle_mutex;
			CCondMutex m_idle_cond;
			CMutex m_busy_mutex;
			CCondMutex m_busy_cond;
			int m_threads;
			std::vector<CWorkThread *> m_idle_list;
			std::vector<CWorkThread *> m_busy_list;
		
		protected:
	};

	class CThreadManger
	{
		public:
			CThreadManger(int num)
			{
				m_threads = num;
				m_pool = new CThreadPool(num);
			}

			void run(st_event *event)
			{
				m_pool->run(event);
			}
		private:
			CThreadPool *m_pool;
			int m_threads;
	};
}
#endif
