#include "pch.h"

#ifndef VULKAN_QT_WINDOW_H
#define VULKAN_QT_WINDOW_H

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include "Character.h"
#include "GameContext.h"
#include "PauseMenuRenderer.h"

class InGameWindow : public QWindow
{
    Q_OBJECT
private:
    Character* character;
    GameContext* gameContext;
    PauseMenuRenderer* pauseMenuView = nullptr;

public:
    InGameWindow(
        QVulkanInstance* instance,
        Character& character, GameContext& gameContext
    );
    void setPauseMenuView(PauseMenuRenderer* view) { pauseMenuView = view; }

    double latestMouseDx = 0.0;
    double latestMouseDy = 0.0;

signals:
    void resized(int width, int height);
    void moved(int x, int y);
    void closed();
    void lostFocus();

protected:
    bool eventFilter(QObject* obj, QEvent* event);
    bool sendEventToUI(QEvent* event);

    void resizeEvent(QResizeEvent* event) override;
    void moveEvent(QMoveEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
};

#endif
