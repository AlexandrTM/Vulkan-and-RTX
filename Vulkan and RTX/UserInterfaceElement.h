#pragma once

#include "Model.h"
#include "UserInterfaceRenderer.h"

struct UserInterfaceElement
{
    std::unique_ptr<UserInterfaceRenderer> renderer;
    Texture texture;
    Model model;
    QString qmlPath;
};
