#include "pch.h"

#ifndef PAUSE_MENU_WIDGET_H
#define PAUSE_MENU_WIDGET_H

class PauseMenuWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PauseMenuWidget(QWidget* parent = nullptr);

signals:
    void resumeGame();
    void openSettings();
    void openMainMenu();
    void exitGame();

private:
    QQuickWidget* quickWidget;
};

#endif
