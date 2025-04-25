#pragma once

class PauseMenuRenderer : public QObject
{
    Q_OBJECT
public:
    PauseMenuRenderer();
    ~PauseMenuRenderer();

    void initialize(const QSize& size);
    void render();
    void resize(const QSize& size);
    QImage grabImage(); // taking screenshot
    QOpenGLFramebufferObject* getFbo() const { return fbo; }

signals:
    void resumeGame();
    void openSettings();
    void openMainMenu();
    void exitGame();

private:
    QQuickRenderControl* renderControl = nullptr;
    QQuickWindow* quickWindow = nullptr;
    QQmlEngine* engine = nullptr;
    QQmlComponent* component = nullptr;
    QQuickItem* rootItem = nullptr;

    QOpenGLFramebufferObject* fbo = nullptr;
    QOpenGLContext* context = nullptr;
    QOffscreenSurface* offscreenSurface = nullptr;
    QSize surfaceSize;

    void setupConnections();
};
