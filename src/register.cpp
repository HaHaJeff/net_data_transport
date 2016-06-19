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
using namespace std;
int main()
{
	map<string, int> my_map;
	string ip;
	int port;
	cout<<"please cin ip and port"<<endl;

	while(1) {
		cin >> ip;
		cin >> port;
		if(port == 0)
		{
			break;
		}
		my_map.insert(make_pair(ip, port));
	}
	//读入文本
	fstream save_file;
	
	save_file.open("register.txt", ios::out | ios::app);

	map<string, int>::iterator iter=my_map.begin();
	
	for(;iter!=my_map.end();iter++){
		save_file << iter->first <<" "<< iter->second << endl; 
	}


	return 0;
}
