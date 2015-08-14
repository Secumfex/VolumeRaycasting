#ifndef TIMER_H
#define TIMER_H

#include <vector>

#include "Updatable.h"

class Timer : public Updatable
{
protected:
	std::vector< double> m_savedTime;	// vector of saved values
	double m_elapsedTime;	// elapsed time since timer started
	bool   m_running;		// true if timer is running
public:
	Timer(bool running);
	~Timer();
	virtual void update(float d_t);	// add d_t to elapsed time
	virtual void reset();			// sets elapsed Time to 0
	virtual void toggleRunning();	// toggles running to true or false

	void clearSavedTimes();	// clears the vector of saved times
	void saveCurrentElapsedTime();	// pushes the current elapsed time into vector

	double getElapsedTime() const;
	double* getElapsedTimePtr();
	bool isRunning() const;
	bool* getRunningPtr();
	void setRunning(bool running);
	const std::vector<double>& getSavedTime() const;
};

class GLFWTimer : public Timer
{
protected:
	double m_lastTime;
public:
	GLFWTimer(bool running);
	~GLFWTimer();
	virtual void update(float d_t);	// read glfwtime and add actual difference to elapsed time if running
	virtual void toggleRunning();
};

#endif
