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
#include "logfile_module.h"
#include <vector>
#include <stdio.h>

using namespace mutex_module;
using namespace event_module;
using namespace logfile_module;
using namespace std;

namespace thread_module
{
#define gettid() (__NR_gettid)

	static const bool BUSY = false;
	static const bool IDLE = true;
	static const int MAX_THREADS = 100;

	class CWorkThread
	{
		public:
			CWorkThread() : m_job_cond(m_job_mutex)
		{
			m_event = NULL;
			m_fac = NULL;
			m_state = IDLE;

			//		printf("this:%p\n", this);
			pthread_create(&m_tid, NULL, thread_function, this);
			stringstream ss;
			ss << m_tid;
			m_str_id = ss.str();
		}

			static void *thread_function(void *args)
			{
				CWorkThread *thread = (CWorkThread *)args;
				thread->run();
			}

			void run()
			{
				while(1)
				{
					m_job_mutex.lock();
					
					while(m_event == NULL)
					{
						m_job_cond.wait();
					}
				
					m_job_mutex.unlock();

					m_state = BUSY;
					SOCKET_T sock = m_event->m_sock;

					/*once job*/
					//		while(1)
					//		{
					int ret = recv(m_event->m_sock,m_event, 20, MSG_PEEK);
				
					/*for debug*/
					if((m_event->m_msg).m_trans == 1)
					{
			//			printf("sleep for m_trans start\n");
			//			sleep(2);
			//			printf("sleep for m_trans end\n");

					}
					if(ret == -1)
					{
						if(errno == EAGAIN)
						{
							//printf("recv errno is EAGAIN\n");
							continue;
						}
					}

					m_event->m_sock = sock;
					CFactory fac(*m_event);
					m_fac = &fac;
					m_fac->generate();
					m_fac->handle();

					/*fin for thread terminate*/
					//			if((m_event->m_msg).m_fins == 1)
					//			{
					//				printf("thread fins\n");
					//				break;
					//			}

					//			if((m_event->m_msg).m_ack == 1)
					//			{
					//				printf("thread ack\n");
					//				break;
					//			}
					//		}

					printf("%s thread end\n", get_id().c_str());
				
					if(m_event != NULL)
					{
						printf("in thread run delete m_event\n");
						delete m_event;
						m_event = NULL;
					}
					
					m_state = IDLE;
				}
			}

			void set_event(st_event *event)
			{
				m_job_mutex.lock();
				m_event = event;
				m_job_mutex.unlock();
				m_job_cond.signal();
			}

			~CWorkThread()
			{
				printf("%s workthread ~CWorkThread\n", get_id());
				if(m_event != NULL)
				{
					delete m_event;
					m_event = NULL;
				}

			}

			std::string get_id()
			{
				return m_str_id;
			}

		private:
			CMutex		m_job_mutex;
			CCondMutex  m_job_cond;
			st_event*   m_event;
			CFactory*   m_fac;
			std::string m_str_id;
			pthread_t   m_tid;
		public:
			bool        m_state;

	};

	class CThreadPool
	{
		public:
			CThreadPool() : m_threads(1), m_idle_cond(m_idle_mutex),m_busy_cond(m_busy_mutex),m_move_cond(m_move_mutex){}

			CThreadPool(int num ) : m_idle_cond(m_idle_mutex),m_busy_cond(m_busy_mutex),m_move_cond(m_move_mutex)
			{
				m_threads = (num < MAX_THREADS) ? num : MAX_THREADS;
				__create_idle_thread();
			}

			void run(st_event *event)
			{
				while(m_idle_list.size() == 0)
				{
			//		printf("m_idle_list.size == 0\n");
					m_idle_cond.wait();
			//		printf("m_idle_list size = %d\n", m_idle_list.size());
				}

				CWorkThread *work = *(m_idle_list.begin());

				std::string str_log;
				str_log = (work->get_id() + ":thread start");
				g_thread_log.push_log(str_log);

				__move_to_busy(work);

				work->set_event(event);

				printf("%s thread start\n",work->get_id().c_str());
				
				__invoke_move_to_idle();
			}

		private:
			void __invoke_move_to_idle()
			{
				m_move_mutex.lock();
				
				std::vector<CWorkThread *>::iterator iter = m_busy_list.begin();
			
//				while(m_busy_list.size() != 0 && iter != m_busy_list.end())
//				{
					if(iter != m_busy_list.end() && *iter != NULL && (*iter)->m_state == IDLE)
					{
						printf("move to idle pthread id:%s\n", ((*iter)->get_id()).c_str());
						__move_to_idle(*iter++);
					}
//					else
//						iter++;
//				}
				m_move_mutex.unlock();
			}

			void __append_to_idle(CWorkThread *work)
			{
				m_idle_mutex.lock();
				
				if(work != NULL)
					m_idle_list.push_back(work);
				
				m_idle_mutex.unlock();
			}

			void __move_to_idle(CWorkThread *work)
			{
				m_idle_mutex.lock();
				m_busy_mutex.lock();
				
				if(work != NULL)
					m_idle_list.push_back(work);
				
				std::vector<CWorkThread *>::iterator iter = find(m_busy_list.begin(), m_busy_list.end(), work);
				
				if(iter != m_busy_list.end())
					m_busy_list.erase(iter);
				
				m_idle_mutex.unlock();
				m_busy_mutex.unlock();
				m_idle_cond.signal();
			}

			void __move_to_busy(CWorkThread *work)
			{
				m_busy_mutex.lock();
				m_idle_mutex.lock();
				
				if(work != NULL)
					m_busy_list.push_back(work);
				
				std::vector<CWorkThread *>::iterator iter = find(m_idle_list.begin(), m_idle_list.end(), work);
				
				if(iter != m_idle_list.end())
					m_idle_list.erase(iter);
				
				m_busy_mutex.unlock();
				m_idle_mutex.unlock();
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
			CMutex m_move_mutex;
			CCondMutex m_move_cond;
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
