#include "pch.h"
#include "PauseMenuQuickView.h"

PauseMenuQuickView::PauseMenuQuickView(QWindow* parentWindow) : QQuickView(parentWindow) {
    setResizeMode(QQuickView::SizeRootObjectToView);
    setSource(QUrl::fromLocalFile("qml/PauseMenu.ui.qml"));

    setFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setColor(Qt::transparent);

    QObject* rootObject = this->rootObject();
    if (!rootObject) {
        qWarning() << "[PauseMenuQuickView] Failed to load QML root object.";
        return;
    }

    connect(rootObject, SIGNAL(resumeGameClicked()), this, SIGNAL(resumeGame()));
    connect(rootObject, SIGNAL(openSettingsClicked()), this, SIGNAL(openSettings()));
    connect(rootObject, SIGNAL(openMainMenuClicked()), this, SIGNAL(openMainMenu()));
    connect(rootObject, SIGNAL(exitGameClicked()), this, SIGNAL(exitGame()));
}
