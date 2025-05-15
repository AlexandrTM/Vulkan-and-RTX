#pragma once

#include <QObject>
#include "gamecontext_instance.h"
#include "Character.h"

class SolveEquationSlotHandler : public QObject
{
    Q_OBJECT

public:
    explicit SolveEquationSlotHandler(bool& isSolveEquationTextFieldActivated, QObject* parent = nullptr)
        : QObject(parent), isSolveEquationTextFieldActivated(&isSolveEquationTextFieldActivated) {
    }

public slots:
    void onAnswerSubmitted(QString answer) {
        //qDebug() << "answer:" << answer.toDouble() << " " << answer;
        *isSolveEquationTextFieldActivated = false;
        gameContext.submittedAnswer = answer.toDouble();
        gameContext.isAnswerSubmitted = true;
    }

private:
    bool* isSolveEquationTextFieldActivated = nullptr;
};