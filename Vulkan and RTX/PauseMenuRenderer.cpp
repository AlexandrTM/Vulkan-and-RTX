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

    context->makeCurrent(offscreenSurface);

    QOpenGLFramebufferObjectFormat fmt;
    fmt.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    fmt.setTextureTarget(GL_TEXTURE_2D);
    fmt.setInternalTextureFormat(GL_RGBA8);
    fbo = new QOpenGLFramebufferObject(size, fmt);

    QQuickRenderTarget renderTarget = QQuickRenderTarget::fromOpenGLTexture(
        fbo->texture(),
        surfaceSize
    );
    quickWindow->setRenderTarget(renderTarget);
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
}

void PauseMenuRenderer::render() {
    if (!rootItem || !quickWindow || !fbo || !context->makeCurrent(offscreenSurface)) return;

    QOpenGLFunctions* f = context->functions();
    f->glViewport(0, 0, surfaceSize.width(), surfaceSize.height());
    f->glClearColor(0, 0, 0, 0); // transparent clear
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    renderControl->beginFrame();
    renderControl->polishItems();
    renderControl->sync();
    renderControl->render();
    renderControl->endFrame();

    fbo->bind();

    fbo->release();
    f->glFlush();
}


QImage PauseMenuRenderer::grabImage() {
    context->makeCurrent(offscreenSurface);
    return quickWindow->grabWindow();
}
