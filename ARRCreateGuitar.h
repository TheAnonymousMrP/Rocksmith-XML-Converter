#ifndef _ARR_CREATE_GUITAR_
#define _ARR_CREATE_GUITAR_

#ifndef _ARR_GUITAR_
#include "ARRGuitar.h"
#endif

#ifndef _MIDI_TRACK
#include "MIDITrack.h"
#endif

#include <array>
#include <string>
#include <vector>

/* The intent here is to take all the 'raw' information of a track and create an 
arrangement ready to be written to the appropriate file (ie, a Rocksmith XML). */

namespace ARR {
	class CreateGuitar {
		public:
			CreateGuitar() { };
			
			const ARR::Guitar 	Create( const MIDI::Track& t, const bool& bass = false );
			
		private:
			MIDI::Track				track;
		
			void 					HandleSpecialMetas( ARR::Guitar& g );
			std::vector<ARR::Note>	ConvertMIDI2ARRNotes( std::vector<MIDI::Note> notes );
			void 					SetTechniques( std::vector<ARR::Note>& notes );
			const ARR::Difficulty	CreateDifficulty( const unsigned int& dif, 
										const std::vector<ARR::Note>& notes,
										std::vector<ARR::Chord>& chords );
			const ARR::Chord		CreateChord( const std::vector<ARR::Note> notes, 
										const unsigned int& noteIt, 
										const unsigned int& chordSize );
			
			// Process Methods
			void 					createAnchors();
			Difficulty 				createDifficulty(unsigned int i, std::ostream& file);
				Chord 				createChord(const std::vector<Note>& nSource, 
										std::vector<Note>::iterator it, int chordSize);
				Chord 				createChord2( const std::vector<Note>& notes, 
										const std::vector<unsigned int>& indexes );
		
			void 					createPhrases();
			void 					createSections();	
	};
};	
	
#endif