/*************************************************************************
	> File Name: encrypt_python.h
	> Author: rebot
	> Mail: 327767852@qq.com 
	> Created Time: Mon 30 May 2016 01:04:34 PM CST
 ************************************************************************/

#ifndef __INNER_FUNCTION_H
#define __INNER_FUNCTION_H
#include <iostream>
#include <python2.7/Python.h>
#include <string>
#include <boost/smart_ptr.hpp>

class CPython
{
	public:
		CPython() : m_name(""){}

		CPython(std::string name) : m_name(name)
		{
			Py_Initialize();
		}
		
		~CPython()
		{
			Py_Finalize();
		}

		bool initial(const std::string &function_name, const char *args)
		{
			if(!Py_IsInitialized())
				return false;

			PyRun_SimpleString("import sys");
			PyRun_SimpleString("sys.path.append('./')");

			PyObject *p_name   = PyString_FromString(m_name.c_str());
			PyObject *p_module = PyImport_Import(p_name);
			if(p_module == NULL)
				return false;

			PyObject *p_dict   = PyModule_GetDict(p_module);
			if(p_dict == NULL)
				return false;
				
			PyObject *p_func   = PyDict_GetItemString(p_dict, function_name.c_str());
			if(!p_func || !PyCallable_Check(p_func))
				return false;
		
			PyObject *p_args = PyTuple_New(1);          
			PyTuple_SetItem(p_args, 0, Py_BuildValue("s", args));
						
			PyObject *p_result = PyEval_CallObject(p_func, p_args);     
			PyArg_Parse(p_result, "s", &m_result);

			Py_DECREF(p_result);
			Py_DECREF(p_func);
			Py_DECREF(p_args);
			Py_DECREF(p_module);
			Py_DECREF(p_name);
			Py_DECREF(p_dict);

			return true;
		}
		
		const  char *get_result()
		{
			return m_result;
		}

	private:

		std::string m_name;
		const char *m_result;
};

#endif
