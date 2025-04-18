﻿#include "pch.h"
#include "MainMenuWidget.h"

MainMenuWidget::MainMenuWidget(QWidget* parent) : QWidget(parent) {
    quickWidget = new QQuickWidget(this);
    quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    quickWidget->setSource(QUrl::fromLocalFile("qml/MainMenu.ui.qml"));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(quickWidget);
    setLayout(layout);

    QObject* rootObject = quickWidget->rootObject();
    if (!rootObject) {
        qWarning() << "[MainMenuWidget] Failed to load QML root object.";
        return;
    }

    // Connect the signals
    connect(rootObject, SIGNAL(startGameClicked()), this, SIGNAL(startGame()));
    connect(rootObject, SIGNAL(openSettingsClicked()), this, SIGNAL(openSettings()));
    connect(rootObject, SIGNAL(exitGameClicked()), this, SIGNAL(exitGame()));
}