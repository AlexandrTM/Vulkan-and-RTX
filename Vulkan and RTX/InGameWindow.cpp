#include "pch.h"
#include "InGameWindow.h"
InGameWindow::InGameWindow(
    QVulkanInstance* instance, 
    Character& character, GameContext& gameContext
) {
    this->character = &character;
    this->gameContext = &gameContext;
    setSurfaceType(QWindow::VulkanSurface);
    setVulkanInstance(instance);
}

void InGameWindow::resizeEvent(QResizeEvent* event) {
    emit framebufferResized(event->size().width(), event->size().height());
    centerPos = { event->size().width() / 2, event->size().height() / 2 };
    centerPos = mapToGlobal(centerPos);
    //std::cout << "center pos: " << centerPos.x() << " " << centerPos.y() << "\n";
    if (isActive()) {
        QCursor::setPos(centerPos);
        //std::cout << "set mouse pos 1: " << centerPos.x() << " " << centerPos.y() << "\n";
    }
    character->camera._isFirstMouse = true;
}

void InGameWindow::closeEvent(QCloseEvent* event) {
    emit windowClosed();
    QWindow::closeEvent(event);
}

void InGameWindow::keyPressEvent(QKeyEvent* event) {
    gameContext->keyboardKeys[event->key()] = true;
}

void InGameWindow::keyReleaseEvent(QKeyEvent* event) {
    gameContext->keyboardKeys[event->key()] = false;
}

void InGameWindow::mouseMoveEvent(QMouseEvent* event) {
    if (!isActive()) {
        return;
    }

    if (character->camera._isFirstMouse) {
        QCursor::setPos(centerPos);
        //std::cout << "set mouse pos 2: " << centerPos.x() << " " << centerPos.y() << "\n";
        //character->camera._isFirstMouse = false;
        return;
    }

    latestMouseDx = (event->globalPosition().x() - centerPos.x()) * character->mouseSensitivity;
    latestMouseDy = (centerPos.y() - event->globalPosition().y()) * character->mouseSensitivity;
}

void InGameWindow::mousePressEvent(QMouseEvent* event) {
    gameContext->mouseKeys[event->button()] = true;
}

void InGameWindow::mouseReleaseEvent(QMouseEvent* event) {
    gameContext->mouseKeys[event->button()] = false;
}

void InGameWindow::wheelEvent(QWheelEvent* event) {
    float delta = event->angleDelta().y() / 120.0f; // Typical mouse wheel delta is 120
    character->camera.setVerticalFov(std::clamp(character->camera.getVerticalFov() - delta, 0.1f, 130.0f));
}

void InGameWindow::focusInEvent(QFocusEvent* event) {
    setCursor(Qt::BlankCursor);
    character->camera._isFirstMouse = true;
    //QCursor::setPos(centerPos);
    //std::cout << "set mouse pos 3: " << centerPos.x() << " " << centerPos.y() << "\n";
}

void InGameWindow::focusOutEvent(QFocusEvent* event) {
    setCursor(Qt::ArrowCursor);
}
