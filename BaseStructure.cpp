#ifndef BASE_STRUCTURE
#include "BaseStructure.h"

// Phrase definitions
Phrase::Phrase() { 
	name = "";
	time = 0;
	duration = 0;
}	
	
Phrase::Phrase( Meta m, unsigned int i, unsigned int c ) {
	id = i;
	name = m.text;
	time = m.time;
	duration = 0;
	variation = c;
}

// Section definitions
Section::Section() {
	name = "";
	time = 0;
	duration = 0;	
	iteration = 1;
}

Section::Section(Meta m, float d, int i) {
	name = m.text;
	time = m.time;
	duration = d;
	
	iteration = i;
}

#endif