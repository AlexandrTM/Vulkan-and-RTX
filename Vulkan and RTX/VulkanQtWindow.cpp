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
    centerPos = mapToGlobal(centerPos);
    QCursor::setPos(centerPos);
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

    if (character->camera._isFirstMouse) {
        QCursor::setPos(centerPos);
        character->camera._isFirstMouse = false;
        return;
    }

    latestMouseDx = (event->globalPosition().x() - centerPos.x()) * character->mouseSensitivity;
    latestMouseDy = (centerPos.y() - event->globalPosition().y()) * character->mouseSensitivity;
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
    QCursor::setPos(centerPos);
    setCursor(Qt::BlankCursor);
}

void VulkanQtWindow::focusOutEvent(QFocusEvent* event) {
    setCursor(Qt::ArrowCursor);
}
