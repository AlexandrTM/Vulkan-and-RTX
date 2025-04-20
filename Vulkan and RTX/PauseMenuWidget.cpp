#include "pch.h"
#include "PauseMenuWidget.h"

PauseMenuWidget::PauseMenuWidget(QWidget* parent) : QWidget(parent) {
    quickWidget = new QQuickWidget(this);
    quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    quickWidget->setSource(QUrl::fromLocalFile("qml/PauseMenu.ui.qml"));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(quickWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    QObject* rootObject = quickWidget->rootObject();
    if (!rootObject) {
        qWarning() << "[PauseMenuWidget] Failed to load QML root object.";
        return;
    }

    connect(rootObject, SIGNAL(resumeGameClicked()), this, SIGNAL(resumeGame()));
    connect(rootObject, SIGNAL(openSettingsClicked()), this, SIGNAL(openSettings()));
    connect(rootObject, SIGNAL(openMainMenuClicked()), this, SIGNAL(openMainMenu()));
    connect(rootObject, SIGNAL(exitGameClicked()), this, SIGNAL(exitGame()));
}
