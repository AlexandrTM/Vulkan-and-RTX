#include "pch.h"
#include "MainMenu.h"

MainMenu::MainMenu(QWidget* parent) : QMainWindow(parent) {
    QWidget* central = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(central);

    QPushButton* startButton = new QPushButton(tr("Start Game"), this);
    QPushButton* settingsButton = new QPushButton(tr("Settings"), this);
    QPushButton* exitButton = new QPushButton(tr("Exit"), this);

    layout->addWidget(startButton);
    layout->addWidget(settingsButton);
    layout->addWidget(exitButton);

    setCentralWidget(central);

    connect(startButton, &QPushButton::clicked, this, &MainMenu::startGame);
    connect(settingsButton, &QPushButton::clicked, this, &MainMenu::openSettings);
    connect(exitButton, &QPushButton::clicked, this, &MainMenu::exitGame);
}