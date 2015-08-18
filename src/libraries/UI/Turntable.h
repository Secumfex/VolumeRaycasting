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
	inline void setDragActive(bool drag){m_dragActive = drag;}
	inline bool getDragActive(){return m_dragActive;}
	inline void setSensitivity(float sensitivity){m_sensitivity = sensitivity;}

	void dragBy(float phi, float theta, glm::mat4& view = glm::mat4(1.0f));
};


#endif
