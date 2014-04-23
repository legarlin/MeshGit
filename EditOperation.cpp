#include "EditOperation.h"

EditOperation::EditOperation() 
{
	matchA = nullptr;
	matchB = nullptr;

	conflict = false;
}

EditOperation::EditOperation(ComponentMatch* a, ComponentMatch* b)
{
	matchA = a;
	matchB = b;

	checkConflict();
}

void EditOperation::checkConflict()
{
	conflict = false;
	aChanged = false;
	bChanged = false;

	Match A = matchA->getMatches();
	MeshComponent* A_original = A.originalComp;
	MeshComponent* A_derivative = A.derivativeComp;
	if (A_original->pos != A_derivative->pos) {
		aChanged = true;
	}

	Match B = matchB->getMatches();
	MeshComponent* B_original = B.originalComp;
	MeshComponent* B_derivative = B.derivativeComp;
	if (B_original->pos != B_derivative->pos) {
		bChanged = true;
	}

	if (aChanged && bChanged) {
		conflict = true;
	}

}