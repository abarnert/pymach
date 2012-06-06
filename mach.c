#include <Python.h>
#include <sys/types.h>
#include <mach/mach_init.h>
#include <mach/mach_traps.h>
#include <mach/mach_types.h>
#include <mach/mach_vm.h>

static PyObject *MachError;

static PyObject *mach_error(kern_return_t ret) {
  return PyErr_Format(MachError, "kernel return code %d", (int)ret);
}

static PyObject *pymach_task_self(PyObject *self, PyObject *args) {
  return Py_BuildValue("i", mach_task_self());
}

static PyObject *pymach_task_for_pid(PyObject *self, PyObject *args) {
  int pid;
  task_t task;
  kern_return_t ret;
  if (!PyArg_ParseTuple(args, "i", &pid)) return NULL;
  ret = task_for_pid(mach_task_self(), pid, &task);
  if (ret) return mach_error(ret);
  return Py_BuildValue("i", task);
}

static PyObject *pymach_vm_protect(PyObject *self, PyObject *args) {
  task_t task;
  mach_vm_address_t address;
  mach_vm_size_t size;
  int prot;
  kern_return_t ret;
  if (!PyArg_ParseTuple(args, "iKLi", &task, &address, &size,
			&prot)) return NULL;
  ret = mach_vm_protect(task, address, size, 0, prot);
  if (ret) return mach_error(ret);
  Py_RETURN_NONE;
}

static PyObject *pymach_vm_read(PyObject *self, PyObject *args) {
  task_t task;
  mach_vm_address_t address;
  mach_vm_size_t size;
  vm_offset_t data;
  mach_msg_type_number_t dataCnt;
  kern_return_t ret;
  if (!PyArg_ParseTuple(args, "iKL", &task, &address, &size)) return NULL;
  ret = mach_vm_read(task, address, size, &data, &dataCnt);
  if (ret) return mach_error(ret);
  PyObject *val = Py_BuildValue("s#", data, dataCnt);
  mach_vm_deallocate(mach_task_self(), data, dataCnt);
  return val;
}

static PyObject *pymach_vm_write(PyObject *self, PyObject *args) {
  task_t task;
  mach_vm_address_t address;
  char *buf;
  int len;
  kern_return_t ret;
  if (!PyArg_ParseTuple(args, "iKt#", &task, &address, 
			&buf, &len)) return NULL;
  ret = mach_vm_write(task, address, (vm_offset_t)buf, len);
  if (ret) return mach_error(ret);
  Py_RETURN_NONE;
}

static PyObject *pymach_vm_region_recurse(PyObject *self, PyObject *args) {
  mach_port_name_t task;
  vm_map_offset_t vmoffset;
  vm_map_size_t vmsize;
  uint32_t nesting_depth = 0;
  struct vm_region_submap_info_64 vbr;
  mach_msg_type_number_t vbrcount = 16;
  kern_return_t ret;

  if (!PyArg_ParseTuple(args, "i", &task)) return NULL;

  ret = mach_vm_region_recurse( task, 
                               &vmoffset, 
                               &vmsize, 
                               &nesting_depth, 
                               (vm_region_recurse_info_t)&vbr, 
                               &vbrcount
                             );
  if (ret) mach_error(ret);
  PyObject *val = Py_BuildValue("KKIii", vmoffset, vmsize, nesting_depth, vbr, vbrcount);
  return val;

}

static PyMethodDef MachMethods[] = {
  {"task_self", pymach_task_self, METH_VARARGS,
   "Get a Mach port for the current task"},
  {"task_for_pid", pymach_task_for_pid, METH_VARARGS,
   "Get a Mach port for the task corresponding to a pid"},
  {"vm_protect", pymach_vm_protect, METH_VARARGS,
   "Change memory protection in another task"},
  {"vm_read", pymach_vm_read, METH_VARARGS,
   "Read memory from another task"},
  {"vm_write", pymach_vm_write, METH_VARARGS,
   "Write memory to another task"},
  {"vm_region_recurse", pymach_vm_region_recurse, METH_VARARGS,
   "Get info about memory region"},
  {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initmach(void) {
  PyObject *m = Py_InitModule("mach", MachMethods);
  if (!m) return;
  MachError = PyErr_NewException("mach.MachError", NULL, NULL);
  Py_INCREF(MachError);
  PyModule_AddObject(m, "MachError", MachError);
  PyModule_AddObject(m, "VM_PROT_NONE", PyInt_FromLong(VM_PROT_NONE));
  PyModule_AddObject(m, "VM_PROT_READ", PyInt_FromLong(VM_PROT_READ));
  PyModule_AddObject(m, "VM_PROT_WRITE", PyInt_FromLong(VM_PROT_WRITE));
  PyModule_AddObject(m, "VM_PROT_EXECUTE", PyInt_FromLong(VM_PROT_EXECUTE));
  PyModule_AddObject(m, "VM_PROT_DEFAULT", PyInt_FromLong(VM_PROT_DEFAULT));
  PyModule_AddObject(m, "VM_PROT_ALL", PyInt_FromLong(VM_PROT_ALL));
}
