#include "Updatable.h"

Updatable::Updatable() {
}

Updatable::~Updatable() {
}

void Updatable::update(float d_t)
{

}

std::function<void(float)> Updatable::getUpdateFct()
{
	return std::bind(
		&Updatable::update,
		this, 
		std::placeholders::_1);
}
