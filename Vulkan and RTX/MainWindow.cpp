#include "pch.h"
#include "MainWindow.h"

MainWindow::MainWindow(QWidget* mainMenuWidget, QWidget* inGameContainer, QWidget* parent)
    : QMainWindow(parent)
{
    stackedWidget = new QStackedWidget(this);
    stackedWidget->addWidget(mainMenuWidget);
    stackedWidget->addWidget(inGameContainer);
    setCentralWidget(stackedWidget);
}

QStackedWidget* MainWindow::getStackedWidget() {
    return stackedWidget;
}

