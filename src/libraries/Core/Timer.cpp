#include "Timer.h"

Timer::~Timer() {
}

void Timer::update(float d_t) {
	if ( m_running )
	{
		m_elapsedTime += d_t;
	}
}

void Timer::reset() {
	m_elapsedTime = 0.0;
}

void Timer::toggleRunning() {
	m_running = !m_running;
}

Timer::Timer(bool running) {
	m_elapsedTime = 0.0;
	m_running = running;
}

void Timer::clearSavedTimes() {
	m_savedTime.clear();
}

double Timer::getElapsedTime() const {
	return m_elapsedTime;
}

bool Timer::isRunning() const {
	return m_running;
}

void Timer::setRunning(bool running) {
	m_running = running;
}

double* Timer::getElapsedTimePtr() {
	return &m_elapsedTime;
}

bool* Timer::getRunningPtr() {
	return &m_running;
}

const std::vector<double>& Timer::getSavedTime() const {
	return m_savedTime;
}

void Timer::saveCurrentElapsedTime() {
	m_savedTime.push_back( m_elapsedTime );
}

GLFWTimer::GLFWTimer(bool running)
	: Timer( running )
{
	m_lastTime = - 1.0;
}

GLFWTimer::~GLFWTimer() {
}

#include <GL/glew.h>
#include <GLFW/glfw3.h>

void GLFWTimer::update(float d_t) {
	if ( m_running )
	{
		if ( m_lastTime != -1.0)
		{
			m_elapsedTime += glfwGetTime() - m_lastTime;
		}
		m_lastTime = glfwGetTime();
	}
}

void GLFWTimer::toggleRunning() {
	Timer::toggleRunning();

	(m_running) ? m_lastTime = glfwGetTime() : m_lastTime = -1.0;

}
