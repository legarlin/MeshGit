#pragma once

#include "MeshComponent.h"
#include "ComponentMatch.h"

class EditOperation {

	void checkConflict();

public:
	ComponentMatch* matchA;
	ComponentMatch* matchB;

	bool conflict;
	bool aChanged;
	bool bChanged;

	EditOperation();
	EditOperation(ComponentMatch* a, ComponentMatch* b);

};