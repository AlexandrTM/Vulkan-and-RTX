#include "pch.h"
#include "VulkanQtWindow.h"

VulkanQtWindow::VulkanQtWindow(QVulkanInstance* instance, Character& character) {
    this->character = &character;
    setSurfaceType(QWindow::VulkanSurface);
    setVulkanInstance(instance);

    setCursor(Qt::BlankCursor);
}

void VulkanQtWindow::resizeEvent(QResizeEvent* event) {
    emit framebufferResized(event->size().width(), event->size().height());
    centerPos = { width() / 2, height() / 2 };
}

void VulkanQtWindow::keyPressEvent(QKeyEvent* event) {
    character->keys[event->key()] = true;
}

void VulkanQtWindow::keyReleaseEvent(QKeyEvent* event) {
    character->keys[event->key()] = false;
}

void VulkanQtWindow::mouseMoveEvent(QMouseEvent* event) {
    // On first frame, initialize lastMousePos to prevent jumps
    if (character->camera._firstMouse) {
        QCursor::setPos(mapToGlobal(centerPos));  // Warp to center
        character->camera._firstMouse = false;
        return;
    }

    double dx = event->position().x() - centerPos.x();
    double dy = event->position().y() - centerPos.y();

    character->camera.rotateRelative(dx, dy, character->mouseSensitivity);
    //character->camera.rotateAbsolute(event->position().x(), event->position().y(), character->mouseSensitivity);

    // Reset cursor position to center to avoid screen edge issues
    QCursor::setPos(mapToGlobal(centerPos));
}

void VulkanQtWindow::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton) {
        character->camera.setVerticalFov(60.0f);
        character->mouseSensitivity = 0.125;
    }
}

void VulkanQtWindow::wheelEvent(QWheelEvent* event) {
    float delta = event->angleDelta().y() / 120.0f; // Typical mouse wheel delta is 120
    character->camera.setVerticalFov(std::clamp(character->camera.getVerticalFov() - delta, 0.1f, 130.0f));
}

void VulkanQtWindow::focusInEvent(QFocusEvent* event) {
    QCursor::setPos(mapToGlobal(centerPos));
}