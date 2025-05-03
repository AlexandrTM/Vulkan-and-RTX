#pragma once

#include <QObject>
#include "gamecontext_instance.h"

class MainMenuSlotHandler : public QObject
{
    Q_OBJECT

public:
    explicit MainMenuSlotHandler(QObject* parent = nullptr)
        : QObject(parent) {
    }

public slots:
    void onStartGameClicked() {
        gameContext.requestedGameState = GameState::DUNGEON_EXPLORATION;
    }
    void onOpenSettingsClicked() {
        gameContext.requestedGameState = GameState::SETTINGS_MENU;
    }
    void onExitGameClicked() {
        gameContext.requestedGameState = GameState::EXIT;
    }
};