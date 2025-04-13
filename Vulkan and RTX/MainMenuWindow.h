#include "pch.h"

#ifndef MAIN_MENU_H
#define MAIN_MENU_H
class MainMenuWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainMenuWindow(QWidget* parent = nullptr);

signals:
    void startGame();
    void openSettings();
    void exitGame();
};

#endif

