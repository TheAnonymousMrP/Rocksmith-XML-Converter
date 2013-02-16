#ifndef _MIDI_READ
#include "MIDIRead.h"
#endif

class RB3Read : public BaseRead {
	public:
		/* void debug(); Will generate a file listing the contents of the 
		pulled tracks. For debug purposes. */
	
	private:
		unsigned int 		ProcessNote( unsigned int it, const float& timer, 
									MIDITrack& track, unsigned char c = 0 );
		unsigned int 		ProcessMeta( unsigned int it, float& timer, 
									MIDITrack& track );
		unsigned int 		ProcessSysEx( unsigned int it, const float& timer, 
									MIDITrack& track ) { return 0; };	
};