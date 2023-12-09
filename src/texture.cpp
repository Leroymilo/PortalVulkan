#include "../include/libs/stb_image.h"

#include "../include/texture.hpp"

void Texture::cleanup(VkDevice &device) {
    vkDestroySampler(device, texture_sampler, nullptr);
    vkDestroyImageView(device, texture_image_view, nullptr);

    vkDestroyImage(device, texture_image, nullptr);
    vkFreeMemory(device, texture_image_memory, nullptr);
}