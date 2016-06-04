/*************************************************************************
	> File Name: inner_invoke.h
	> Author: rebot
	> Mail: 327767852@qq.com 
	> Created Time: Tue 31 May 2016 09:32:34 AM CST
 ************************************************************************/
#ifndef __INNER_INVOKE_H
#define __INNER_INVOKE_H


#include "encrypt_python_version01.h"
#include "inner_message.h"
#include <sstream>

class Serialization
{
	public:
		static std::string Serialize(struct message_header& msg);
		static struct message_header DeSerialize(const std::string& message);
};

std::string Serialization::Serialize(struct message_header& msg)
{
    std::ostringstream archiveStream;
	boost::archive::text_oarchive archive(archiveStream);
	archive<<msg;
	return archiveStream.str();
}

struct message_header Serialization::DeSerialize(const std::string& message)
{
	struct message_header msg;
	std::istringstream archiveStream(message);
	boost::archive::text_iarchive archive(archiveStream);
	archive>>msg;
	return msg;
}
/*
bool encrypt(const char *, char *);
bool decrypt(const char *, struct message_header *);
*/

#endif
