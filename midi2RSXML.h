#include <cstring>

#include "MIDIRead.cpp"
#include "RSXMLWrite.cpp"

// Flag-based variables
bool palmToggle = false;
bool externalLyrics = false; 
eMidi midiMode = eMidi::normal;