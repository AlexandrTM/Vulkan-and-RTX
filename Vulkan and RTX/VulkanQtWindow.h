#include "pch.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include "Character.h"

#ifndef VULKAN_QT_WINDOW_H
#define VULKAN_QT_WINDOW_H

class VulkanQtWindow : public QWindow
{
    Q_OBJECT

private:
    Character* character;
    QPoint centerPos;

public:
    VulkanQtWindow(QVulkanInstance* instance, Character& character);

signals:
    void framebufferResized(int width, int height);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
};

#endif
