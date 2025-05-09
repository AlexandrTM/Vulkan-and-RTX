#include "pch.h"

#ifndef VULKAN_QT_WINDOW_H
#define VULKAN_QT_WINDOW_H

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include "Character.h"
#include "gamecontext_instance.h"
#include "UserInterfaceRenderer.h"

class InGameWindow : public QWindow
{
    Q_OBJECT
private:
    Character* character = nullptr;
    UserInterfaceRenderer* mainMenuRenderer = nullptr;
    UserInterfaceRenderer* settingsMenuRenderer = nullptr;
    UserInterfaceRenderer* pauseMenuRenderer = nullptr;
    UserInterfaceRenderer* selectEquationRenderer = nullptr;
    UserInterfaceRenderer* solveEquationRenderer = nullptr;

public:
    InGameWindow(
        QVulkanInstance* instance,
        Character& character
    );
    void setMainMenuRenderer      (UserInterfaceRenderer* renderer) { mainMenuRenderer = renderer; }
    void setSettingsMenuRenderer  (UserInterfaceRenderer* renderer) { settingsMenuRenderer = renderer; }
    void setPauseMenuRenderer     (UserInterfaceRenderer* renderer) { pauseMenuRenderer = renderer; }
    void setSelectEquationRenderer(UserInterfaceRenderer* renderer) { selectEquationRenderer = renderer; }
    void setSolveEquationRenderer (UserInterfaceRenderer* renderer) { solveEquationRenderer = renderer; }

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
