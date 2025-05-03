#pragma once

#include <QObject>
#include "gamecontext_instance.h"

class SettingsMenuSlotHandler : public QObject
{
    Q_OBJECT

public:
    explicit SettingsMenuSlotHandler(QObject* parent = nullptr)
        : QObject(parent) {
    }

public slots:
    void onReturnToMainMenuClicked() {
        gameContext.requestedGameState = GameState::MAIN_MENU;
    }
};