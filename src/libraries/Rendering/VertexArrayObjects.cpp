#include "VertexArrayObjects.h"

#include "Core/DebugLog.h"

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

#include <glm/gtc/constants.hpp>

glm::vec3 Sphere::sampleSurface(float u, float v)
{
    glm::vec3 result;
    
    result.x = std::cos(u) * std::cos(v);
    result.y = std::sin(u) * std::cos(v);
    result.z = std::sin(v);
    
    return result;
}

Sphere::Sphere(unsigned int hSlices, unsigned int vSlices, float radius)
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    GLuint vertexBufferHandles[3];
    glGenBuffers(3, vertexBufferHandles);
    
    m_positions.m_vboHandle = vertexBufferHandles[0];
    m_uvs.m_vboHandle       = vertexBufferHandles[1];
    m_normals.m_vboHandle   = vertexBufferHandles[2];

    m_mode = GL_TRIANGLES;
    
    std::vector<float> positions;
    std::vector<float> uv;
    std::vector<float> normals;
    positions.resize(hSlices * vSlices * 3 * 3 * 2);
    uv.resize(hSlices * vSlices * 3 * 2 * 2);
    normals.resize(hSlices * vSlices * 3 * 3 * 2);

    m_positions.m_size = positions.size() / 3;
    m_uvs.m_size = uv.size() / 2;
    m_normals.m_size = normals.size() / 3;


    float d_u = (2.0f * glm::pi<float>()) / ((float) vSlices);
    float d_v = glm::pi<float>() / ((float) hSlices);

    // parametric surface description
    unsigned int i = 0;
    for (float u = - glm::pi<float>(); u <= glm::pi<float>(); u += d_u)
    {
        for ( float v = -glm::half_pi<float>(); v <= glm::half_pi<float>(); v+= d_v)
        {
            //////////////// first triangle
            // first point
            glm::vec3 point = sampleSurface(u,v);
            positions[3 * i + 0] = point.x * radius;
            positions[3 * i + 1] = point.y * radius;
            positions[3 * i + 2] = point.z * radius;
            uv[2 * i + 0] = (point.x + 1.0f) * 0.5f;
            uv[2 * i + 1] = (point.z + 1.0f) * 0.5f;
            normals[3 * i + 0] = point.x;
            normals[3 * i + 1] = point.y;
            normals[3 * i + 2] = point.z;
            i++;

            // second point
            point = sampleSurface(u,v + d_v);
            positions[3 * i + 0] = point.x * radius;
            positions[3 * i + 1] = point.y * radius;
            positions[3 * i + 2] = point.z * radius;
            uv[2 * i + 0] = (point.x + 1.0f) * 0.5f;
            uv[2 * i + 1] = (point.z + 1.0f) * 0.5f;
            normals[3 * i + 0] = point.x;
            normals[3 * i + 1] = point.y;
            normals[3 * i + 2] = point.z;
            i++;

            // third point
            point = sampleSurface(u + d_u, v + d_v);
            positions[3 * i + 0] = point.x * radius;
            positions[3 * i + 1] = point.y * radius;
            positions[3 * i + 2] = point.z * radius;
            uv[2 * i + 0] = (point.x + 1.0f) * 0.5f;
            uv[2 * i + 1] = (point.z + 1.0f) * 0.5f;
            normals[3 * i + 0] = point.x;
            normals[3 * i + 1] = point.y;
            normals[3 * i + 2] = point.z;
            i++;

            //////////////// second triangle
            // first point
            point = sampleSurface(u + d_u, v + d_v);
            positions[3 * i + 0] = point.x * radius;
            positions[3 * i + 1] = point.y * radius;
            positions[3 * i + 2] = point.z * radius;
            uv[2 * i + 0] = (point.x + 1.0f) * 0.5f;
            uv[2 * i + 1] = (point.z + 1.0f) * 0.5f;
            normals[3 * i + 0] = point.x;
            normals[3 * i + 1] = point.y;
            normals[3 * i + 2] = point.z;
            i++;

            // second point
            point = sampleSurface(u + d_u, v);
            positions[3 * i + 0] = point.x * radius;
            positions[3 * i + 1] = point.y * radius;
            positions[3 * i + 2] = point.z * radius;
            uv[2 * i + 0] = (point.x + 1.0f) * 0.5f;
            uv[2 * i + 1] = (point.z + 1.0f) * 0.5f;
            normals[3 * i + 0] = point.x;
            normals[3 * i + 1] = point.y;
            normals[3 * i + 2] = point.z;
            i++;

            // third point
            point = sampleSurface(u, v);
            positions[3 * i + 0] = point.x * radius;
            positions[3 * i + 1] = point.y * radius;
            positions[3 * i + 2] = point.z * radius;
            uv[2 * i + 0] = (point.x + 1.0f) * 0.5f;
            uv[2 * i + 1] = (point.z + 1.0f) * 0.5f;
            normals[3 * i + 0] = point.x;
            normals[3 * i + 1] = point.y;
            normals[3 * i + 2] = point.z;
            i++;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandles[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * positions.size(), &positions[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandles[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * uv.size(), &uv[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandles[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals.size(), &normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);
}

Sphere::~Sphere()
{
    glDeleteBuffersARB(1, &(m_positions.m_vboHandle));
    glDeleteBuffersARB(1, &(m_uvs.m_vboHandle));
    glDeleteBuffersARB(1, &(m_normals.m_vboHandle));
}

void Sphere::draw()
{
    glBindVertexArray(m_vao);
    glDrawArrays(m_mode, 0, m_positions.m_size);
}

Grid::Grid(unsigned int fieldsX, unsigned  int fieldsY, float sizeX, float sizeY, bool centered)
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    GLuint positionBuffer;
    GLuint uvBuffer;
    GLuint normalBuffer;
    GLuint indexBuffer;
    glGenBuffers(1, &positionBuffer);
    glGenBuffers(1, &uvBuffer);
    glGenBuffers(1, &normalBuffer);
    glGenBuffers(1, &indexBuffer);

    m_positions.m_vboHandle = positionBuffer;
    m_normals.m_vboHandle = normalBuffer;
    m_uvs.m_vboHandle = uvBuffer;
    m_indices.m_vboHandle = indexBuffer;

    m_mode = GL_TRIANGLE_STRIP;
    

    std::vector<float> positions( ((fieldsX+1) * (fieldsY+1))*3, 0.0f );
    std::vector<float> normals( ((fieldsX+1) * (fieldsY+1))*3, 0.0f );
    std::vector<float> uv( ((fieldsX+1) * (fieldsY+1))*2, 0.0f );

    int posIdx = 0;
    int uvIdx = 0;

    float centerOffsetX = 0.0f;
    float centerOffsetY = 0.0f;

    float x = 0.0f;
    float y = 0.0f;
    float u = 0.0f;
    float v = 0.0f; 

    float dx = sizeX;
    float dy = sizeY;
    float du = 1.0f/((float)(fieldsX));
    float dv = 1.0f/((float)(fieldsY));

    if (centered)
    {
        centerOffsetX = - ( (float) fieldsX / 2.0f) * dx;
        centerOffsetY = - ( (float) fieldsY / 2.0f) * dy;
    }

    for (int iY = 0; iY <= fieldsY; iY++)
    {
        x = 0.0f;
        u = 0.0f;
        for (int iX = 0; iX <= fieldsX; iX++)
        {

            positions[posIdx + 0] = x + centerOffsetX;
            positions[posIdx + 1] = y + centerOffsetY;

            // DEBUGLOG->log("idx : ", posIdx);
            // DEBUGLOG->log("posX: ", x);
            // DEBUGLOG->log("posY: ", y);

            normals[posIdx + 2] = 1.0f;

            posIdx += 3;


            uv[uvIdx + 0] = u;
            uv[uvIdx + 1] = v;
            uvIdx += 2;
                        
            x += dx;
            u += du;
        }

        v +=dv;
        y += dy;
    }

    m_positions.m_size = positions.size() / 3;
    m_uvs.m_size = uv.size() / 2;
    m_normals.m_size = normals.size() / 3;

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*positions.size(), &positions[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * uv.size(), &uv[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*normals.size(), &normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // Index Buffer / Element Array Buffer
    std::vector<unsigned int> indices(((fieldsX+1)*2)*(fieldsY),0);

    int top = 0;
    int bottom = fieldsX+1;
    for (int i = 0; i < indices.size(); i++)
    {
        if (i%2 == 0) // even index: top vertex
        {
            indices[i] = top;
            top++;
        } 
        else{   //odd index: bottom vertex
            indices[i] = bottom;
            bottom++;
        }

        // DEBUGLOG->log("index: ", indices[i]);
        // DEBUGLOG->indent();        
        // DEBUGLOG->log("x: ", positions[indices[i]*3]);
        // DEBUGLOG->log("y: ", positions[indices[i]*3+1]);
        // DEBUGLOG->log("z: ", positions[indices[i]*3+2]);
        // DEBUGLOG->outdent();
    }
    // DEBUGLOG->log("indices: ", indices.size());

    m_indices.m_size = indices.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), &indices[0], GL_STATIC_DRAW);
}

Grid::~Grid()
{
    glDeleteBuffersARB(1, &(m_positions.m_vboHandle));
    glDeleteBuffersARB(1, &(m_uvs.m_vboHandle));
    glDeleteBuffersARB(1, &(m_normals.m_vboHandle));
}

void Grid::draw()
{
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices.m_vboHandle);
    glDrawElements(GL_TRIANGLE_STRIP, m_indices.m_size, GL_UNSIGNED_INT, 0);
    // glDrawArrays(GL_POINTS,  0, m_positions.m_size);
}