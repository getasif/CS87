.PHONY: default
default: build test clean

PYTHON = python
PYTHON_INCLUDE = ${shell ${PYTHON} -c 'from distutils import sysconfig; print( sysconfig.get_python_inc() )'}
MPI4PY_INCLUDE = ${shell ${PYTHON} -c 'import mpi4py; print( mpi4py.get_include() )'}
BOOST_FLAGS    = -lboost_python -lboost_python-mt

MPICXX = mpicxx
CXXFLAGS = -shared -fPIC
SO = ${shell ${PYTHON} -c 'import imp; print (imp.get_suffixes()[0][0])'}
.PHONY: build
build: helloworld${SO}
helloworld${SO}: helloworld.cxx
	${MPICXX} ${CXXFLAGS} -I${PYTHON_INCLUDE} -I${MPI4PY_INCLUDE} -I${BOOST_FLAGS} -o $@ $<


MPIEXEC = mpiexec
NP = -n 5
.PHONY: test
test: build
	${MPIEXEC} ${NP} ${PYTHON} test.py


.PHONY: clean
clean:
	${RM} helloworld${SO}
