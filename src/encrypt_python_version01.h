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
		
		CPython(const CPython&)=delete;

		CPython operator=(const CPython&)=delete;

		CPython(std::string name) : m_name(name)
		{
			Py_Initialize();
			__initial();
		}
		
		~CPython()
		{
			Py_Finalize();
			__py_decref();
		}

		bool initial()
		{
			__py_decref();
			if(!Py_IsInitialized())
				return false;

			PyRun_SimpleString("import sys");
			PyRun_SimpleString("sys.path.append('./')");

			m_p_name   = PyString_FromString(m_name.c_str());
			m_p_module = PyImport_Import(m_p_name);
			if(m_p_module == NULL)
				return false;

			m_p_dict   = PyModule_GetDict(m_p_module);
			if(m_p_dict == NULL)
				return false;

			return true;
		}

		bool exec(const std::string function_name, const std::string args)
		{
			m_p_func = PyDict_GetItemString(m_p_dict, function_name.c_str());
			if(!m_p_func || !PyCallable_Check(m_p_func))
				return false;
		
			m_p_args = PyTuple_New(1);          
			PyTuple_SetItem(m_p_args, 0, Py_BuildValue("s", args.c_str()));
			if(!m_p_args)
				return false;

			m_p_result = PyEval_CallObject(m_p_func, m_p_args);     
			PyArg_Parse(m_p_result, "s", &m_result);		
			if(!m_p_result)
				return false;

			return true;
		}

		const  char *get_result()
		{
			return m_result;
		}

	private:
		void __py_decref()
		{
			if(m_p_name->ob_refcnt )
			{
				Py_DECREF(m_p_name);
			}
			if(m_p_func->ob_refcnt)
			{			
				Py_DECREF(m_p_func);
			}
			if(m_p_module->ob_refcnt)
			{
				Py_DECREF(m_p_module);
			}
			if(m_p_dict->ob_refcnt)	
			{
				Py_DECREF(m_p_dict);
			}
			if(m_p_result->ob_refcnt)
			{
				Py_DECREF(m_p_result);
			}
			if(m_p_args->ob_refcnt)
			{
				Py_DECREF(m_p_args);
			}
		}
		
		void __initial()
		{
			m_p_args   = new PyObject();
			m_p_dict   = new PyObject();
			m_p_func   = new PyObject();
			m_p_name   = new PyObject();
			m_p_module = new PyObject();
			m_p_result = new PyObject();
		}

		PyObject *m_p_result;
		PyObject *m_p_args;
		PyObject *m_p_name;
		PyObject *m_p_module;
		PyObject *m_p_dict;
		PyObject *m_p_func;
		std::string m_name;
		const char *m_result;
};

#endif
