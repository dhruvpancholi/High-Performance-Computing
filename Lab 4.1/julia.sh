gcc sjulia.c -lX11
for (( i = 100; i <= 2000; i=i+100 )); do
	./a.out $i
done