#pragma once

#include <QObject>
#include "GameContext.h"

class SolveEquationSlotHandler : public QObject
{
    Q_OBJECT

public:
    explicit SolveEquationSlotHandler(GameContext& context, QObject* parent = nullptr)
        : QObject(parent), gameContext(&context) {
    }

public slots:
    void onAnswerSubmitted(QString answer) {
        //std::cout << "answer:" << answer.toStdString() << "\n";
        qDebug() << "answer:" << answer;
    }

private:
    GameContext* gameContext = nullptr;
};