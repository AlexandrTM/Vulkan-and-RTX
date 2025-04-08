#include "pch.h"

#ifndef MAIN_MENU_H
#define MAIN_MENU_H
class MainMenu : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainMenu(QWidget* parent = nullptr);

signals:
    void startGame();
    void openSettings();
    void exitGame();
};

#endif

