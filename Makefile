CFLAGS = -std=c++17 #-O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

OBJ_DIR := ./objects
INCLUDE := ./include

SRC 		:= $(wildcard src/*.cpp)
OBJECTS		:= $(SRC:%.cpp=$(OBJ_DIR)/%.o)
V_SHADERS 	:= $(wildcard shaders/*.vert)
V_SPIV 		:= $(V_SHADERS:%.vert=$(OBJ_DIR)/%_vert.spv)
F_SHADERS 	:= $(wildcard shaders/*.frag)
F_SPIV 		:= $(F_SHADERS:%.frag=$(OBJ_DIR)/%_frag.spv)

HelloTriangle: $(OBJECTS) $(V_SPIV) $(F_SPIV) main.cpp
	g++ $(CFLAGS) $(OBJECTS) -o HelloTriangle.x86_64 main.cpp $(LDFLAGS)

$(OBJ_DIR)/%_vert.spv: %.vert
	@mkdir -p $(@D)
	glslc $< -O -o $@

$(OBJ_DIR)/%_frag.spv: %.frag
	@mkdir -p $(@D)
	glslc $< -O -o $@

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	g++ -I$(INCLUDE) -c $< -o $@ -DSFML_STATIC -static

.PHONY: test clean

test: HelloTriangle
	./HelloTriangle.x86_64

clean:
	rm -f HelloTriangle
	rm -r objects