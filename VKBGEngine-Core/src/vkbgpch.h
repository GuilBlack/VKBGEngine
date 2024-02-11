#pragma once

// Standard Library
#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <stdexcept>

// Data Structures
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <array>

// File System
#include <filesystem>
#include <fstream>

// VKBGEngine Libraries

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#if defined(DEBUG) || defined(_DEBUG)
    #define LOG(x) std::cout << x
#else
    #define LOG(x)
#endif
