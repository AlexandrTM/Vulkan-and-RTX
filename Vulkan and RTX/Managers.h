#pragma once

#include "pch.h"
#include "VulkanInitializer.h"
#include "BufferManager.h"
#include "ImageManager.h"
#include "ModelManager.h"

struct Managers
{
    VulkanInitializer vkInit;
    BufferManager bufferManager{ vkInit };
    ImageManager imageManager{ vkInit, bufferManager };
    ModelManager modelManager{ vkInit, bufferManager, imageManager };
};