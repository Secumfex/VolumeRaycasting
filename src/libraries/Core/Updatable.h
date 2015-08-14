#ifndef UPDATABLE_H_
#define UPDATABLE_H_

class Updatable {
public:
	Updatable();
	virtual ~Updatable();
	virtual void update(float d_t = 0.1f);
};

#endif
