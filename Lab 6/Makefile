PETSC_DIR=/usr/local/petsc

include ${PETSC_DIR}/conf/variables
include ${PETSC_DIR}/conf/rules
include ${PETSC_DIR}/conf/test

CLINKER=g++

krylov : krylov.o chkopts
	${CLINKER} -w -o krylov krylov.o ${PETSC_LIB}
	${RM} krylov.o	
	./krylov
