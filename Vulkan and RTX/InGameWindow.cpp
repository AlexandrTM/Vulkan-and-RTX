#include "pch.h"
#include "InGameWindow.h"

InGameWindow::InGameWindow(
    QVulkanInstance* instance, 
    Character& character
) {
    this->character = &character;
    setSurfaceType(QWindow::VulkanSurface);
    setVulkanInstance(instance);
    this->installEventFilter(this);
}

bool InGameWindow::eventFilter(QObject* obj, QEvent* event) {
    if (sendEventToUI(event)) {
        return true;
    }
    // For all other cases, let normal event processing happen
    return QWindow::eventFilter(obj, event);
}

bool InGameWindow::sendEventToUI(QEvent* event) {
    const QEvent::Type eventType = event->type();

    if (mainMenuRenderer && gameContext.currentGameState == GameState::MAIN_MENU) {
        switch (eventType) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
        case QEvent::HoverMove:
            mainMenuRenderer->forwardEvent(event);
            return true;
        default:
            break;
        }
    }

    if (settingsMenuRenderer && gameContext.currentGameState == GameState::SETTINGS_MENU) {
        switch (eventType) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
        case QEvent::HoverMove:
            settingsMenuRenderer->forwardEvent(event);
            return true;
        default:
            break;
        }
    }

    if (pauseMenuRenderer && gameContext.currentGameState == GameState::PAUSED) {
        switch (eventType) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
        case QEvent::HoverMove:
            pauseMenuRenderer->forwardEvent(event);
            return true;
        default:
            break;
        }
    }

    if (selectEquationRenderer && gameContext.currentGameState == GameState::COMBAT_PLAYER_SELECT_EQUATION) {
        switch (eventType) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
        case QEvent::HoverMove:
            selectEquationRenderer->forwardEvent(event);
            return true;
        default:
            break;
        }
    }

    if (solveEquationRenderer && gameContext.currentGameState == GameState::COMBAT_PLAYER_SOLVE_EQUATION) {
        switch (eventType) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
        case QEvent::HoverMove:
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
        case QEvent::InputMethod:
            solveEquationRenderer->forwardEvent(event);
            return true;
        default:
            break;
        }
    }

    return false;
}

void InGameWindow::keyPressEvent(QKeyEvent* event) {
    gameContext.keyboardKeys[event->key()] = true;
}

void InGameWindow::keyReleaseEvent(QKeyEvent* event) {
    gameContext.keyboardKeys[event->key()] = false;
}

void InGameWindow::mousePressEvent(QMouseEvent* event) {
    gameContext.mouseKeys[event->button()] = true;
}

void InGameWindow::mouseReleaseEvent(QMouseEvent* event) {
    gameContext.mouseKeys[event->button()] = false;
}

void InGameWindow::resizeEvent(QResizeEvent* event) {
    emit resized(event->size().width(), event->size().height());
    //std::cout << "in game window resized: " << event->size().width() << " " <<  event->size().height() << "\n";
    gameContext.windowCenterPos = { event->size().width() / 2, event->size().height() / 2 };
    gameContext.windowCenterPos = mapToGlobal(gameContext.windowCenterPos);
    if (isActive() and (gameContext.currentGameState == GameState::IN_GAME_TESTING)) {
        QCursor::setPos(gameContext.windowCenterPos);
        //std::cout << "center pos: " << gameContext.windowCenterPos.x() << " " << gameContext.windowCenterPos.y() << "\n";
    }
    character->camera._isFirstMouse = true;
}

void InGameWindow::moveEvent(QMoveEvent* event) {
    emit moved(event->pos().x(), event->pos().y());
    gameContext.windowCenterPos = { width() / 2, height() / 2 };
    gameContext.windowCenterPos = mapToGlobal(gameContext.windowCenterPos);
}

void InGameWindow::closeEvent(QCloseEvent* event) {
    emit closed();
    QWindow::closeEvent(event);
}

void InGameWindow::mouseMoveEvent(QMouseEvent* event) {
    if (!(isActive() and (gameContext.currentGameState == GameState::IN_GAME_TESTING))) {
        return;
    }

    if (character->camera._isFirstMouse) {
        QCursor::setPos(gameContext.windowCenterPos);
        //std::cout << "set mouse pos 2: " << windowCenterPos.x() << " " << windowCenterPos.y() << "\n";
        return;
    }

    latestMouseDx = (event->globalPosition().x() - gameContext.windowCenterPos.x()) * character->mouseSensitivity;
    latestMouseDy = (gameContext.windowCenterPos.y() - event->globalPosition().y()) * character->mouseSensitivity;
}

void InGameWindow::wheelEvent(QWheelEvent* event) {
    if (!(gameContext.currentGameState == GameState::IN_GAME_TESTING)) {
        return;
    }

    float delta = event->angleDelta().y() / 120.0f; // Typical mouse wheel delta is 120
    character->camera.setVerticalFov(std::clamp(character->camera.getVerticalFov() - delta, 0.1f, 130.0f));
}

void InGameWindow::focusInEvent(QFocusEvent* event) {
    if (gameContext.currentGameState == GameState::IN_GAME_TESTING) {
        setCursor(Qt::BlankCursor);
    }
    character->camera._isFirstMouse = true;
    //std::cout << "set mouse pos 3: " << windowCenterPos.x() << " " << windowCenterPos.y() << "\n";
}

void InGameWindow::focusOutEvent(QFocusEvent* event) {
    emit lostFocus();
    setCursor(Qt::ArrowCursor);
}
