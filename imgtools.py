# This file was automatically generated by SWIG (http://www.swig.org).
# Version 3.0.8
#
# Do not make changes to this file unless you know what you are doing--modify
# the SWIG interface file instead.





from sys import version_info
if version_info >= (2, 6, 0):
    def swig_import_helper():
        from os.path import dirname
        import imp
        fp = None
        try:
            fp, pathname, description = imp.find_module('_imgtools', [dirname(__file__)])
        except ImportError:
            import _imgtools
            return _imgtools
        if fp is not None:
            try:
                _mod = imp.load_module('_imgtools', fp, pathname, description)
            finally:
                fp.close()
            return _mod
    _imgtools = swig_import_helper()
    del swig_import_helper
else:
    import _imgtools
del version_info
try:
    _swig_property = property
except NameError:
    pass  # Python < 2.2 doesn't have 'property'.


def _swig_setattr_nondynamic(self, class_type, name, value, static=1):
    if (name == "thisown"):
        return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'SwigPyObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name, None)
    if method:
        return method(self, value)
    if (not static):
        if _newclass:
            object.__setattr__(self, name, value)
        else:
            self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)


def _swig_setattr(self, class_type, name, value):
    return _swig_setattr_nondynamic(self, class_type, name, value, 0)


def _swig_getattr_nondynamic(self, class_type, name, static=1):
    if (name == "thisown"):
        return self.this.own()
    method = class_type.__swig_getmethods__.get(name, None)
    if method:
        return method(self)
    if (not static):
        return object.__getattr__(self, name)
    else:
        raise AttributeError(name)

def _swig_getattr(self, class_type, name):
    return _swig_getattr_nondynamic(self, class_type, name, 0)


def _swig_repr(self):
    try:
        strthis = "proxy of " + self.this.__repr__()
    except Exception:
        strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

try:
    _object = object
    _newclass = 1
except AttributeError:
    class _object:
        pass
    _newclass = 0



def edge(filename, threshold, quant):
    return _imgtools.edge(filename, threshold, quant)
edge = _imgtools.edge

def compare(filename1, filename2, quant):
    return _imgtools.compare(filename1, filename2, quant)
compare = _imgtools.compare

def lumi_compare(filename1, filename2, quant):
    return _imgtools.lumi_compare(filename1, filename2, quant)
lumi_compare = _imgtools.lumi_compare

def compare_png(filename1, filename2, quant):
    return _imgtools.compare_png(filename1, filename2, quant)
compare_png = _imgtools.compare_png

def median_merge(argc, argv):
    return _imgtools.median_merge(argc, argv)
median_merge = _imgtools.median_merge

def sdev_merge(argc, argv):
    return _imgtools.sdev_merge(argc, argv)
sdev_merge = _imgtools.sdev_merge

def sdev_merge_png(argc, argv):
    return _imgtools.sdev_merge_png(argc, argv)
sdev_merge_png = _imgtools.sdev_merge_png

def sdev_merge_png_to_png(argc, argv):
    return _imgtools.sdev_merge_png_to_png(argc, argv)
sdev_merge_png_to_png = _imgtools.sdev_merge_png_to_png

def dark(filename, threshold, quant):
    return _imgtools.dark(filename, threshold, quant)
dark = _imgtools.dark

def mean_merge(argc, argv, faktor1, faktor2):
    return _imgtools.mean_merge(argc, argv, faktor1, faktor2)
mean_merge = _imgtools.mean_merge

def mean_merge_png(argc, argv, faktor1, faktor2):
    return _imgtools.mean_merge_png(argc, argv, faktor1, faktor2)
mean_merge_png = _imgtools.mean_merge_png

def lumi(filename, quant):
    return _imgtools.lumi(filename, quant)
lumi = _imgtools.lumi

def lumi_area(filename, quant):
    return _imgtools.lumi_area(filename, quant)
lumi_area = _imgtools.lumi_area
# This file is compatible with both classic and new-style classes.


