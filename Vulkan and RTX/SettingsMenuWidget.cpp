#include "pch.h"
#include "SettingsMenuWidget.h"

SettingsMenuWidget::SettingsMenuWidget(QWidget* parent) : QWidget(parent) {
    quickWidget = new QQuickWidget(this);
    quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    quickWidget->setSource(QUrl::fromLocalFile("qml/SettingsMenu.ui.qml"));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(quickWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    QObject* rootObject = quickWidget->rootObject();
    if (!rootObject) {
        qWarning() << "[SettingsMenuWidget] Failed to load QML root object.";
        return;
    }

    connect(rootObject, SIGNAL(returnToMainMenuClicked()), this, SIGNAL(returnToMainMenu()));
}