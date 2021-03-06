#include "SceneView.h"

#include <QExposeEvent>
#include <QOpenGLShaderProgram>
#include <QDateTime>

#include "DebugApplication.h"
#include "PickObject.h"

#define SHADER(x) m_shaderPrograms[x].shaderProgram()

const QVector3D Y_VECTOR = QVector3D(0.0f, 1.0f, 0.0f);
const QVector3D X_VECTOR = QVector3D(1.0f, 0.0f, 0.0f);
const QVector3D Z_VECTOR = QVector3D(0.0f, 0.0f, 1.0f);

SceneView::SceneView() :
    m_inputEventReceived(false)
{
    // tell keyboard handler to monitor certain keys
    m_keyboardMouseHandler.addRecognizedKey(Qt::Key_W);
    m_keyboardMouseHandler.addRecognizedKey(Qt::Key_A);
    m_keyboardMouseHandler.addRecognizedKey(Qt::Key_S);
    m_keyboardMouseHandler.addRecognizedKey(Qt::Key_D);
    m_keyboardMouseHandler.addRecognizedKey(Qt::Key_Q);
    m_keyboardMouseHandler.addRecognizedKey(Qt::Key_E);
    m_keyboardMouseHandler.addRecognizedKey(Qt::Key_Shift);

    // *** create scene (no OpenGL calls are being issued below, just the data structures are created.

    // Shaderprogram #0 : regular geometry (painting triangles via element index)
    ShaderProgram blocks(":/shaders/withWorldAndCamera.vert",":/shaders/simple.frag");
    blocks.m_uniformNames.append("worldToView");
    m_shaderPrograms.append( blocks );

    // Shaderprogram #1 : grid (painting grid lines)
    ShaderProgram grid(":/shaders/grid.vert",":/shaders/grid.frag");
    grid.m_uniformNames.append("worldToView"); // mat4
    grid.m_uniformNames.append("gridColor"); // vec3
    grid.m_uniformNames.append("backColor"); // vec3
    m_shaderPrograms.append( grid );

    // Shaderprogram #2 : regular geometry with lighting
    ShaderProgram lightedBlocks(":/shaders/VertexNormalColor.vert",":/shaders/diffuse.frag");
    lightedBlocks.m_uniformNames.append("worldToView");
    lightedBlocks.m_uniformNames.append("lightPos");
    lightedBlocks.m_uniformNames.append("lightColor");
    m_shaderPrograms.append( lightedBlocks );

    // Shaderprogram #3 : transparent planes
    ShaderProgram transPlanes(":/shaders/VertexColorTransparent.vert",":/shaders/simple.frag");
    transPlanes.m_uniformNames.append("worldToView");
    m_shaderPrograms.append( transPlanes );

    // Shaderprogram #4 : planes with textures
    ShaderProgram texturedPlanes(":/shaders/VertexFontTexture.vert",":/shaders/texture.frag");
    texturedPlanes.m_uniformNames.append("worldToView");
    texturedPlanes.m_uniformNames.append("text01"); // associate uniform index with texture name
    m_shaderPrograms.append( texturedPlanes );

    // *** initialize camera placement and model placement in the world

    // move camera a little back and up
    m_camera.translate(47,100,-147);
    // look slightly down
    m_camera.rotate(-15, m_camera.right());
    // look slightly right
    m_camera.rotate(165, Y_VECTOR);
    m_camera.rotate(3, X_VECTOR);

}


SceneView::~SceneView() {
    if (m_context) {
        m_context->makeCurrent(this);

        for (ShaderProgram & p : m_shaderPrograms)
            p.destroy();

//        m_boxObject.destroy();
        m_smartPhoneObject.destroy();
        m_gridObject.destroy();
        m_axisObject.destroy();
        m_textObject.destroy();

        m_gpuTimers.destroy();
    }
}


void SceneView::initializeGL() {
    FUNCID(SceneView::initializeGL);
    try {
        // initialize shader programs
        for (ShaderProgram & p : m_shaderPrograms)
            p.create();

        // enable depth testing, important for the grid and for the drawing order of several objects
        glEnable(GL_DEPTH_TEST);

        // initialize drawable objects
//        m_boxObject.create(SHADER(2));
        m_smartPhoneObject.create(SHADER(2));
        m_gridObject.create(SHADER(1));
        m_axisObject.create(SHADER(0));

        m_textObject.addText("XXX", QVector3D(0,30,0), QVector3D(10,30,0), QVector3D(0,45,0));
        m_textObject.addText("YYY", QVector3D(-70,30,70), QVector3D(0,30,0), QVector3D(-70,45,70));

        m_textObject.create(m_shaderPrograms[4]);

        // Timer
        m_gpuTimers.setSampleCount(5);
        m_gpuTimers.create();
    }
    catch (OpenGLException & ex) {
        throw OpenGLException(ex, "OpenGL initialization failed.", FUNC_ID);
    }
}


void SceneView::resizeGL(int width, int height) {
    // the projection matrix need to be updated only for window size changes
    m_projection.setToIdentity();
    // create projection matrix, i.e. camera lens
    m_projection.perspective(
                /* vertical angle */ 45.0f,
                /* aspect ratio */   width / float(height),
                /* near */           0.1f,
                /* far */            10000.0f
        );
    // Mind: to not use 0.0 for near plane, otherwise depth buffering and depth testing won't work!

    // update cached world2view matrix
    updateWorld2ViewMatrix();
}


void SceneView::paintGL() {
    m_cpuTimer.start();
    if (((DebugApplication *)qApp)->m_aboutToTerminate)
        return;

    // process input, i.e. check if any keys have been pressed
    if (m_inputEventReceived)
        processInput();

    const qreal retinaScale = devicePixelRatio(); // needed for Macs with retina display
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);
    qDebug() << "SceneView::paintGL(): Rendering to:" << width() << "x" << height();

    // enable updating of z-buffer; NOTE: must be enabled before call to glClear(), because
    // otherwise the depth buffer won't be modified.
    glDepthMask(GL_TRUE);

    // set the background color = clear color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // set the background color = clear color
    QVector3D backColor(0.1f, 0.15f, 0.3f);
    glClearColor(0.1f, 0.15f, 0.3f, 1.0f);

    QVector3D gridColor(0.5f, 0.5f, 0.7f);
    QVector3D lightColor(1.f, 1.f, 1.f);

    QVector3D lightPos(0.f, 2800.f, 1500.f);

    m_rotationCounter = (m_rotationCounter + 1) % 1800;
    QQuaternion lightRot = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), -0.2*m_rotationCounter);
//	QQuaternion lightRot = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), 5*m_rotationCounter/180. * 3.1415);
    lightPos = lightRot.rotatedVector(lightPos);
//	qDebug() << lightPos;
//	renderLater();

    m_gpuTimers.reset();

    // tell OpenGL to show only faces whose normal vector points towards us
    glEnable(GL_CULL_FACE);

    // *** render boxes
    m_gpuTimers.recordSample();

    SHADER(2)->bind();
    SHADER(2)->setUniformValue(m_shaderPrograms[2].m_uniformIDs[0], m_worldToView);
    SHADER(2)->setUniformValue(m_shaderPrograms[2].m_uniformIDs[1], lightPos);
    SHADER(2)->setUniformValue(m_shaderPrograms[2].m_uniformIDs[2], lightColor);

    m_smartPhoneObject.render();
    //m_boxObject.render();

    SHADER(2)->release();


    // *** render lines
    m_gpuTimers.recordSample();

    SHADER(0)->bind();
    SHADER(0)->setUniformValue(m_shaderPrograms[0].m_uniformIDs[0], m_worldToView);

    //if (m_axisObject.m_visible)
        m_axisObject.render();

    SHADER(0)->release();


    // *** render grid ***
    m_gpuTimers.recordSample();

    SHADER(1)->bind();
    SHADER(1)->setUniformValue(m_shaderPrograms[1].m_uniformIDs[0], m_worldToView);
    SHADER(1)->setUniformValue(m_shaderPrograms[1].m_uniformIDs[1], gridColor);
    SHADER(1)->setUniformValue(m_shaderPrograms[1].m_uniformIDs[2], backColor);
    m_gridObject.render();

//    SHADER(1)->setUniformValue(m_shaderPrograms[1].m_uniformIDs[1], majorGridColor);
//    m_majorGridObject.render();

    SHADER(1)->release();

    // tell OpenGL to show all planes
    glDisable(GL_CULL_FACE);

    // disable update of depth test but still use it
    glDepthMask (GL_FALSE);

    // *** render transparent planes
    m_gpuTimers.recordSample(); // done painting

    // *** render text (always in front of all transparent stuff)

    SHADER(4)->bind();
    SHADER(4)->setUniformValue(m_shaderPrograms[4].m_uniformIDs[0], m_worldToView);
    m_textObject.render();
    SHADER(4)->release();

    m_gpuTimers.recordSample(); // done painting


#if 0
    // do some animation stuff
    m_transform.rotate(1.0f, QVector3D(0.0f, 0.1f, 0.0f));
    updateWorld2ViewMatrix();
    renderLater();
#endif

    checkInput();

    QVector<GLuint64> intervals = m_gpuTimers.waitForIntervals();
    for (GLuint64 it : intervals)
        qDebug() << "  " << it*1e-6 << "ms/frame";
    QVector<GLuint64> samples = m_gpuTimers.waitForSamples();
    qDebug() << "Total render time: " << (samples.back() - samples.front())*1e-6 << "ms/frame";

    qint64 elapsedMs = m_cpuTimer.elapsed();
    qDebug() << "Total paintGL time: " << elapsedMs << "ms";
}


void SceneView::keyPressEvent(QKeyEvent *event) {
    m_keyboardMouseHandler.keyPressEvent(event);
    checkInput();
}

void SceneView::keyReleaseEvent(QKeyEvent *event) {
    m_keyboardMouseHandler.keyReleaseEvent(event);
    checkInput();
}

void SceneView::mousePressEvent(QMouseEvent *event) {
    m_keyboardMouseHandler.mousePressEvent(event);
    checkInput();
}

void SceneView::mouseReleaseEvent(QMouseEvent *event) {
    m_keyboardMouseHandler.mouseReleaseEvent(event);
    checkInput();
}

void SceneView::mouseMoveEvent(QMouseEvent * /*event*/) {
    checkInput();
}

void SceneView::wheelEvent(QWheelEvent *event) {
    m_keyboardMouseHandler.wheelEvent(event);
    checkInput();
}

void SceneView::checkInput() {
    // this function is called whenever _any_ key/mouse event was issued

    // we test, if the current state of the key handler requires a scene update
    // (camera movement) and if so, we just set a flag to do that upon next repaint
    // and we schedule a repaint

    // trigger key held?
    if (m_keyboardMouseHandler.buttonDown(Qt::RightButton)) {
        // any of the interesting keys held?
        if (m_keyboardMouseHandler.keyDown(Qt::Key_W) ||
            m_keyboardMouseHandler.keyDown(Qt::Key_A) ||
            m_keyboardMouseHandler.keyDown(Qt::Key_S) ||
            m_keyboardMouseHandler.keyDown(Qt::Key_D) ||
            m_keyboardMouseHandler.keyDown(Qt::Key_Q) ||
            m_keyboardMouseHandler.keyDown(Qt::Key_E))
        {
            m_inputEventReceived = true;
//			qDebug() << "SceneView::checkInput() inputEventReceived";
            renderLater();
            return;
        }

        // has the mouse been moved?
        if (m_keyboardMouseHandler.mouseDownPos() != QCursor::pos()) {
            m_inputEventReceived = true;
//			qDebug() << "SceneView::checkInput() inputEventReceived: " << QCursor::pos() << m_keyboardMouseHandler.mouseDownPos();
            renderLater();
            return;
        }
    }
    // has the left mouse butten been release
    if (m_keyboardMouseHandler.buttonReleased(Qt::LeftButton)) {
        m_inputEventReceived = true;
        renderLater();
        return;
    }

    // scroll-wheel turned?
    if (m_keyboardMouseHandler.wheelDelta() != 0) {
        m_inputEventReceived = true;
        renderLater();
        return;
    }
}


void SceneView::processInput() {
    // function must only be called if an input event has been received
    Q_ASSERT(m_inputEventReceived);
    m_inputEventReceived = false;
//	qDebug() << "SceneView::processInput()";

    // check for trigger key
    if (m_keyboardMouseHandler.buttonDown(Qt::RightButton)) {

        // Handle translations
        QVector3D translation;
        if (m_keyboardMouseHandler.keyDown(Qt::Key_W)) 		translation += m_camera.forward();
        if (m_keyboardMouseHandler.keyDown(Qt::Key_S)) 		translation -= m_camera.forward();
        if (m_keyboardMouseHandler.keyDown(Qt::Key_A)) 		translation -= m_camera.right();
        if (m_keyboardMouseHandler.keyDown(Qt::Key_D)) 		translation += m_camera.right();
        if (m_keyboardMouseHandler.keyDown(Qt::Key_Q)) 		translation -= m_camera.up();
        if (m_keyboardMouseHandler.keyDown(Qt::Key_E)) 		translation += m_camera.up();

        float transSpeed = 0.8f;
        if (m_keyboardMouseHandler.keyDown(Qt::Key_Shift))
            transSpeed = 0.1f;
        m_camera.translate(transSpeed * translation);

        // Handle rotations
        // get and reset mouse delta (pass current mouse cursor position)
        QPoint mouseDelta = m_keyboardMouseHandler.resetMouseDelta(QCursor::pos()); // resets the internal position
        static const float rotatationSpeed  = 0.4f;
        const QVector3D LocalUp(0.0f, 1.0f, 0.0f); // same as in Camera::up()
        m_camera.rotate(-rotatationSpeed * mouseDelta.x(), LocalUp);
        m_camera.rotate(-rotatationSpeed * mouseDelta.y(), m_camera.right());

    }
    int wheelDelta = m_keyboardMouseHandler.resetWheelDelta();
    if (wheelDelta != 0) {
        float transSpeed = 8.f;
        if (m_keyboardMouseHandler.keyDown(Qt::Key_Shift))
            transSpeed = 0.8f;
        m_camera.translate(wheelDelta * transSpeed * m_camera.forward());
    }

    // finally, reset "WasPressed" key states
    m_keyboardMouseHandler.clearWasPressedKeyStates();

    updateWorld2ViewMatrix();
    // not need to request update here, since we are called from paint anyway
}


void SceneView::updateWorld2ViewMatrix() {
    // transformation steps:
    //   model space -> transform -> world space
    //   world space -> camera/eye -> camera view
    //   camera view -> projection -> normalized device coordinates (NDC)
    m_worldToView = m_projection * m_camera.toMatrix() * m_transform.toMatrix();
}
