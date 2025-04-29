#include "pch.h"

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include "gamecontext_instance.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QStackedWidget* stackedWidget;

public:
    MainWindow(QWidget* parent = nullptr);
    QStackedWidget* getStackedWidget();
    void addWidget(QWidget* widget);

signals:
    void resized(int width, int height);
    void moved(int x, int y);
    void closed();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void moveEvent(QMoveEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

};


#endif
