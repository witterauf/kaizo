#include "objects.h"
#include "addresses.h"

using namespace fuse;

auto toPython(const AnnotatedBinary& binary) -> PyObject*
{
    PyObject* pyObjectList = PyList_New(binary.objectCount());
    for (size_t i = 0; i < binary.objectCount(); ++i)
    {
        auto const& object = *binary.object(i);
        PyObject* pyObject = toPython(object);

        auto const offset = static_cast<const PackedObject&>(object).offset();
        PyObject* pyBinary = PyByteArray_FromStringAndSize(
            reinterpret_cast<const char*>(binary.binary().data() + offset), object.size());
        PyDict_SetItemString(pyObject, "binary", pyBinary);

        PyList_SetItem(pyObjectList, i, pyObject);
    }
    return pyObjectList;
}

auto toPython(const Object::Section& section) -> PyObject*
{
    PyObject* pySection = PyDict_New();
    PyObject* pyOffset = Py_BuildValue("K", static_cast<unsigned long long>(section.offset));
    PyDict_SetItemString(pySection, "offset", pyOffset);
    PyObject* pyActualOffset =
        Py_BuildValue("K", static_cast<unsigned long long>(section.realOffset));
    PyDict_SetItemString(pySection, "actual_offset", pyActualOffset);
    PyObject* pySize = Py_BuildValue("K", static_cast<unsigned long long>(section.size));
    PyDict_SetItemString(pySection, "size", pySize);
    return pySection;
}

auto toPython(const Object& object) -> PyObject*
{
    PyObject* pyObject = PyDict_New();

    PyObject* pyPath = PyUnicode_FromStringAndSize(object.path().toString().c_str(),
                                                   object.path().toString().length());
    PyDict_SetItemString(pyObject, "path", pyPath);

    if (object.hasFixedOffset())
    {
        PyDict_SetItemString(
            pyObject, "fixed_offset",
            Py_BuildValue("K", static_cast<unsigned long long>(object.fixedOffset())));
    }

    if (object.alignment() != 1)
    {
        PyDict_SetItemString(
            pyObject, "alignment",
            Py_BuildValue("K", static_cast<unsigned long long>(object.alignment())));
    }

    PyObject* pyActualSize = Py_BuildValue("K", static_cast<unsigned long long>(object.realSize()));
    PyDict_SetItemString(pyObject, "actual_size", pyActualSize);

    PyObject* pyRefList = PyList_New(object.unresolvedReferenceCount());
    for (size_t i = 0; i < object.unresolvedReferenceCount(); ++i)
    {
        PyObject* pyRef = toPython(object.unresolvedReference(i));
        PyList_SetItem(pyRefList, i, pyRef);
    }
    PyDict_SetItemString(pyObject, "unresolved", pyRefList);

    PyObject* pySectionList = PyList_New(object.sectionCount());
    for (size_t i = 0; i < object.sectionCount(); ++i)
    {
        PyObject* pySection = toPython(object.section(i));
        PyList_SetItem(pySectionList, i, pySection);
    }
    PyDict_SetItemString(pyObject, "sections", pySectionList);

    return pyObject;
}

auto toPython(const UnresolvedReference& ref) -> PyObject*
{
    PyObject* pyRef = PyDict_New();
    PyObject* pyOffset = Py_BuildValue("K", static_cast<unsigned long long>(ref.relativeOffset()));
    PyDict_SetItemString(pyRef, "offset", pyOffset);
    PyObject* pyPath = PyUnicode_FromStringAndSize(ref.referencedPath().toString().c_str(),
                                                   ref.referencedPath().toString().length());
    PyDict_SetItemString(pyRef, "path", pyPath);

    PyObject* pyLayout = PyAddressLayout_New(ref.addressLayout().copy());
    PyDict_SetItemString(pyRef, "layout", pyLayout);

    return pyRef;
}