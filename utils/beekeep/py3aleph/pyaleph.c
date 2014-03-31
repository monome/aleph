#include "Python.h"
#include <stdlib.h>

PyObject* PyInit_pyalephC(void); /* Forward */

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
        //led_val = Py_BuildValue("i",play_led);
        //pArgs = PyTuple_Pack(1, led_val);
        //PyObject_CallObject(pFunc, pArgs);
        char buffer[50];
        sprintf(buffer,"pyaleph.set_play_led(%d)\n",play_led);
        //printf("set play string = %s", buffer);
	    PyRun_SimpleString(buffer);

    }
    return Py_None;
}

static PyMethodDef pyalephC_methods[] = {
    {"button",  button_pressed, METH_VARARGS, "Execute a shell command."},
    {"knob",  knob_tweaked, METH_VARARGS, "Execute a shell command."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef pyalephCmodule = {
	{}, /* m_base */
	"pyalephC",  /* m_name */
	0,  /* m_doc */
	0,  /* m_size */
	pyalephC_methods,  /* m_methods */
	0,  /* m_reload */
	0,  /* m_traverse */
	0,  /* m_clear */
	0,  /* m_free */
};


int python_gui (void) {

    //Fire up the python interpreter, including path to pyaleph.py
    setenv("PYTHONPATH", "py3aleph", 0);
	PyImport_AppendInittab("pyalephC", PyInit_pyalephC);
    Py_Initialize();

	//PyRun_SimpleString("import pyalephC\n");
	//PyRun_SimpleString("print(pyalephC.foo())\n");
    //Make the methods listed in PyAlephCMethods available as a module called pyaleph_c
    static int numargs=1;
    //Py_InitModule("pyaleph_c", PyAlephCMethods);
    PyObject *pName, *pFunc, *pArgs, *pValue;

    //Load pyaleph/pyaleph.py
	PyRun_SimpleString("import pyaleph\n");

    //Call pyleph.start_gui()
	PyRun_SimpleString("pyaleph.start_gui()\n");
    return 0;
}
PyObject* PyInit_pyalephC(void)
{
	return PyModule_Create(&pyalephCmodule);
}
