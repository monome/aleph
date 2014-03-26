#include "Python.h"
#include <stdlib.h>

static int knobs[4];
static PyObject * knob_tweaked(PyObject *self, PyObject *args) {
    int knob_index, value;
    if (!PyArg_ParseTuple(args, "ii", &knob_index, &value))
        return NULL;
    //fprintf (stderr, "knob %d tweaked by %d and C function called\n",knob_index, value);
    knobs[knob_index]+=value;
    fprintf (stderr, "Knob %d now has value %d\n",knob_index, knobs[knob_index]);
    return Py_None;
}
static PyObject* pyalephModule;
static int play_led = 0;
static PyObject * button_pressed(PyObject *self, PyObject *args) {
    int button;
    if (!PyArg_ParseTuple(args, "i", &button))
        return NULL;
    fprintf (stderr, "button %d pressed and C function called\n",button);
    if(button == 4) {
        PyObject *pFunc, *pArgs, *led_val;
        play_led ^=1;
        pFunc = PyObject_GetAttrString(pyalephModule, "set_play_led");
        led_val = PyString_FromFormat("%d",play_led);
        pArgs = PyTuple_Pack(1, led_val);
        PyObject_CallObject(pFunc, pArgs);
    }
    return Py_None;
}



static PyMethodDef PyAlephCMethods[] = {
    {"button",  button_pressed, METH_VARARGS, "Execute a shell command."},
    {"knob",  knob_tweaked, METH_VARARGS, "Execute a shell command."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

int python_gui (void) {

    //Fire up the python interpreter, including path to pyaleph.py
    setenv("PYTHONPATH", "pyaleph", 0);
    Py_Initialize();

    //Make the methods listed in PyAlephCMethods available as a module called pyaleph_c
    static int numargs=1;
    Py_InitModule("pyaleph_c", PyAlephCMethods);
    PyObject *pName, *pFunc, *pArgs, *pValue;

    //Load pyaleph/pyaleph.py
    pName = PyString_FromString("pyaleph");
    pyalephModule = PyImport_Import(pName);
    Py_DECREF(pName);

    //Call pyleph.start_gui()
    pFunc = PyObject_GetAttrString(pyalephModule, "start_gui");
    pArgs = PyTuple_New(0);
    PyObject_CallObject(pFunc, pArgs);
    return 0;
}
