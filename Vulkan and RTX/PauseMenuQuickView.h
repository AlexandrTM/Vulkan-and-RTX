#include "pch.h"

#ifndef PAUSE_MENU_QUICK_VIEW_H
#define PAUSE_MENU_QUICK_VIEW_H

class PauseMenuQuickView : public QQuickView
{
    Q_OBJECT
public:
    explicit PauseMenuQuickView(QWindow* parentWindow = nullptr);

signals:
    void resumeGame();
    void openSettings();
    void openMainMenu();
    void exitGame();
};

#endif
