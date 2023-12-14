CFLAGS = -std=c++17 -g #-O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

OBJ_DIR := ./objects
INCLUDE := ./include

SRC 		:= $(wildcard src/*.cpp)
OBJECTS		:= $(SRC:%.cpp=$(OBJ_DIR)/%.o)
V_SHADERS 	:= $(wildcard shaders/*.vert)
V_SPIV 		:= $(V_SHADERS:%.vert=$(OBJ_DIR)/%_vert.spv)
F_SHADERS 	:= $(wildcard shaders/*.frag)
F_SPIV 		:= $(F_SHADERS:%.frag=$(OBJ_DIR)/%_frag.spv)

VulkanPortal: $(OBJECTS) $(V_SPIV) $(F_SPIV) main.cpp
	g++ $(CFLAGS) $(OBJECTS) -o VulkanPortal.x86_64 main.cpp $(LDFLAGS)

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

test: VulkanPortal
	./VulkanPortal.x86_64

clean:
	rm -f VulkanPortal
	rm -r objects