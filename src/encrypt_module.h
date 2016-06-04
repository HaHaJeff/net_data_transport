/*************************************************************************
  > File Name: encrypt_module.h
  > Author: rebot
  > Mail: 327767852@qq.com 
  > Created Time: Tue 31 May 2016 09:03:00 PM CST
 ************************************************************************/

#include "inner_invoke.h"
#include <string>

namespace encrypt_module{
	const std::string STR_PYTHON = "aes_encrypt_cpp";
	const std::string STR_ENCRYPT = "encrypt";
	const std::string STR_DECRYPT = "decrypt";

	bool construct_transport_message_header(struct message_header msg,
			std::string str_python,
			std::string str_encrypt,
			std::string &trans_msg)
	{
		CPython py(str_python.c_str());
		py.initial();
		std::string str = Serialization::Serialize(msg);
		if(str.empty())
			return false;
		py.exec(str_encrypt, str);
		trans_msg = py.get_result();
		if(trans_msg.empty())
			return false;
		return true;
	}


	bool parse_transport_message_header(struct message_header &msg,
			std::string str_python,
			std::string str_decrypt,
			std::string trans_msg)
	{
		CPython py(str_python.c_str());
		py.initial();
		py.exec(str_decrypt, trans_msg);
		std::string str = py.get_result();
		msg = Serialization::DeSerialize(str);
		return true;
	}
}
