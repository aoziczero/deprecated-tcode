#include "stdafx.h"
#include "python.hpp"

#include <Python.h>


namespace tcode {

bool python_init()
{
	Py_Initialize();
	return true;
}

}
