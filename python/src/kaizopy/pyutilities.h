#pragma once

#include <fuse/BinaryView.h>
#include <optional>
#include <string>
#include <type_traits>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

bool pykCheckArguments(const Py_ssize_t nargs, const Py_ssize_t expected, const std::string& names);

class PyBufferWrapper
{
public:
    explicit PyBufferWrapper(PyObject* object, const bool writable);
    ~PyBufferWrapper();

    bool isValid() const;
    auto view() const -> fuse::BinaryView;
    auto mutableView() const -> fuse::MutableBinaryView;

    operator bool() const;

private:
    bool m_valid{false};
    Py_buffer m_pyBuffer;
};

auto pykGetBuffer(PyObject* object) -> PyBufferWrapper;
auto pykGetWritableBuffer(PyObject* object) -> PyBufferWrapper;

template <class T> auto pykGetNumber(PyObject* arg) -> std::optional<T>
{
    if constexpr (std::is_unsigned<T>::value)
    {
        if constexpr (sizeof(T) == sizeof(unsigned long))
        {
            auto const number = PyLong_AsUnsignedLong(arg);
            if (number == static_cast<decltype(number)>(-1) && PyErr_Occurred())
            {
                return {};
            }
            return number;
        }
        else if constexpr (sizeof(T) == sizeof(unsigned int))
        {
            auto const number = PyLong_AsUnsignedLong(arg);
            if (number == static_cast<decltype(number)>(-1) && PyErr_Occurred())
            {
                return {};
            }
            return static_cast<unsigned int>(number);
        }
        else if constexpr (sizeof(T) == sizeof(unsigned long long))
        {
            auto const number = PyLong_AsUnsignedLongLong(arg);
            if (number == static_cast<decltype(number)>(-1) && PyErr_Occurred())
            {
                return {};
            }
            return number;
        }
        else
        {
            static_assert(false, "unsupported");
        }
    }
    else
    {
        static_assert(false, "unsupported");
    }
}

template <class Instance> auto pykGetObject(PyObject* arg, PyTypeObject* type) -> Instance*
{
    if (!PyObject_IsInstance(arg, (PyObject*)type))
    {
        PyErr_SetString(PyExc_TypeError,
                        ("tile must be an instance of " + std::string{type->tp_name}).c_str());
        return nullptr;
    }
    return reinterpret_cast<Instance*>(arg);
}

auto pykGetString(PyObject* arg) -> std::optional<std::string>;
