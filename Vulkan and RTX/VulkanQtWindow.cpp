#include "pch.h"
#include "VulkanQtWindow.h"

VulkanQtWindow::VulkanQtWindow(QVulkanInstance* instance, Character& character) {
    this->character = &character;
    setSurfaceType(QWindow::VulkanSurface);
    setVulkanInstance(instance);
}

void VulkanQtWindow::resizeEvent(QResizeEvent* event) {
    emit framebufferResized(event->size().width(), event->size().height());
    centerPos = { event->size().width() / 2, event->size().height() / 2 };
    QCursor::setPos(mapToGlobal(centerPos));
    character->camera._isFirstMouse = true;
}

void VulkanQtWindow::keyPressEvent(QKeyEvent* event) {
    character->keys[event->key()] = true;
    //std::cout << "event->nativeScanCode(): " << event->nativeScanCode() << "\n";
}

void VulkanQtWindow::keyReleaseEvent(QKeyEvent* event) {
    character->keys[event->key()] = false;
}

void VulkanQtWindow::mouseMoveEvent(QMouseEvent* event) {
    if (!isActive()) {
        return;
    }

    // On first frame, initialize lastMousePos to prevent jumps
    if (character->camera._isFirstMouse) {
        QCursor::setPos(mapToGlobal(centerPos));
        character->camera._isFirstMouse = false;
        return;
    }

    double dx = event->position().x() - centerPos.x();
    double dy = centerPos.y() - event->position().y();

    character->camera.addRotationDelta(dx, dy, character->mouseSensitivity);

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
    setCursor(Qt::BlankCursor);
}

void VulkanQtWindow::focusOutEvent(QFocusEvent* event) {
    setCursor(Qt::ArrowCursor);
}
