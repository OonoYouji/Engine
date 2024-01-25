#pragma once

// Base -----
#include "ITransition.h"

#include <cmath>

class NormalTransition final
	: public ITransition {
public: // member method

	// Constructor & Destructor
	NormalTransition();
	~NormalTransition();

	// default method
	void Init() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

private: // member object

	uint32_t color_;

};

