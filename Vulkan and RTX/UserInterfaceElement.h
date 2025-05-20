#pragma once

#include "ui_core.h"
#include "Model.h"
#include "UserInterfaceRenderer.h"

struct UserInterfaceElement
{
    std::unique_ptr<UserInterfaceRenderer> renderer;
    std::unique_ptr<UiCache> cache;
    Texture texture;
    Model model;
    QString qmlPath;
};
