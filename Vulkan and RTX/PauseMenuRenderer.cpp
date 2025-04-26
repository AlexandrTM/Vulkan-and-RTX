#include "pch.h"
#include "PauseMenuRenderer.h"

PauseMenuRenderer::PauseMenuRenderer() {
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

PauseMenuRenderer::~PauseMenuRenderer() {
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

void PauseMenuRenderer::forwardMouseEvent(QMouseEvent* event) {
    QCoreApplication::sendEvent(quickWindow, event);
}

void PauseMenuRenderer::forwardHoverEvent(QHoverEvent* event) {
    QCoreApplication::sendEvent(quickWindow, event);
}

void PauseMenuRenderer::initialize(const QSize& size) {
    component = new QQmlComponent(engine, QUrl::fromLocalFile("qml/PauseMenu.ui.qml"));

    rootItem = qobject_cast<QQuickItem*>(component->create());
    if (!rootItem) {
        qWarning() << "[PauseMenuRenderControl] Failed to load QML root item.";
        return;
    }

    rootItem->setParentItem(quickWindow->contentItem());
    rootItem->setSize(size);
    quickWindow->resize(size);

    setupConnections();

    surfaceSize = size;

    createFbo(size);
}

void PauseMenuRenderer::setupConnections() {
    QObject::connect(rootItem, SIGNAL(resumeGameClicked()), this, SIGNAL(resumeGame()));
    QObject::connect(rootItem, SIGNAL(openSettingsClicked()), this, SIGNAL(openSettings()));
    QObject::connect(rootItem, SIGNAL(openMainMenuClicked()), this, SIGNAL(openMainMenu()));
    QObject::connect(rootItem, SIGNAL(exitGameClicked()), this, SIGNAL(exitGame()));
}

void PauseMenuRenderer::resize(const QSize& size) {
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

void PauseMenuRenderer::createFbo(const QSize& size) {
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

void PauseMenuRenderer::render() {
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


QImage PauseMenuRenderer::grabImage() {
    context->makeCurrent(offscreenSurface);
    QImage image = fbo->toImage();
    //image.save("pause_menu_debug.png");
    //return quickWindow->grabWindow();
    return image;
}
