#include "DebugLog.h"

DebugLog::DebugLog(bool autoPrint)
{
	m_autoPrint = autoPrint;
	m_indent = 0;
}


DebugLog::~DebugLog()
{
	clear();
}


void DebugLog::log(std::string msg)
{
	m_log.push_back( createIndent() +  msg );
	if (m_autoPrint)
	{
		printLast();
	}
}

void DebugLog::log(std::string msg, bool value)
{
	std::stringstream ss;
	if ( value )
	{
		ss<< "TRUE";
	}
	else
	{
		ss<< "FALSE";
	}
	log(msg + ss.str());
}

void DebugLog::log(std::string msg, int value)
{
	std::stringstream ss;
	ss << value;
	log(msg + ss.str());
}

void DebugLog::log(std::string msg, unsigned int value)
{
	std::stringstream ss;
	ss << value;
	log(msg + ss.str());
}

void DebugLog::log(std::string msg, const glm::vec3& vector)
{
	std::stringstream ss;
	ss << vector.x << ", ";
	ss << vector.y << ", ";
	ss << vector.z;
	log(msg + ss.str());
}

void DebugLog::log(std::string msg, const glm::vec4& vector)
{
	std::stringstream ss;
	ss << vector.x << ", ";
	ss << vector.y << ", ";
	ss << vector.z << ", ";
	ss << vector.w;
	log(msg + ss.str());
}

void DebugLog::log(std::string msg, float value)
{
	std::stringstream ss;
	ss << value;
	log(msg + ss.str());
}

void DebugLog::log(std::string msg, double value)
{
	std::stringstream ss;
	ss << value;
	log(msg + ss.str());
}

void DebugLog::indent()
{
	m_indent++;
}

void DebugLog::outdent()
{
	m_indent--;
	if(m_indent < 0)
	{
		m_indent = 0;
	}
}

std::string DebugLog::createIndent() const
{
	std::string indent;
	for ( int j = 0; j < m_indent; j++)
		{
			indent.append( ".." );
		}
	return indent;
}

void DebugLog::print() const{
	for (unsigned int i = 0; i < m_log.size(); i++)
	{
		std::cout << m_log[i] << std::endl;
	}
}

void DebugLog::printLast() const{
	if (!m_log.empty())
	{
		std::cout << m_log.back() << std::endl;
	}
}

void DebugLog::clear()
{
	m_log.clear();
}

void DebugLog::setAutoPrint(bool to)
{
	m_autoPrint = to;
}
