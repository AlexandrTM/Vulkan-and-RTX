#ifndef PCH_H
#define PCH_H

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/hash.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <stb_image_write.h>
#include <stb_image.h>
#include <tiny_gltf.h>
#include <tiny_obj_loader.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <map>
#include <optional>
#include <set>
#include <cstdint>
#include <algorithm>
#include <fstream>
#include <bitset>
#include <array>
#include <unordered_map>
#include <random>

#endif