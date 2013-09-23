SOURCES= \
	Sources/collider.c \
	Sources/collidergjk.c \
	Sources/matrix.c \
	Sources/mem.c \
	Sources/object.c \
	Sources/smartArray.c \
	Sources/transform.c \
	Sources/vector.c \
	Sources/quadtree.c \
	Sources/world.c

OBJECTS = $(SOURCES:Sources/%.c=Objects/%.o)

all: Objects $(OBJECTS)
	ar -rcs libtphysic.a $(OBJECTS)

Objects/%.o: Sources/%.c
	gcc -c -O3 -I Include/TPhysic Sources/$*.c -o Objects/$*.o

Objects:
	mkdir Objects

clean:
	rm $(OBJECTS)

fclean: clean
	rm libtphysic.a

re: fclean all
