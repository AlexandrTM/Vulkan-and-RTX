#pragma once

#include <QObject>
#include "GameContext.h"

class PauseMenuSlotHandler : public QObject
{
    Q_OBJECT

public:
    explicit PauseMenuSlotHandler(GameContext& context, QObject* parent = nullptr)
        : QObject(parent), gameContext(&context) {
    }

public slots:
    void onResumeGameClicked() {
        gameContext->requestedGameState = GameState::DUNGEON_EXPLORATION;
    }

    void onOpenSettingsClicked() {
        gameContext->requestedGameState = GameState::SETTINGS_MENU;
    }

    void onOpenMainMenuClicked() {
        gameContext->requestedGameState = GameState::MAIN_MENU;
    }

    void onExitGameClicked() {
        gameContext->requestedGameState = GameState::EXIT;
    }

private:
    GameContext* gameContext = nullptr;
};