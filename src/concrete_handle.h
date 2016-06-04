/*************************************************************************
  > File Name: concrete_handle.h
  > Author: rebot
  > Mail: 327767852@qq.com 
  > Created Time: Thu 02 Jun 2016 02:58:57 PM CST
 ************************************************************************/

#ifndef __CONCRETE_HANDLE_H
#define __CONCRETE_HANDLE_H
#include <string>

namespace user_module
{ 
	class CConcreteHandle
	{
		public:
			CConcreteHandle(){}
			void set_buf(std::string str){m_str_buff = str;}
			std::string get_buf(){return m_str_buff;}
			virtual void handle_event(){}
		private:
			std::string m_str_buff;
	};
}
#endif
