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
    character->keyboardKeys[event->key()] = true;
    //std::cout << "event->nativeScanCode(): " << event->nativeScanCode() << "\n";
}

void VulkanQtWindow::keyReleaseEvent(QKeyEvent* event) {
    character->keyboardKeys[event->key()] = false;
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

    double dx = (event->position().x() - centerPos.x()) * character->mouseSensitivity;
    double dy = (centerPos.y() - event->position().y()) * character->mouseSensitivity;

    //std::cout << "dx: " << dx << " " << "dy: " << dy << "\n";

    character->camera.addRotationDelta(dx, dy);
}

void VulkanQtWindow::mousePressEvent(QMouseEvent* event) {
    character->mouseKeys[event->button()] = true;
}

void VulkanQtWindow::mouseReleaseEvent(QMouseEvent* event) {
    character->mouseKeys[event->button()] = false;
}

void VulkanQtWindow::wheelEvent(QWheelEvent* event) {
    float delta = event->angleDelta().y() / 120.0f; // Typical mouse wheel delta is 120
    character->camera.setVerticalFov(std::clamp(character->camera.getVerticalFov() - delta, 0.1f, 130.0f));
}

void VulkanQtWindow::focusInEvent(QFocusEvent* event) {
    character->camera._isFirstMouse = true;
    setCursor(Qt::BlankCursor);
}

void VulkanQtWindow::focusOutEvent(QFocusEvent* event) {
    setCursor(Qt::ArrowCursor);
}
