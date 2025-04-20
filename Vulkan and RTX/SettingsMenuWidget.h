#include "pch.h"

#ifndef SETTINGS_MENU_WIDGET_H
#define SETTINGS_MENU_WIDGET_H

class SettingsMenuWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SettingsMenuWidget(QWidget* parent = nullptr);

signals:
    void returnToMainMenu();

private:
    QQuickWidget* quickWidget;
};

#endif
