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
        qDebug() << "Button clicked:" << buttonId;
    }

private:
    GameContext* gameContext = nullptr;
};