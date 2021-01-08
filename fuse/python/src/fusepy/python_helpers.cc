#include "python_helpers.h"

auto splitKeywordArgs(PyObject* kwargs, const char** keywords)
    -> std::optional<std::pair<PyObject*, PyObject*>>
{
    PyObject *key{nullptr}, *value{nullptr};
    Py_ssize_t pos{0};

    PyObject* matches = PyDict_New();
    PyObject* remaining = PyDict_New();

    while (PyDict_Next(kwargs, &pos, &key, &value))
    {
        if (PyUnicode_Check(key))
        {
            const char** keyword = keywords;
            bool found{false};
            while (*keyword != nullptr)
            {
                if (PyUnicode_CompareWithASCIIString(key, *keyword) == 0)
                {
                    found = true;
                    break;
                }
                ++keyword;
            }
            if (found)
            {
                PyDict_SetItem(matches, key, value);
            }
            else
            {
                PyDict_SetItem(remaining, key, value);
            }
        }
        else
        {
            Py_DECREF(matches);
            Py_DECREF(remaining);
            return {};
        }
    }

    return std::make_pair(matches, remaining);
}
