#include "pch.h"

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QStackedWidget* stackedWidget;

public:
    MainWindow(QWidget* parent = nullptr);
    QStackedWidget* getStackedWidget();
    void addWidgets(QWidget* mainMenuWidget, QWidget* inGameContainer);

signals:
    void windowClosed();

protected:
    void closeEvent(QCloseEvent* event) override;

};


#endif
