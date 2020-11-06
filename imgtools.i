%module imgtools
int edge(char *filename, int threshold, int quant);
long compare(char *filename1, char *filename2, int quant);
long lumi_compare(char *filename1, char *filename2, int quant);
long compare_png(char *filename1, char *filename2, int quant);

%typemap(in) char ** {
  /* Check if is a list */
  if (PyList_Check($input)) {
    int size = PyList_Size($input);
    int i = 0;
    $1 = (char **) malloc((size+1)*sizeof(char *));
    for (i = 0; i < size; i++) {
      PyObject *o = PyList_GetItem($input,i);
      if (PyString_Check(o))
	$1[i] = PyString_AsString(PyList_GetItem($input,i));
      else {
	PyErr_SetString(PyExc_TypeError,"list must contain strings");
	free($1);
	return NULL;
      }
    }
    $1[i] = 0;
  } else {
    PyErr_SetString(PyExc_TypeError,"not a list");
    return NULL;
  }
}

%typemap(freearg) char ** {
  free((char *) $1);
}

int median_merge(int argc, char **argv);
int sdev_merge(int argc, char **argv);
int sdev_merge_png(int argc, char **argv);
int sdev_merge_png_to_png(int argc, char **argv);
int dark(char *filename, int threshold, int quant);
int mean_merge(int argc, char **argv, int faktor1, int faktor2);
int mean_merge_png(int argc, char **argv, int faktor1, int faktor2);
int lumi(char *filename,int quant);
long lumi_area(char *filename,int quant);
