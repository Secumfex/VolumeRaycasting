#ifndef UNIFORM_H
#define UNIFORM_H

#include <string>
#include <Rendering/ShaderProgram.h>

#include <Core/DebugLog.h>

class Uploadable
{
public:
	virtual void uploadUniform(ShaderProgram* shader);
	virtual ~Uploadable();
};

/**
 * make sure there are only valid uniform types allowed...
 */
template< class T>
class Uniform : public Uploadable
{
public:
	T* p_value;
	std::string m_name;
public:
	Uniform(std::string name, T* valuePtr) {
		p_value = valuePtr;
		m_name = name;
	}

	Uniform(const char* name, T* valuePtr) {
		p_value = valuePtr;
		m_name = std::string( name );
	}

	virtual ~Uniform() {
	}

	void uploadUniform(ShaderProgram* shader) {
		if (p_value != 0) {
			shader->update(*p_value, m_name);
		}
	}
};

#endif
