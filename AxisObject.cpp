#include "AxisObject.h"

#include <QOpenGLShaderProgram>
#include <vector>

void AxisObject::create(QOpenGLShaderProgram * shaderProgramm) {
	// create a temporary buffer that will contain the x-z coordinates of all grid lines
	// we have 1 line, with two vertexes, with 2xthree floats (position and color)
    m_vertexBufferData.resize(6);

//	m_vertexBufferData[0] = Vertex(QVector3D(5,5,5), Qt::white);
//	m_vertexBufferData[1] = Vertex(QVector3D(-5,-5,-5), Qt::red);

    m_vertexBufferData[0] = Vertex(QVector3D(48.5, 97.5, -142), Qt::red);
    m_vertexBufferData[1] = Vertex(QVector3D(47.5, 97.5, -142), Qt::red);
    m_vertexBufferData[2] = Vertex(QVector3D(48.5, 97.5, -142), Qt::green);
    m_vertexBufferData[3] = Vertex(QVector3D(48.5, 98.5, -142), Qt::green);
    m_vertexBufferData[4] = Vertex(QVector3D(48.5, 97.5, -142), Qt::blue);
    m_vertexBufferData[5] = Vertex(QVector3D(48.5, 97.5, -143), Qt::blue);

	// Create Vertex Array Object
	m_vao.create();		// create Vertex Array Object
	m_vao.bind();		// and bind it

	// Create Vertex Buffer Object
	m_vbo.create();
	m_vbo.bind();
	m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	int vertexMemSize = m_vertexBufferData.size()*sizeof(Vertex);
	m_vbo.allocate(m_vertexBufferData.data(), vertexMemSize);

	// index 0 = position
	shaderProgramm->enableAttributeArray(0); // array with index/id 0
	shaderProgramm->setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof(Vertex));
	// index 1 = color
	shaderProgramm->enableAttributeArray(1); // array with index/id 1
	shaderProgramm->setAttributeBuffer(1, GL_FLOAT, offsetof(Vertex, r), 3, sizeof(Vertex));

	m_vao.release();
	m_vbo.release();
}


void AxisObject::destroy() {
	m_vao.destroy();
	m_vbo.destroy();
}


void AxisObject::render() {
	m_vao.bind();
	glDrawArrays(GL_LINES, 0, m_vertexBufferData.size());
	m_vao.release();
}


void AxisObject::setPoints(const QVector3D & a, const QVector3D & b) {
	m_vertexBufferData[0] = Vertex(a, Qt::white);
	m_vertexBufferData[1] = Vertex(b, QColor(64,0,0));
	int vertexMemSize = m_vertexBufferData.size()*sizeof(Vertex);
	m_vbo.bind();
	m_vbo.allocate(m_vertexBufferData.data(), vertexMemSize);
	m_vbo.release();
	m_visible = true;
}

