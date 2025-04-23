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

void InGameWindow::keyPressEvent(QKeyEvent* event) {
    gameContext->keyboardKeys[event->key()] = true;
}

void InGameWindow::keyReleaseEvent(QKeyEvent* event) {
    gameContext->keyboardKeys[event->key()] = false;
}

void InGameWindow::mousePressEvent(QMouseEvent* event) {
    gameContext->mouseKeys[event->button()] = true;
}

void InGameWindow::mouseReleaseEvent(QMouseEvent* event) {
    gameContext->mouseKeys[event->button()] = false;
}

void InGameWindow::resizeEvent(QResizeEvent* event) {
    emit framebufferResized(event->size().width(), event->size().height());
    //std::cout << "in game window resized: " << event->size().width() << " " <<  event->size().height() << "\n";
    gameContext->windowCenterPos = { event->size().width() / 2, event->size().height() / 2 };
    gameContext->windowCenterPos = mapToGlobal(gameContext->windowCenterPos);
    if (isActive() and (gameContext->currentGameState == GameState::IN_GAME_TESTING)) {
        QCursor::setPos(gameContext->windowCenterPos);
        //std::cout << "center pos: " << gameContext->windowCenterPos.x() << " " << gameContext->windowCenterPos.y() << "\n";
    }
    character->camera._isFirstMouse = true;
}

void InGameWindow::moveEvent(QMoveEvent* event) {
    gameContext->windowCenterPos = { width() / 2, height() / 2 };
    gameContext->windowCenterPos = mapToGlobal(gameContext->windowCenterPos);
}

void InGameWindow::closeEvent(QCloseEvent* event) {
    emit windowClosed();
    QWindow::closeEvent(event);
}

void InGameWindow::mouseMoveEvent(QMouseEvent* event) {
    if (!(isActive() and (gameContext->currentGameState == GameState::IN_GAME_TESTING))) {
        return;
    }

    if (character->camera._isFirstMouse) {
        QCursor::setPos(gameContext->windowCenterPos);
        //std::cout << "set mouse pos 2: " << windowCenterPos.x() << " " << windowCenterPos.y() << "\n";
        return;
    }

    latestMouseDx = (event->globalPosition().x() - gameContext->windowCenterPos.x()) * character->mouseSensitivity;
    latestMouseDy = (gameContext->windowCenterPos.y() - event->globalPosition().y()) * character->mouseSensitivity;
}

void InGameWindow::wheelEvent(QWheelEvent* event) {
    if (!(gameContext->currentGameState == GameState::IN_GAME_TESTING)) {
        return;
    }

    float delta = event->angleDelta().y() / 120.0f; // Typical mouse wheel delta is 120
    character->camera.setVerticalFov(std::clamp(character->camera.getVerticalFov() - delta, 0.1f, 130.0f));
}

void InGameWindow::focusInEvent(QFocusEvent* event) {
    if (gameContext->currentGameState == GameState::IN_GAME_TESTING) {
        setCursor(Qt::BlankCursor);
    }
    character->camera._isFirstMouse = true;
    //std::cout << "set mouse pos 3: " << windowCenterPos.x() << " " << windowCenterPos.y() << "\n";
}

void InGameWindow::focusOutEvent(QFocusEvent* event) {
    setCursor(Qt::ArrowCursor);
}
