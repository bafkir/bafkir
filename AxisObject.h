#ifndef AXISOBJECT_H
#define AXISOBJECT_H

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

QT_BEGIN_NAMESPACE
class QOpenGLShaderProgram;
QT_END_NAMESPACE


#include "Vertex.h"

/*! For drawing a simple line. */
class AxisObject {
public:
	void create(QOpenGLShaderProgram * shaderProgramm);
	void destroy();
	void render();

	void setPoints(const QVector3D & a, const QVector3D & b);

    bool						m_visible = false;
	std::vector<Vertex>			m_vertexBufferData;
	QOpenGLVertexArrayObject	m_vao;
	QOpenGLBuffer				m_vbo;
};

#endif // AXISOBJECT_H
