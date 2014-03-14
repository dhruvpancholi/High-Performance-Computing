mpicc -o 32 32.c -lm -lmpi -lmpe -llmpe -std=c99
# mpirun -np $i ./ori_integral


for i in 1 2 4 8 16
do
	mpirun -np $i ./32 MonteCarlo100000000$i > 32out100000000$i.txt
	# clog2TOslog2 adapQuad$i.slog2 adapQuad$1.clog2
	#jumpshot integral$i.slog2
done
