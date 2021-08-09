#include "Window.h"

#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QKeyEvent>
#include <QSlider>

#include "SceneView.h"

Window::Window() {
	// *** create OpenGL window

	QSurfaceFormat format;
	format.setRenderableType(QSurfaceFormat::OpenGL);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setVersion(3,3);
	format.setSamples(4);	// enable multisampling (antialiasing)		// DISCUSS
    format.setDepthBufferSize(24);

#ifdef GL_DEBUG_
	format.setOption(QSurfaceFormat::DebugContext);
#endif // GL_DEBUG
    mSceneView = new SceneView;
    mSceneView->setFormat(format);

	// *** create window container widget
    QWidget *container = QWidget::createWindowContainer(mSceneView);
	container->setFocusPolicy(Qt::TabFocus);
	container->setMinimumSize(QSize(640,400));

	// *** create the layout and insert widget container
	QVBoxLayout * vlay = new QVBoxLayout;
	vlay->setMargin(0);
	vlay->setSpacing(0);
	vlay->addWidget(container);

//	// now create some buttons at the bottom
//	QHBoxLayout * hlay = new QHBoxLayout;
//	hlay->setMargin(0);

//	QLabel * navigationInfo = new QLabel(this);
//	navigationInfo->setWordWrap(true);
//	navigationInfo->setText("Hold right mouse button for free mouse look and to navigate "
//							"with keys WASDQE. Hold shift to slow down. Use scroll-wheel to move quickly forward and backward. "
//							"Use left-click to select objects.");
//	hlay->addWidget(navigationInfo);


//    QPushButton * closeBtn = new QPushButton(tr("Close"), this);
//    connect(closeBtn, &QPushButton::clicked, this, &QWidget::close);

//	hlay->addWidget(closeBtn);
//	hlay->setStretch(0,1);

//	vlay->addLayout(hlay);

    mXRotSlider = createSlider();
    mYRotSlider = createSlider();
    mZRotSlider = createSlider();

    mXPosSlider = createSlider();
    mYPosSlider = createSlider();
    mZPosSlider = createSlider();


    QLabel *labelXAngleValue = new QLabel("0");
    QLabel *labelYAngleValue = new QLabel("0");
    QLabel *labelZAngleValue = new QLabel("0");
    QLabel *labelXPosValue = new QLabel("0");
    QLabel *labelYPosValue = new QLabel("0");
    QLabel *labelZPosValue = new QLabel("0");

    connect(mXRotSlider, SIGNAL(valueChanged(int)), mSceneView, SLOT(setXRotation(int)));
    connect(mYRotSlider, SIGNAL(valueChanged(int)), mSceneView, SLOT(setYRotation(int)));
    connect(mZRotSlider, SIGNAL(valueChanged(int)), mSceneView, SLOT(setZRotation(int)));

    connect(mSceneView, SIGNAL(xRotationChanged(int)), labelXAngleValue, SLOT(setNum(int)));
    connect(mSceneView, SIGNAL(xRotationChanged(int)), mXRotSlider, SLOT(setValue(int)));
    connect(mSceneView, SIGNAL(yRotationChanged(int)), labelYAngleValue, SLOT(setNum(int)));
    connect(mSceneView, SIGNAL(yRotationChanged(int)), mYRotSlider, SLOT(setValue(int)));
    connect(mSceneView, SIGNAL(zRotationChanged(int)), labelZAngleValue, SLOT(setNum(int)));
    connect(mSceneView, SIGNAL(zRotationChanged(int)), mZRotSlider, SLOT(setValue(int)));

    QHBoxLayout *sliderXRotLayout = new QHBoxLayout;
    QLabel *labelXRot = new QLabel("X-Rot");
    sliderXRotLayout->addWidget(labelXRot);
    sliderXRotLayout->addWidget(mXRotSlider);
    sliderXRotLayout->addWidget(labelXAngleValue);
    //------------------------------------
    QHBoxLayout *sliderYRotLayout = new QHBoxLayout;
    QLabel *labelYRot = new QLabel("Y-Rot");
    sliderYRotLayout->addWidget(labelYRot);
    sliderYRotLayout->addWidget(mYRotSlider);
    sliderYRotLayout->addWidget(labelYAngleValue);
    //------------------------------------
    QHBoxLayout *sliderZRotLayout = new QHBoxLayout;
    QLabel *labelZRot = new QLabel("Z-Rot");
    sliderZRotLayout->addWidget(labelZRot);
    sliderZRotLayout->addWidget(mZRotSlider);
    sliderZRotLayout->addWidget(labelZAngleValue);

    //------------------------------------
    // ROTATION
    //------------------------------------
    QGroupBox *groupBoxRotation = new QGroupBox(tr("Rotation"));
    QVBoxLayout *slidersRotationLayout = new QVBoxLayout;
    slidersRotationLayout->addLayout(sliderXRotLayout);
    slidersRotationLayout->addLayout(sliderYRotLayout);
    slidersRotationLayout->addLayout(sliderZRotLayout);
    slidersRotationLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    groupBoxRotation->setLayout(slidersRotationLayout);

    QHBoxLayout *sliderXPosLayout = new QHBoxLayout;
    QLabel *labelXPos = new QLabel("X");
    sliderXPosLayout->addWidget(labelXPos);
    sliderXPosLayout->addWidget(mXPosSlider);
    sliderXPosLayout->addWidget(labelXPosValue);
    //------------------------------------
    QHBoxLayout *sliderYPosLayout = new QHBoxLayout;
    QLabel *labelYPos = new QLabel("Y");
    sliderYPosLayout->addWidget(labelYPos);
    sliderYPosLayout->addWidget(mYPosSlider);
    sliderYPosLayout->addWidget(labelYPosValue);
    //------------------------------------
    QHBoxLayout *sliderZPosLayout = new QHBoxLayout;
    QLabel *labelZPos = new QLabel("Z");
    sliderZPosLayout->addWidget(labelZPos);
    sliderZPosLayout->addWidget(mZPosSlider);
    sliderZPosLayout->addWidget(labelZPosValue);

    //------------------------------------
    // TRANSLATION
    //------------------------------------
    QGroupBox *groupBoxTranslation = new QGroupBox(tr("Translation"));
    QVBoxLayout *slidersTranslationLayout = new QVBoxLayout;
    slidersRotationLayout->addLayout(sliderXPosLayout);
    slidersRotationLayout->addLayout(sliderYPosLayout);
    slidersRotationLayout->addLayout(sliderZPosLayout);
    slidersRotationLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    groupBoxTranslation->setLayout(slidersTranslationLayout);


    //------------------------------------
    // INFORMATIONS
    //------------------------------------
    QGroupBox *groupBoxInformation = new QGroupBox(tr("Informations"));
    QVBoxLayout *informationLayout = new QVBoxLayout;
    QLabel *labelAccelerometer = new QLabel(tr("Accelerometer (m/s2)"));
    QLabel *labelGyroscope = new QLabel(tr("Gyroscope (rad/s)"));
    QLabel *labelMagnetometer = new QLabel(tr("Magnetometer (uT)"));
    informationLayout->addWidget(labelAccelerometer);
    informationLayout->addWidget(labelGyroscope);
    informationLayout->addWidget(labelMagnetometer);
    groupBoxInformation->setLayout(informationLayout);

    QVBoxLayout *slidersLayout = new QVBoxLayout;
    slidersLayout->addWidget(groupBoxRotation);
    slidersLayout->addWidget(groupBoxTranslation);
    slidersLayout->addWidget(groupBoxInformation);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addLayout(vlay);
    mainLayout->addLayout(slidersLayout);

    setLayout(mainLayout);

    mXRotSlider->setValue(144);
    mYRotSlider->setValue(46);
    mZRotSlider->setValue(3);
    setWindowTitle(tr("Emulator | Widget motion sensors"));

//	resize(700,450);

	container->setFocus();
}

QSlider *Window::createSlider()
{
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 360);
    slider->setSingleStep(1);
    //slider->setPageStep(15 * 16);
    slider->setTickInterval(15 * 16);
    slider->setTickPosition(QSlider::TicksRight);
    return slider;
}


void Window::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();
    else
        QWidget::keyPressEvent(e);
}
