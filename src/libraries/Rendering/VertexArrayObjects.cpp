#include "VertexArrayObjects.h"

Renderable::Renderable()
{
}

Renderable::~Renderable()
{
    std::vector<GLuint> buffers;
    buffers.push_back(m_indices.m_vboHandle);
    buffers.push_back(m_positions.m_vboHandle);
    buffers.push_back(m_normals.m_vboHandle);
    buffers.push_back(m_uvs.m_vboHandle);

    glDeleteBuffersARB(buffers.size(), &buffers[0]);
}

GLuint Renderable::createVbo(std::vector<float> content, GLuint dimensions, GLuint vertexAttributePointer)
{

    GLuint vbo = 0;

	if ( content.size() != 0 )// && content.size() % dimensions == 0 )
	{
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, content.size() * sizeof(float), &content[0], GL_STATIC_DRAW);
        glVertexAttribPointer(vertexAttributePointer, dimensions, GL_FLOAT, 0, 0, 0);
		glEnableVertexAttribArray(vertexAttributePointer);
	}

    return vbo;
}

void Renderable::draw()
{
    bind();
    glDrawElements(m_mode, m_indices.m_size, GL_UNSIGNED_INT, 0);
    unbind();
}

GLuint Renderable::createIndexVbo(std::vector<unsigned int> content, GLuint vertexAttributePointer) 
{
    
	GLuint vbo = 0;
	
	glGenBuffers(1, &vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, content.size() * sizeof(unsigned int), &content[0], GL_STATIC_DRAW);

	return vbo;
}

void Renderable::bind()
{
    glBindVertexArray(m_vao);
}

void Renderable::unbind()
{
    glBindVertexArray(0);
}

unsigned int Renderable::getVertexCount()
{
    return m_positions.m_size;
}

unsigned int Renderable::getIndexCount()
{
    return m_indices.m_size;
}

void Renderable::setDrawMode(GLenum type)
{
    m_mode = type;
}

void Volume::generateBuffers(float size_x, float size_y, float size_z)
{
	m_mode = GL_TRIANGLES;

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    GLuint vertexBufferHandles[3];
    glGenBuffers(3, vertexBufferHandles);

	m_positions.m_vboHandle = vertexBufferHandles[0];
	m_uvs.m_vboHandle = vertexBufferHandles[1];
	m_normals.m_vboHandle = vertexBufferHandles[2];

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandles[0]);
    float positions[] = {
                // Front Face
                 size_x, size_y,size_z,
                 size_x,-size_y,size_z,
                -size_x,-size_y,size_z,

                  -size_x,-size_y,size_z,
                 -size_x, size_y,size_z,
		        size_x,  size_y,size_z,
		        // Right face
                  size_x, size_y,-size_z,
                 size_x,-size_y,-size_z,
		        size_x,-size_y, size_z,

                  size_x,-size_y, size_z,
                 size_x, size_y, size_z,
		        size_x, size_y,-size_z,
		        // Back face
                   size_x,-size_y,-size_z,
                  size_x, size_y,-size_z,
		        -size_x,-size_y,-size_z,

                   -size_x, size_y,-size_z, 
                  -size_x,-size_y,-size_z,
		         size_x, size_y,-size_z,
		        // Left face
                  -size_x,-size_y,-size_z, 
                 -size_x, size_y,-size_z,
		        -size_x,-size_y, size_z,

                  -size_x, size_y, size_z,
                 -size_x,-size_y, size_z,
		        -size_x, size_y,-size_z,
		        // Bottom face
                  size_x,-size_y, size_z,
                 size_x,-size_y,-size_z,
		        -size_x,-size_y, size_z,

                   -size_x,-size_y,-size_z, 
                  -size_x,-size_y, size_z,
		         size_x,-size_y,-size_z,
		        // Top Face
                    size_x,size_y,-size_z,
                  size_x,size_y, size_z,
		        -size_x,size_y, size_z,

                   -size_x,size_y, size_z,
                  -size_x,size_y,-size_z,
		         size_x,size_y,-size_z,
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    m_positions.m_size = sizeof(positions);

    GLfloat uvCoordinates[] = {
        // Front face
        1,0,1,
        1,0,0,
        0,0,0,

        0,0,0,
        0,0,1,
        1,0,1,
        // Right face
        1,1,1,
        1,1,0, 
        1,0,0, 

        1,0,0,
        1,0,1, 
        1,1,1, 
        // Back face
        1,1,0,
        1,1,1, 
        0,1,0, 

        0,1,1,
        0,1,0, 
        1,1,1,
        // Left face
        0,1,0,
        0,1,1, 
        0,0,0, 

        0,0,1,
        0,0,0, 
        0,1,1, 
        // Bottom face
        1,0,0,
        1,1,0, 
        0,0,0, 

        0,1,0, 
        0,0,0,
        1,1,0, 
        // Top face
        1,1,1,
        1,0,1, 
        0,0,1, 

        0,0,1,
        0,1,1, 
        1,1,1 
    };
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandles[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uvCoordinates), uvCoordinates, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    m_uvs.m_size = sizeof(uvCoordinates);

	GLfloat normals[] = {
        // Front face
		0.0f, 0.0f, 1.0f, 
		0.0f, 0.0f, 1.0f, 
		0.0f, 0.0f, 1.0f, 
		
		0.0f, 0.0f, 1.0f, 
		0.0f, 0.0f, 1.0f, 
		0.0f, 0.0f, 1.0f, 
        // Right face
		1.0f, 0.0f, 0.0f, 
        1.0f, 0.0f, 0.0f, 
		1.0f, 0.0f, 0.0f, 

		1.0f, 0.0f, 0.0f, 
        1.0f, 0.0f, 0.0f, 
		1.0f, 0.0f, 0.0f, 

		// Back face
		0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f, 
		0.0f, 0.0f, -1.0f, 

		0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f, 
		0.0f, 0.0f, -1.0f, 
		// Left face
		-1.0f, 0.0f, 0.0f, 
        -1.0f, 0.0f, 0.0f, 
		-1.0f, 0.0f, 0.0f, 

		-1.0f, 0.0f, 0.0f, 
        -1.0f, 0.0f, 0.0f, 
		-1.0f, 0.0f, 0.0f, 
        // Bottom face
		0.0f, -1.0f, 0.0f, 
		0.0f, -1.0f, 0.0f, 
		0.0f, -1.0f, 0.0f, 
		
		0.0f, -1.0f, 0.0f, 
		0.0f, -1.0f, 0.0f, 
		0.0f, -1.0f, 0.0f, 
		// Top face
		0.0f, 1.0f, 0.0f, 
		0.0f, 1.0f, 0.0f, 
		0.0f, 1.0f, 0.0f, 
		
		0.0f, 1.0f, 0.0f, 
		0.0f, 1.0f, 0.0f, 
		0.0f, 1.0f, 0.0f, 
    };
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandles[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);
    m_normals.m_size = sizeof(normals);
}

Volume::Volume(float size)
{
	generateBuffers(size, size, size);
}

Volume::Volume(float size_x, float size_y, float size_z)
{
	generateBuffers(size_x, size_y, size_z);
}

Volume::~Volume()
{
	std::vector<GLuint> buffers;
	buffers.push_back(m_positions.m_vboHandle);
	buffers.push_back(m_uvs.m_vboHandle);
	buffers.push_back(m_normals.m_vboHandle);

	glDeleteBuffersARB(3, &buffers[0]);
}

void Volume::draw()
{
    glBindVertexArray(m_vao);
    glDrawArrays(m_mode, 0, 36);
}

Quad::Quad()
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    GLuint positionBuffer;
    glGenBuffers(1, &positionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);

    m_positions.m_vboHandle = positionBuffer;

    m_mode = GL_TRIANGLE_STRIP;
    
    float positions[] = 
    {
        -1.0f, -1.0f,
        -1.0f, 1.0f,
        1.0f, -1.0f,
        1.0f, 1.0f
    };

    float uv[] = 
    {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*8, positions, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*8, uv, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
}

Quad::~Quad()
{
    glDeleteBuffersARB(1, &(m_positions.m_vboHandle));
    glDeleteBuffersARB(1, &(m_uvs.m_vboHandle));
}

void Quad::draw()
{
    glBindVertexArray(m_vao);
    glDrawArrays(m_mode, 0, 4);
}
