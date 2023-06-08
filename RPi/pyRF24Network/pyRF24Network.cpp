#include "boost/python.hpp"
#include "RF24/RF24.h"
#include "RF24Network/RF24Network.h"

namespace bp = boost::python;

// **************** explicit wrappers *****************
// where needed, especially where buffer is involved

void throw_ba_exception(void)
{
    PyErr_SetString(PyExc_TypeError, "buf parameter must be bytes or bytearray");
    bp::throw_error_already_set();
}

char* get_bytes_or_bytearray_str(bp::object buf)
{
    PyObject* py_ba;
    py_ba = buf.ptr();
    if (PyByteArray_Check(py_ba))
        return PyByteArray_AsString(py_ba);
    else if (PyBytes_Check(py_ba))
        return PyBytes_AsString(py_ba);
    else
        throw_ba_exception();

    return NULL;
}

int get_bytes_or_bytearray_ln(bp::object buf)
{
    PyObject* py_ba;
    py_ba = buf.ptr();
    if (PyByteArray_Check(py_ba))
        return PyByteArray_Size(py_ba);
    else if (PyBytes_Check(py_ba))
        return PyBytes_Size(py_ba);
    else
        throw_ba_exception();

    return 0;
}

bp::tuple read_wrap(RF24Network& ref, size_t maxlen)
{
    RF24NetworkHeader header;

    char* buf = new char[maxlen + 1];
    uint16_t len = ref.read(header, buf, maxlen);
    bp::object py_ba(bp::handle<>(PyByteArray_FromStringAndSize(buf, len)));
    delete[] buf;

    return bp::make_tuple(header, py_ba);
}

bp::tuple peek_read_wrap(RF24Network& ref, size_t maxlen)
{
    RF24NetworkHeader header;
    char* buf = new char[maxlen + 1];

    uint16_t len = rf24_min(maxlen, ref.peek(header));
    ref.peek(header, buf, len);
    bp::object py_ba(bp::handle<>(PyByteArray_FromStringAndSize(buf, len)));
    delete[] buf;

    return bp::make_tuple(header, py_ba);
}

bool write_wrap(RF24Network& ref, RF24NetworkHeader& header, bp::object buf)
{
    return ref.write(header, get_bytes_or_bytearray_str(buf), get_bytes_or_bytearray_ln(buf));
}

#if defined RF24NetworkMulticast
bool multicast_wrap(RF24Network& ref, RF24NetworkHeader& header, bp::object buf, uint8_t level)
{
    return ref.multicast(header, get_bytes_or_bytearray_str(buf), get_bytes_or_bytearray_ln(buf), level);
}
#endif // defined RF24NetworkMulticast

std::string toString_wrap(RF24NetworkHeader& ref)
{
    return std::string(ref.toString());
}

// **************** Overload wrappers ********************
void (RF24Network::*begin1)(uint8_t, uint16_t) = &RF24Network::begin;
void (RF24Network::*begin2)(uint16_t) = &RF24Network::begin;
uint16_t (RF24Network::*peek_header)(RF24NetworkHeader&) = &RF24Network::peek;

// **************** RF24Network exposed  *****************

BOOST_PYTHON_MODULE(RF24Network)
{
    //::RF24Network
    bp::class_<RF24Network>("RF24Network", bp::init<RF24&>((bp::arg("_radio"))))
        .def("available", &RF24Network::available)
        .def("begin", begin1, (bp::arg("_channel"), bp::arg("_node_address")))
        .def("begin", begin2, (bp::arg("_node_address")))
        .def("parent", &RF24Network::parent)
        .def("peek", peek_header, (bp::arg("header")))
        .def("peek", &peek_read_wrap, (bp::arg("maxlen") = MAX_PAYLOAD_SIZE))
        .def("read", &read_wrap, (bp::arg("maxlen") = MAX_PAYLOAD_SIZE))
        .def("update", &RF24Network::update)
        .def("write", &write_wrap, (bp::arg("header"), bp::arg("buf")))

#if defined RF24NetworkMulticast

        .def("multicastLevel", &RF24Network::multicastLevel, (bp::arg("level")))
        .def("multicast", &multicast_wrap, (bp::arg("header"), bp::arg("buf"), bp::arg("level") = 7))
        .def_readwrite("multicastRelay", &RF24Network::multicastRelay)
#endif // defined RF24NetworkMulticast

        .def("is_valid_address", &RF24Network::is_valid_address, (bp::arg("address")))
        .def_readwrite("txTimeout", &RF24Network::txTimeout)
        .def_readwrite("routeTimeout", &RF24Network::routeTimeout)
        .def_readwrite("networkFlags", &RF24Network::networkFlags)
        .add_property("parent", &RF24Network::parent);

    // **************** RF24NetworkHeader exposed  *****************

    bp::class_<RF24NetworkHeader>("RF24NetworkHeader", bp::init<>())
        .def(bp::init<uint16_t, bp::optional<unsigned char>>((bp::arg("_to"), bp::arg("_type") = (unsigned char)(0))))
        .def("toString", &toString_wrap)
        .def_readwrite("from_node", &RF24NetworkHeader::from_node)
        .def_readwrite("id", &RF24NetworkHeader::id)
        .def_readwrite("next_id", RF24NetworkHeader::next_id)
        .def_readwrite("reserved", &RF24NetworkHeader::reserved)
        .def_readwrite("to_node", &RF24NetworkHeader::to_node)
        .def_readwrite("type", &RF24NetworkHeader::type);
}
