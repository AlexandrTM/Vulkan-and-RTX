#pragma once

#include <QObject>
#include "gamecontext_instance.h"
#include "Character.h"

class SolveEquationSlotHandler : public QObject
{
    Q_OBJECT

public:
    explicit SolveEquationSlotHandler(Character& character, QObject* parent = nullptr)
        : QObject(parent), character(&character) {
    }

public slots:
    void onAnswerSubmitted(QString answer) {
        //qDebug() << "answer:" << answer.toDouble() << " " << answer;

        const Equation& selectedEquation = gameContext.equations[gameContext.selectedEquationIndex];
        if (answer.toDouble() == selectedEquation.answer) {
            auto& mobs = gameContext.currentRoom->mobs;
            Mob& mob = mobs[0];
            mob.takeDamage(selectedEquation.damage + character->attackPower);

            // Award experience for dead mobs BEFORE erasing them
            for (const Mob& mob : mobs) {
                if (!mob.isAlive()) {
                    character->experience += mob.experienceReward;
                }
            }

            mobs.erase(std::remove_if(mobs.begin(), mobs.end(),
                [](const Mob& mob) { return !mob.isAlive(); }), mobs.end());

            if (!mobs.empty()) {
                gameContext.requestedGameState = GameState::COMBAT_MOB_TURN;
            }
            else {
                gameContext.requestedGameState = GameState::DUNGEON_EXPLORATION;
            }
        }
    }

private:
    Character* character = nullptr;
};