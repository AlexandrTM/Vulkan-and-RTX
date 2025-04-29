#include "pch.h"
#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    emit resized(event->size().width(), event->size().height());
    gameContext.windowCenterPos = { event->size().width() / 2, event->size().height() / 2 };
    gameContext.windowCenterPos = mapToGlobal(gameContext.windowCenterPos);
}

void MainWindow::moveEvent(QMoveEvent* event) {
    emit moved(event->pos().x(), event->pos().y());
    gameContext.windowCenterPos = { width() / 2, height() / 2 };
    gameContext.windowCenterPos = mapToGlobal(gameContext.windowCenterPos);
    //std::cout << "windowMoved: " << event->pos().x() << " " << event->pos().y() << "\n";
}

void MainWindow::addWidget(QWidget* widget)
{
    stackedWidget->addWidget(widget);
}

QStackedWidget* MainWindow::getStackedWidget() {
    return stackedWidget;
}

void MainWindow::closeEvent(QCloseEvent* event) {
    emit closed();
    QMainWindow::closeEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    //gameContext.keyboardKeys[event->key()] = true;
}

void MainWindow::keyReleaseEvent(QKeyEvent* event) {
    //gameContext.keyboardKeys[event->key()] = false;
}

void MainWindow::mousePressEvent(QMouseEvent* event) {
    //gameContext.mouseKeys[event->button()] = true;
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event) {
    //gameContext.mouseKeys[event->button()] = false;
}

