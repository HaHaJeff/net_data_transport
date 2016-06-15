/*************************************************************************
  > File Name: inner_message.h
  > Author: rebot
  > Mail: 327767852@qq.com 
  > Created Time: Mon 30 May 2016 10:10:46 AM CST
 ************************************************************************/

#ifndef __INNER_MESSAGE_H
#define __INNER_MESSAGE_H

#include <utility>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <map>
#include <queue>
#include <list>

/*NATServer struct to <sockfdin, sockfdout>*/
namespace server_module
{
	std::map<int, int> g_sock_map;
	std::list<int> g_leave_queue;
}

typedef unsigned short int			U_INT_16;
typedef unsigned int				U_INT_32;

static const U_INT_32 CONTENT_LENGTH = 1460;
static const U_INT_32 IP_LENGTH      = 16;

typedef char					    CONTENT[CONTENT_LENGTH];  
typedef unsigned long				IP;
typedef U_INT_16					PORT;
typedef U_INT_32					PROTOCOL;
typedef std::pair<IP, PORT>         TUPLE_TWO;
#define TUPLE_LENGTH				(sizeof(TUPLE_TWO))
#define PROTOCOL_LENGTH             (sizeof(PROTOCOL))

/*define message construct*/
typedef struct message_header
{
	friend class boost::serialization::access;

	//use this function to serialize and deserialize object
	template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & m_src & m_dst & m_protocol;
		}

	TUPLE_TWO m_src;
	TUPLE_TWO m_dst;
	PROTOCOL  m_protocol;
}st_message_header;


typedef struct message_data
{
	CONTENT				m_content;
}st_message_data;

/*
   typedef enum _type
   {SYN=1, ACK, DATA, FIN}en_type;
   */

typedef struct bit_type
{
	unsigned int syn:1;
	unsigned int ack:1;
	unsigned int txt:1;
	unsigned int finc:1;
	unsigned int fins:1;
	unsigned int trans:1;
}st_type;

typedef struct message
{
	st_type           m_type;
	st_message_header m_header;
	st_message_data   m_data;
}st_message;

typedef struct event
{
	int   m_sock;
	st_message m_msg;
}st_event;

#define m_syn  m_type.syn
#define m_ack  m_type.ack 
#define m_txt m_type.txt
#define m_finc  m_type.finc
#define m_fins  m_type.fins
#define m_trans m_type.trans

#endif
