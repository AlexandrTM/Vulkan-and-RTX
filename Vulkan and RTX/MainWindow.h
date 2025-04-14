#include "pch.h"

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* mainMenuWidget, QWidget* inGameContainer, QWidget* parent = nullptr);
    QStackedWidget* getStackedWidget();

private:
    QStackedWidget* stackedWidget;
};


#endif
