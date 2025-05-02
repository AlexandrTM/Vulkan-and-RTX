#pragma once

#include <QObject>
#include "gamecontext_instance.h"
#include "Character.h"

class SolveEquationSlotHandler : public QObject
{
    Q_OBJECT

public:
    explicit SolveEquationSlotHandler(Character& character, bool& isSolveEquationTextFieldActivated, QObject* parent = nullptr)
        : QObject(parent), character(&character), isSolveEquationTextFieldActivated(&isSolveEquationTextFieldActivated) {
    }

public slots:
    void onAnswerSubmitted(QString answer) {
        //qDebug() << "answer:" << answer.toDouble() << " " << answer;
        *isSolveEquationTextFieldActivated = false;
        Equation& selectedEquation = *gameContext.selectedEquation;
        if (std::abs(answer.toDouble() - selectedEquation.answer) < 0.25) {
            handleRightAnswer(selectedEquation);
        }
        else {
            handleWrongAnswer(selectedEquation);
        }
    }

    void handleRightAnswer(Equation& selectedEquation) {
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
            //gameContext.requestedGameState = GameState::COMBAT_PLAYER_SELECT_EQUATION;
        }
        else {
            //gameContext.requestedGameState = GameState::DUNGEON_EXPLORATION
            gameContext.requestedGameState = GameState::DUNGEON_ROOM_CLEANED;
        }

        selectedEquation.isSolved = true;
    }
    void handleWrongAnswer(const Equation& selectedEquation) {
        if (gameContext.isAnswerSubmitted) {
            return;
        }

        gameContext.timeRemainingToSolveEquation -= selectedEquation.wrongAnswerPenalty;
        gameContext.timeRemainingToSolveEquation = std::max(gameContext.timeRemainingToSolveEquation, 0.0);
        gameContext.isAnswerSubmitted = true;

        QTimer::singleShot(500, this, [this]() {
            gameContext.isAnswerSubmitted = false;
        });
    }

private:
    Character* character = nullptr;
    bool* isSolveEquationTextFieldActivated = nullptr;
};