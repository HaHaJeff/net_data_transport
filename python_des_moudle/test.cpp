/*************************************************************************
	> File Name: test.cpp
	> Author: rebot
	> Mail: 327767852@qq.com 
	> Created Time: Tue 31 May 2016 02:18:23 PM CST
 ************************************************************************/
#include "./encrypt_python_version01.h"
#include <iostream>
using namespace std;
int main()
{
	const char *str = "MSGF_FUCK";

	CPython p1("aes_encrpty_cpp");
	p1.initial();
	p1.exec("encrypt", str);
	const char *estr = p1.get_result();
	cout << estr << endl;

	p1.exec("decrypt", estr);
	const char *dstr = p1.get_result();
//	CPython p2("ase_encrypt_cpp");
//	p2.initial("decrypt", estr);
//	const char *dstr = p2.get_result();
	cout << dstr << endl;
	cout << estr << endl;
}
