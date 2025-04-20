#include "pch.h"
#include "MainWindow.h"

MainWindow::MainWindow(GameContext& gameContext, QWidget* parent)
    : QMainWindow(parent)
{
    stackedWidget = new QStackedWidget(this);
    this->gameContext = &gameContext;
    setCentralWidget(stackedWidget);
}

void MainWindow::addWidget(QWidget* widget)
{
    stackedWidget->addWidget(widget);
}

QStackedWidget* MainWindow::getStackedWidget() {
    return stackedWidget;
}

void MainWindow::closeEvent(QCloseEvent* event) {
    emit windowClosed();
    QMainWindow::closeEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    gameContext->keyboardKeys[event->key()] = true;
}

void MainWindow::keyReleaseEvent(QKeyEvent* event) {
    gameContext->keyboardKeys[event->key()] = false;
}

void MainWindow::mousePressEvent(QMouseEvent* event) {
    gameContext->mouseKeys[event->button()] = true;
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event) {
    gameContext->mouseKeys[event->button()] = false;
}

