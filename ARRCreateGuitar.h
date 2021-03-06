#ifndef ARR_CREATE_GUITAR
#define ARR_CREATE_GUITAR

#ifndef ARR_GUITAR
#include "ARRGuitar.h"
#endif

#ifndef MIDI_TRACK
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
			CreateGuitar( const bool& debug = false ) : debug( debug ) { };
			
			const ARR::Guitar 				Create( const MIDI::Track& t );
			
		private:
			const bool						debug;

			MIDI::Track						track;
		
			void 							ConvertSpecialMetas( ARR::Guitar& g ) const;
			std::vector<ARR::Note>			ConvertMIDI2ARRNotes( const std::vector<MIDI::Note>& notes, const Base::Tuning& tuning ) const;
			void 							SetTechniques( std::vector<ARR::Note>& notes, const bool& isBass = false ) const;
			const ARR::Difficulty			CreateDifficulty( const unsigned int& dif, const std::vector<ARR::Note>& notes, std::vector<ARR::Chord>& chords ) const;
			const ARR::Chord				CreateChord( const std::vector<ARR::Note> notes, const std::vector<ARR::Chord>& chords,
												const unsigned int& noteIt, const unsigned char& chordSize ) const;
			const std::vector<ARR::Phrase> 	CreatePhrases( const std::vector<Base::MetaString>& source ) const;
			const std::vector<ARR::Section> CreateSections( const std::vector<Base::MetaString>& source ) const;
			
			// Process Methods
			void 							createAnchors();
			Difficulty 						createDifficulty(unsigned int i, std::ostream& file);
			Chord 							createChord(const std::vector<Note>& nSource, std::vector<Note>::iterator it, int chordSize);
			Chord 							createChord2( const std::vector<Note>& notes, const std::vector<unsigned int>& indexes );
	};
};	
	
#endif