#pragma once

#include <QObject>
#include "gamecontext_instance.h"

class PauseMenuSlotHandler : public QObject
{
    Q_OBJECT

public:
    explicit PauseMenuSlotHandler(QObject* parent = nullptr)
        : QObject(parent) {
    }

public slots:
    void onResumeGameClicked() {
        gameContext.requestedGameState = GameState::DUNGEON_EXPLORATION;
    }
    void onOpenSettingsClicked() {
        gameContext.requestedGameState = GameState::SETTINGS_MENU;
    }
    void onOpenMainMenuClicked() {
        gameContext.requestedGameState = GameState::MAIN_MENU;
    }
    void onExitGameClicked() {
        gameContext.requestedGameState = GameState::EXIT;
    }
};