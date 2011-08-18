#include "stdafx.h"
#include "trie.h"
#include <Python.h>

typedef struct CharTrie{
    PyObject_HEAD
	Trie trie;
} CharTrie;

void chartrie_dealloc(PyObject *self){
	CharTrie* charTrie = (CharTrie*)self;
	trie_destroy(&charTrie->trie);
    PyMem_DEL(self);
}
  
int chartrie_len(PyObject *self){
	CharTrie* charTrie = (CharTrie*)self;
	return trie_size(&charTrie->trie);
}

int chartrie_getitem(PyObject *self, PyObject *key){
	CharTrie* charTrie = (CharTrie*)self;
}

Trie* trie = &charTrie->trie;
		Node* result_id = trie_add_word(trie->root, str);


PyMappingMethods counter_as_mapping = {
    chartrie_len,         /* inquiry mp_length;                 /* __len__ */
    chartrie_getitem,     /* binaryfunc mp_subscript            /* __getitem__ */
    0,     /* objobjargproc mp_ass_subscript;    /* __setitem__ */
};

static PyTypeObject CharTrieType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "chartrie.CharTrie",       /*tp_name*/
    sizeof(CharTrie),          /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    chartrie_dealloc,          /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    &counter_as_mapping,        /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "CharTrie object",         /* tp_doc */
};

static PyMethodDef noddy_methods[] = {
    {NULL}  /* Sentinel */
};

/* this is the Python constructor, as opposed to the C constructor */
/* this is a normal module function */
PyObject *chartrie_new(PyObject *self, PyObject *args){
	PyObject *object = NULL;
	long value = 0;  /* default initial value */
	if (PyArg_ParseTuple(args, "")){
		object = (PyObject*)PyObject_NEW(CharTrie, &CharTrieType);
	}
	return object;
}

//	int trie_add_word(Trie *trie, char* str, int value);
PyObject* chartrie_add_word(PyObject *self, PyObject *args){
	CharTrie* charTrie;
	char* str;
	if (!PyArg_ParseTuple(args, "os", &charTrie, &str)){
        return NULL;
	}else{
		Trie* trie = &charTrie->trie;
		Node* result_id = trie_add_word(trie->root, str);
		return Py_BuildValue("i", result_id);
	}
}

//	int trie_find_word(Trie *trie, char* str);
PyObject* chartrie_find_word(PyObject *self, PyObject *args){
	CharTrie* charTrie;
	char* str;
	if (!PyArg_ParseTuple(args, "os", &charTrie, &str)){
        return NULL;
	}else{
		Trie* trie = &charTrie->trie;
		int result_id = trie_find_word(trie->root, str);
		return Py_BuildValue("i", result_id);//    return Py_BuildValue("(iii)", best_size, a_offset, b_offset);
	}
}

PyObject* chartrie_size(PyObject *self, PyObject *args){
	CharTrie* charTrie;
	if (!PyArg_ParseTuple(args, "o", &charTrie)){
        return NULL;
	}else{
		Trie* trie = &charTrie->trie;
		int size = trie_size(trie);
		return Py_BuildValue("ll", size, sizeof(Node));
	}
}

PyObject* chartrie_print(PyObject *self, PyObject *args){
	CharTrie* charTrie;
	if (!PyArg_ParseTuple(args, "i", &charTrie)){
        return NULL;
	}else{
		Trie* trie = &charTrie->trie;
		trie_print(trie);
		return Py_None;
	}
}

//static PyObject * function_marker(PyObject *self, PyObject *args) {
//    return PyString_FromStringAndSize(MARKER, sizeof(MARKER)-1);
//}

static PyMethodDef ModuleMethods[] = {
    // longest_match is commented out because it's not necessary to expose it
    // at the Python level. To expose it, uncomment the following line.
/*    {"longest_match", function_longest_match, METH_VARARGS, "Given two strings, determines the longest common substring and returns a tuple of (best_size, a_offset, b_offset)."},*/
    {"trie_create", chartrie_new, METH_VARARGS, "Creates a new CharTrie object."},
    {"trie_find_word", chartrie_find_word, METH_VARARGS, "Finds a string and returns its ID, or its length to nearest match (~2)."},
    {"trie_add_word", chartrie_add_word, METH_VARARGS, "Adds a string and returns its ID."},
    {"trie_size", chartrie_size, METH_VARARGS, "Return trie size tuple."},
    {"trie_print", chartrie_print, METH_VARARGS, "Prints trie."},
    {NULL, NULL, 0, NULL}        // sentinel
};

PyMODINIT_FUNC init_chartrie(void) {
    (void) Py_InitModule("_chartrie", ModuleMethods);
}