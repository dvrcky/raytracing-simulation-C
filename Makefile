CFLAGS = -lm -lSDL2
CC = clang

raytracing: raytracing.c
	$(CC) $(CFLAGS) raytracing.c -o raytracing
