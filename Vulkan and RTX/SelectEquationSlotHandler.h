#pragma once

#include <QObject>
#include "GameContext.h"

class SelectEquationSlotHandler : public QObject
{
    Q_OBJECT

public:
    explicit SelectEquationSlotHandler(GameContext& context, QObject* parent = nullptr)
        : QObject(parent), gameContext(&context) {
    }

public slots:
    void onButtonClicked(int buttonId) {
        gameContext->requestedGameState = GameState::COMBAT_PLAYER_SOLVE_EQUATION;
        gameContext->selectedEquation = buttonId;
    }

private:
    GameContext* gameContext = nullptr;
};