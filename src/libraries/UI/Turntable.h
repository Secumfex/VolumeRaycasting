#ifndef TURNTABLE_H
#define TURNTABLE_H

#include <glm/glm.hpp>

class Turntable
{
protected:
	glm::mat4 m_rotation;
	bool m_dragActive;
	float m_sensitivity;

public:
	Turntable(float sensitivity = 0.01f);
	virtual ~Turntable();

	inline glm::mat4 getRotationMatrix(){ return m_rotation; }

	void setDragActive(bool drag);
	bool getDragActive();

	void setSensitivity(float sensitivity);

	void dragBy(float phi, float theta, glm::mat4& view = glm::mat4(1.0f));
};


#endif
