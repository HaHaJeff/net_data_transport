/*************************************************************************
  > File Name: register.cpp
  > Author: rebot
  > Mail: 327767852@qq.com 
  > Created Time: Thu 16 Jun 2016 04:21:54 PM CST
 ************************************************************************/
#include<iostream>
#include<fstream>
#include<string>
#include<map>
#include<utility>
#include<stdio.h>
#include<stdlib.h>
using namespace std;
int main()
{
	string ip_modify;
	int port_modify;

	map<string, int> my_map;
	string ip;
	int port;
	
	ifstream file("register.txt");

	while(!file.eof()){
		file >> ip >> port;
		my_map.insert(make_pair(ip, port));
	}
	
	cout << "please cin ip and port to modify" << endl;
	while(1)
	{
		cin >> ip_modify >> port_modify;
		
		if(port_modify == 0)
		{
			break;
		}

		map<string, int>::iterator iter_modify = my_map.find(ip_modify);

		if(iter_modify != my_map.end())
			my_map.erase(iter_modify);
	}

	//读入文本
	fstream save_file;
	
	save_file.open("register.txt", ios::out);

	map<string, int>::iterator iter=my_map.begin();
	
	for(;iter!=my_map.end();iter++){
		save_file << iter->first <<" "<< iter->second << endl; 
	}
	return 0;
}
