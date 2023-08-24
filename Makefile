MAGMA_SRC_DIRS = magma/core/*.c \
				 magma/platform/*.c \
				 magma/math/*.c \
				 magma/rendering/*.c \
#				 magma/rendering/opengl/*.c

MAGMA_INCLUDE_FLAGS = -Imagma -Imagma/vendor
MAGMA_LIB_FLAGS = -lgdi32 -lglu32 -lopengl32

all:
	gcc -shared -o bin/libmagma.dll $(MAGMA_SRC_DIRS) $(MAGMA_INCLUDE_FLAGS) $(MAGMA_LIB_FLAGS)
	gcc -o bin/a.exe *.c -Imagma -Lbin -llibmagma