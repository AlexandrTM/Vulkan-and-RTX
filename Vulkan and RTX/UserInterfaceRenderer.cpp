#include "pch.h"
#include "UserInterfaceRenderer.h"

UserInterfaceRenderer::UserInterfaceRenderer() {
    renderControl = new QQuickRenderControl();
    quickWindow = new QQuickWindow(renderControl);
    quickWindow->setColor(Qt::transparent);
    quickWindow->setGraphicsApi(QSGRendererInterface::OpenGL);

    context = new QOpenGLContext;
    context->setFormat(QSurfaceFormat::defaultFormat());
    context->create();

    offscreenSurface = new QOffscreenSurface();
    offscreenSurface->setFormat(context->format());
    offscreenSurface->create();

    context->makeCurrent(offscreenSurface);
    quickWindow->setGraphicsDevice(QQuickGraphicsDevice::fromOpenGLContext(context));
    renderControl->initialize();

    engine = new QQmlEngine;
    if (!engine->incubationController()) {
        engine->setIncubationController(quickWindow->incubationController());
    }
}

UserInterfaceRenderer::~UserInterfaceRenderer() {
    if (rootItem) {
        rootItem->setParentItem(nullptr);
        delete rootItem;
    }

    delete renderControl;
    delete quickWindow;
    delete engine;
    delete component;
    delete fbo;

    if (context) {
        context->doneCurrent();
        delete context;
    }

    if (offscreenSurface) {
        offscreenSurface->destroy();
        delete offscreenSurface;
    }
}

bool UserInterfaceRenderer::handleEvent(QEvent* event) {
    switch (event->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove: {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        forwardMouseEvent(mouseEvent);
        return true;
    }
    case QEvent::HoverMove: {
        QHoverEvent* hoverEvent = static_cast<QHoverEvent*>(event);
        forwardHoverEvent(hoverEvent);
        return true;
    }
    default:
        return false;
    }
}

void UserInterfaceRenderer::forwardMouseEvent(QMouseEvent* event) {
    QCoreApplication::sendEvent(quickWindow, event);
}

void UserInterfaceRenderer::forwardHoverEvent(QHoverEvent* event) {
    QCoreApplication::sendEvent(quickWindow, event);
}

void UserInterfaceRenderer::initialize(const QSize& size, const QString& qmlPath) {
    component = new QQmlComponent(engine, QUrl::fromLocalFile(qmlPath));

    rootItem = qobject_cast<QQuickItem*>(component->create());
    if (!rootItem) {
        qWarning() << "[UserInterfaceRenderer] Failed to load QML root item." << qmlPath;
        return;
    }

    rootItem->setParentItem(quickWindow->contentItem());
    rootItem->setSize(size);
    quickWindow->resize(size);

    surfaceSize = size;

    createFbo(size);
}

void UserInterfaceRenderer::resize(const QSize& size) {
    if (!rootItem) return;

    rootItem->setSize(size);
    quickWindow->resize(size);
    surfaceSize = size;

    if (fbo) {
        delete fbo;
        fbo = nullptr;
    }

    createFbo(size);
}

void UserInterfaceRenderer::createFbo(const QSize& size) {
    context->makeCurrent(offscreenSurface);

    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setTextureTarget(GL_TEXTURE_2D);
    format.setInternalTextureFormat(GL_RGBA8);
    fbo = new QOpenGLFramebufferObject(size, format);

    QQuickRenderTarget renderTarget = QQuickRenderTarget::fromOpenGLTexture(
        fbo->texture(),
        surfaceSize
    );
    quickWindow->setRenderTarget(renderTarget);
}

void UserInterfaceRenderer::render() {
    if (!rootItem || !quickWindow || !fbo) return;

    /*qDebug() << "Current context before:" << QOpenGLContext::currentContext();
    qDebug() << "Expected context before:" << context;*/

    if (!context->makeCurrent(offscreenSurface)) {
        qWarning() << "Failed to make OpenGL context current!";
        return;
    }
    
    QOpenGLFunctions* f = context->functions();
    f->glViewport(0, 0, surfaceSize.width(), surfaceSize.height());
    f->glClearColor(0, 0, 0, 0); // transparent clear
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderControl->beginFrame();
    renderControl->polishItems();
    renderControl->sync();
    renderControl->render();
    renderControl->endFrame();

    f->glFlush();
}


QImage UserInterfaceRenderer::grabImage() {
    context->makeCurrent(offscreenSurface);
    QImage image = fbo->toImage();
    //image.save("pause_menu_debug.png");
    //return quickWindow->grabWindow();
    return image;
}
