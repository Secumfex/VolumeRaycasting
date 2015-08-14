#ifndef DEBUGLOG_H
#define DEBUGLOG_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <glm/glm.hpp>

#include "Singleton.h"

class DebugLog : public Singleton<DebugLog>
{
friend class Singleton< DebugLog >;
private:
	std::vector< std::string > m_log;
	int  m_indent;
	bool m_autoPrint;
	inline std::string createIndent() const;
public:
	DebugLog(bool autoPrint = false);
	~DebugLog();
	void log(std::string msg);
	void log(std::string msg, bool value);
	void log(std::string msg, int value);
	void log(std::string msg, unsigned int value);
	void log(std::string msg, float value);
	void log(std::string msg, double value);
	void log(std::string msg, const glm::vec3& vector);
	void log(std::string msg, const glm::vec4& vector);
	void indent();
	void outdent();
	void print() const;
	void printLast() const;
	void clear();

	void setAutoPrint(bool to);
};

// for convenient access
#define DEBUGLOG DebugLog::getInstance()

#endif

