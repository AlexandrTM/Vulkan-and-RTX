#include "pch.h"

#ifndef MAIN_MENU_H
#define MAIN_MENU_H

class MainMenuWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainMenuWidget(QWidget* parent = nullptr);

signals:
    void startGame();
    void openSettings();
    void exitGame();

private:
    QQuickWidget* quickWidget;
};

#endif

