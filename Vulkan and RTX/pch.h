#ifndef PCH_H
#define PCH_H

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

#include <QApplication>
#include <QGuiApplication>
#include <QPushButton>
#include <QWidget>
#include <QtQml/QtQml>
#include <QtWidgets/QStackedLayout>
#include <QtQuickWidgets/QQuickWidget>
#include <QtQuickControls2/QtQuickControls2>
#include <QStackedWidget>
#include <QWindow>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QVulkanWindow>
#include <QVulkanInstance>
#include <QVulkanFunctions>

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/hash.hpp>
#include <gtx/string_cast.hpp>

#include "rtx_core.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
//#include <meshoptimizer.h> // need release version probably

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <stb_image_write.h>
#include <stb_image.h>
#include <tiny_gltf.h>
#include <tiny_obj_loader.h>

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
#include <numeric>
#include <sstream>
#include <filesystem>

#endif