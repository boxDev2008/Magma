MAGMA_SRC_DIRS = magma/core/*.c \
				 magma/platform/*.c \
				 magma/math/*.c \
				 magma/rendering/*.c \
				 magma/rendering/vulkan/*.c \
				 magma/rendering/vulkan/platform/*.c \
				 magma/graphics/*.c \
				 magma/physics/2d/*.c \
#				 magma/rendering/opengl/*.c \
#				 magma/rendering/opengl/platform/*.c

MAGMA_INCLUDE_FLAGS = -Imagma -Imagma/vendor
MAGMA_LIB_FLAGS = -lgdi32 -lglu32 -lopengl32
MAGMA_DEFINES = -D=VK_USE_PLATFORM_WIN32_KHR

all:
	gcc -w -shared -o bin/libmagma.dll $(MAGMA_SRC_DIRS) $(MAGMA_INCLUDE_FLAGS) $(MAGMA_LIB_FLAGS) $(MAGMA_DEFINES)
	gcc -w -o bin/a.exe *.c -Imagma -Lbin -llibmagma $(MAGMA_INCLUDE_FLAGS)