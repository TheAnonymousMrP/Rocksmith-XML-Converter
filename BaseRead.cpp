#ifndef BASE_READ
#include "BaseRead.h"
#endif

BaseRead::MIDIRead(std::string s, eMidi m) { 
	fileName = s; 
	if(m == eMidi::logic) { midiMode = eMidi::logic; } 
	else { midiMode = eMidi::normal; }
	currentTempo = DEFAULTTEMPO;
}