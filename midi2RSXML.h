#include <cstring>

#include "MIDIRead.cpp"
#include "RSXMLWrite.cpp"

namespace midi2RSXML {
	const std::vector<Track>& getTracks(std::string name, eMidi m, int arrN) {
		MIDIRead midi(name, m);
		midi.process(arrN);
		// midi.debug();
		return midi.getTracks();
	};
}

// Flag-based variables
bool palmToggle = false;
bool externalLyrics = false; 
eMidi midiMode = eMidi::normal;