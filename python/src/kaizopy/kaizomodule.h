#pragma once

#include <map>
#include <string>
#include <utility>
#include <vector>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#ifdef _WIN32
#ifdef KAIZO_WIN_EXPORT
#define KAIZO_EXPORTED __declspec(dllexport)
#else
#define KAIZO_EXPORTED __declspec(dllimport)
#endif
#else
#define KAIZO_EXPORTED
#endif

class KaizoModule
{
public:
    virtual ~KaizoModule() = default;
    virtual bool initialize();
    virtual auto createTypes() -> std::map<std::string, PyTypeObject*>;
    virtual auto createFunctions() -> std::vector<PyMethodDef>;
};