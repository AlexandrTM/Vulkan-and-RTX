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
        gameContext.selectedEquationIndex = buttonId;
        gameContext.requestedGameState = GameState::COMBAT_PLAYER_SOLVE_EQUATION;
    }

private:
};