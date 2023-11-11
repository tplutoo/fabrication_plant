CC=gcc
CFLAGS=-lpthread

all: main assembly_manager

main: fabrication_plant_manager.c
	$(CC) -o main fabrication_plant_manager.c $(CFLAGS)

assembly_manager: assembly_manager.c
	$(CC) -o assembly_manager assembly_manager.c $(CFLAGS)

clean:
	rm -f main assembly_manager
