#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>

class QSlider;
class SceneView;

class Window : public QWidget {
	Q_OBJECT

public:
    Window();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    QSlider *createSlider();

    QSlider *mXRotSlider, *mYRotSlider, *mZRotSlider;
    QSlider *mXPosSlider, *mYPosSlider, *mZPosSlider;

	// The scene view, that shows our world and allows navigation
    SceneView * mSceneView;
};

#endif // WINDOW_H
