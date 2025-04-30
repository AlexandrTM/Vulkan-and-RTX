#pragma once

#include <QObject>
#include "gamecontext_instance.h"

class SelectEquationSlotHandler : public QObject
{
    Q_OBJECT

public:
    explicit SelectEquationSlotHandler(QObject* parent = nullptr)
        : QObject(parent) {
    }

public slots:
    void onButtonClicked(int buttonId) {
        gameContext.selectedEquation = &gameContext.equations[buttonId];
        gameContext.requestedGameState = GameState::COMBAT_PLAYER_SOLVE_EQUATION;
    }

private:
};