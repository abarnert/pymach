PY=python
SUDO=sudo
build: mach.c
	$(PY) setup.py build_ext --inplace
install:
	$(SUDO) $(PY) setup.py install
clean:
	[ -f mach.so ] && $(SUDO) rm mach.so
	[ -d build ] && $(SUDO) rm -rf build