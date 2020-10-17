#include "fusepy.h"
#include "addresses.h"
#include "dataformat.h"
#include "text.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

static PyMethodDef fusepy_functions[] = {
    {NULL},
};

static PyModuleDef fusepymodule = {PyModuleDef_HEAD_INIT,
                                   "_fusepy",
                                   "Classes and functions for data extraction and insertion",
                                   -1,
                                   fusepy_functions,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL};

PyMODINIT_FUNC PyInit__fusepy(void)
{
    PyObject* m = PyModule_Create(&fusepymodule);
    if (m == NULL)
    {
        return NULL;
    }
    if (!registerDataFormatTypes(m))
    {
        return NULL;
    }
    if (!registerFuseText(m))
    {
        return NULL;
    }
    if (!registerFuseAddresses(m))
    {
        return NULL;
    }
    return m;
}
