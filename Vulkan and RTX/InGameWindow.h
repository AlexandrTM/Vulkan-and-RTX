#include "pch.h"

#ifndef VULKAN_QT_WINDOW_H
#define VULKAN_QT_WINDOW_H

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include "Character.h"
#include "GameContext.h"

class InGameWindow : public QWindow
{
    Q_OBJECT

private:
    Character* character;
    GameContext* gameContext;

public:
    InGameWindow(
        QVulkanInstance* instance,
        Character& character, GameContext& gameContext
    );
    QPoint centerPos;
    double latestMouseDx = 0.0;
    double latestMouseDy = 0.0;

signals:
    void framebufferResized(int width, int height);

protected:
    void resizeEvent(QResizeEvent* event) override;

    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
};

#endif
