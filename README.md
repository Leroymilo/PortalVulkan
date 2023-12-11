# Portal in C++ Vulkan

This is hell

This is based on this [Vulkan tutorial](https://vulkan-tutorial.com/), the textures are from [here](https://gamebanana.com/mods/383184).

## Setup

To run this project, you need :
- gcc
- make
- Vulkan SDK (follow [this tutorial](https://vulkan.lunarg.com/doc/sdk/1.3.268.0/linux/getting_started.html))
- Some libraries (these are Fedora dnf names, I think some distros have `-dev` instead of `-devel`):
	- `vulkan-tools`
	- `vulkan-loader-devel`
	- `vulkan-validation-layers-devel`
	- `glfw-devel`
	- `glm-devel`
	- `glslc`
	- `libXi-devel`
	- `libXxf86vm-devel`

Once everything is installed, you should be able to just `make test` to run it if your GPU is supported (very low requirements from what I now).