#!/bin/sh
#$ -S /bin/sh
#$ -cwd
#$ -V
#$ -N julia
#$ -pe mpich 5
myjob=pjulianew
mpirun -np $NSLOTS -machinefile $TMPDIR/machines $myjob
