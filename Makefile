MAGMA_SRC_DIRS = magma/core/*.c \
				 magma/platform/*.c \
				 magma/math/*.c \
				 magma/rendering/*.c \
				 magma/rendering/vulkan/*.c \
				 magma/rendering/vulkan/platform/*.c \
#				 magma/rendering/opengl/*.c

MAGMA_INCLUDE_FLAGS = -Imagma -Imagma/vendor
MAGMA_LIB_FLAGS = -lgdi32 -lglu32 -lopengl32
MAGMA_DEFINES = -D=VK_USE_PLATFORM_WIN32_KHR

all:
	gcc -shared -o bin/libmagma.dll $(MAGMA_SRC_DIRS) $(MAGMA_INCLUDE_FLAGS) $(MAGMA_LIB_FLAGS) $(MAGMA_DEFINES)
	gcc -o bin/a.exe *.c -Imagma -Lbin -llibmagma $(MAGMA_INCLUDE_FLAGS)