#include "Smartphone.h"

#include <QVector3D>
#include <QOpenGLShaderProgram>
#include <QElapsedTimer>

#include "BoxMesh.h"
#include "Transform3D.h"

SmartPhone::SmartPhone() :
	m_vbo(QOpenGLBuffer::VertexBuffer), // actually the default, so default constructor would have been enough
	m_ebo(QOpenGLBuffer::IndexBuffer) // make this an Index Buffer
{
	Transform3D trans;

    // create coordinate system boxes
    BoxMesh smartphoneMesh(4, 0.5, 8);
    smartphoneMesh.setFaceColors({Qt::blue, Qt::red, Qt::yellow, Qt::green, Qt::magenta, Qt::darkCyan});
//	smartphoneMesh.setColor(Qt::red);

    //trans.setTranslation(5,0,0);
    trans.setTranslation(43, 97, -137);


    trans.rotate(-20, QVector3D(1.0, 0.0, 0.0));
    trans.rotate(-50, QVector3D(0.0, 1.0, 0.0));

    smartphoneMesh.transform(trans.toMatrix()); // inplace-modification


	// resize storage arrays
    m_vertexBufferData.resize(BoxMesh::VertexCount);
    m_elementBufferData.resize(BoxMesh::IndexCount);

	// update the buffers
	VertexVNC * vertexBuffer = m_vertexBufferData.data();
	unsigned int vertexCount = 0;
	GLuint * elementBuffer = m_elementBufferData.data();

    smartphoneMesh.copy2Buffer(vertexBuffer, elementBuffer, vertexCount);
}


void SmartPhone::create(QOpenGLShaderProgram * shaderProgramm) {
	// create and bind Vertex Array Object
	m_vao.create();
	m_vao.bind();

	// create and bind vertex buffer
	m_vbo.create();
	m_vbo.bind();
	m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	int vertexMemSize = m_vertexBufferData.size()*sizeof(VertexVNC);
    qDebug() << "SmartPhone - VertexBuffer size =" << vertexMemSize/1024.0 << "kByte";
	m_vbo.allocate(m_vertexBufferData.data(), vertexMemSize);

	// create and bind element buffer
	m_ebo.create();
	m_ebo.bind();
	m_ebo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	int elementMemSize = m_elementBufferData.size()*sizeof(GLuint);
    qDebug() << "SmartPhone - ElementBuffer size =" << elementMemSize/1024.0 << "kByte";
	m_ebo.allocate(m_elementBufferData.data(), elementMemSize);

	// set shader attributes
	// tell shader program we have two data arrays to be used as input to the shaders

	// index 0 = position
	shaderProgramm->enableAttributeArray(0); // array with index/id 0
	shaderProgramm->setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof(VertexVNC));
	// index 1 = normal
	shaderProgramm->enableAttributeArray(1); // array with index/id 1
	shaderProgramm->setAttributeBuffer(1, GL_FLOAT, offsetof(VertexVNC, m), 3, sizeof(VertexVNC));
	// index 2 = color
	shaderProgramm->enableAttributeArray(2); // array with index/id 2
	shaderProgramm->setAttributeBuffer(2, GL_FLOAT, offsetof(VertexVNC, r), 3, sizeof(VertexVNC));

	// Release (unbind) all
	m_vao.release();
	m_vbo.release();
	m_ebo.release();
}


void SmartPhone::destroy() {
	m_vao.destroy();
	m_vbo.destroy();
	m_ebo.destroy();
}


void SmartPhone::render() {
	// set the geometry ("position", "normal" and "color" arrays)
	m_vao.bind();

	// now draw the cube by drawing individual triangles
	// - GL_TRIANGLES - draw individual triangles via elements
	glDrawElements(GL_TRIANGLES, m_elementBufferData.size(), GL_UNSIGNED_INT, nullptr);
	// release vertices again
	m_vao.release();
}
