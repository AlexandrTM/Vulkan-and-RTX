#include "pch.h"
#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);
}

void MainWindow::addWidgets(QWidget* mainMenuWidget, QWidget* inGameContainer)
{
    stackedWidget->addWidget(mainMenuWidget);
    stackedWidget->addWidget(inGameContainer);
    show();
}

QStackedWidget* MainWindow::getStackedWidget() {
    return stackedWidget;
}

void MainWindow::closeEvent(QCloseEvent* event) {
    emit windowClosed();
    QMainWindow::closeEvent(event);
}

