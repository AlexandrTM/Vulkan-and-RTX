#pragma once

class UserInterfaceRenderer : public QObject
{
    Q_OBJECT
public:
    UserInterfaceRenderer();
    ~UserInterfaceRenderer();

    void loadQml(const QSize& size, const QString& qmlPath);
    void render();
    void resize(const QSize& size);
    void createFbo(const QSize& size);
    void deleteFbo();
    QImage grabImage(); // taking screenshot
    QOpenGLFramebufferObject* getFbo() const { return fbo; }
    QQuickWindow* getQuickWindow() const { return quickWindow; }

    void forwardEvent(QEvent* event);
    void forwardMouseEvent(QMouseEvent* event);
    QQuickItem* getRootItem() const { return rootItem; }

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
};
