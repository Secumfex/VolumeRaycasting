#include "Turntable.h"

#include <glm/gtc/matrix_transform.hpp>

Turntable::Turntable(float sensitivity)
	: m_rotation(1.0f),
	m_dragActive(false),
	m_sensitivity(sensitivity)
{
}

Turntable::~Turntable()
{

}

void Turntable::dragBy(float phi, float theta, glm::mat4& view)
{
	// first: rotate "turn vector" as proposed by view matrix
	glm::mat4 transformMatrix = glm::inverse( view );

	glm::vec3 yRotation = glm::vec3 ( transformMatrix * glm::vec4 ( 0.0, 1.0, 0.0, 0.0) );
	glm::vec3 xRotation = glm::vec3 ( transformMatrix * glm::vec4 ( 1.0, 0.0, 0.0, 0.0) );

	m_rotation = glm::rotate( glm::mat4(1.0f), phi * m_sensitivity, yRotation ) * m_rotation;
	m_rotation = glm::rotate( glm::mat4(1.0f), theta * m_sensitivity, xRotation ) * m_rotation;
}