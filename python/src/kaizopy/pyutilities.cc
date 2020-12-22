#include "pyutilities.h"

using namespace fuse;

bool pykCheckArguments(const Py_ssize_t nargs, const Py_ssize_t expected, const std::string& names)
{
    if (nargs != expected)
    {
        PyErr_SetString(PyExc_TypeError,
                        ("got " + std::to_string(nargs) + " arguments, but expected " +
                         std::to_string(expected) + " arguments (" + names + ")")
                            .c_str());
        return false;
    }
    return true;
}

PyBufferWrapper::PyBufferWrapper(PyObject* object, const bool writable)
{
    if (PyObject_GetBuffer(object, &m_pyBuffer,
                           PyBUF_C_CONTIGUOUS | PyBUF_SIMPLE | (writable ? PyBUF_WRITEABLE : 0)) >=
        0)
    {
        m_valid = true;
    }
}

PyBufferWrapper::~PyBufferWrapper()
{
    if (m_valid)
    {
        PyBuffer_Release(&m_pyBuffer);
    }
}

bool PyBufferWrapper::isValid() const
{
    return m_valid;
}

auto PyBufferWrapper::view() const -> BinaryView
{
    return BinaryView{reinterpret_cast<const uint8_t*>(m_pyBuffer.buf),
                      static_cast<size_t>(m_pyBuffer.len)};
}

auto PyBufferWrapper::mutableView() const -> fuse::MutableBinaryView
{
    return MutableBinaryView{reinterpret_cast<uint8_t*>(m_pyBuffer.buf),
                             static_cast<size_t>(m_pyBuffer.len)};
}

PyBufferWrapper::operator bool() const
{
    return isValid();
}

auto pykGetBuffer(PyObject* object) -> PyBufferWrapper
{
    return PyBufferWrapper{object, false};
}

auto pykGetWritableBuffer(PyObject* object) -> PyBufferWrapper
{
    return PyBufferWrapper{object, true};
}

auto pykGetString(PyObject* arg) -> std::optional<std::string>
{
    const char* szArg = PyUnicode_AsUTF8(arg);
    if (!szArg)
    {
        return {};
    }
    return szArg;
}
