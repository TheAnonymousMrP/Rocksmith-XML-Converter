#ifndef DEBUG_MIDI
#define DEBUG_MIDI

#ifndef DEBUG_STUFF
#include "debug.h"
#endif

#ifndef MIDI_TRACK
#include "MIDITrack.h"
#endif

#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

namespace MIDI {
	namespace Debug {
		void Tracks( std::string fileName, const std::vector<MIDI::Track>& tracks ) {
			std::string name = fileName + "-test.txt";
			std::ofstream file;
			file.open(name.c_str()); // This is the file we're writing to.
	
			file << "Test File for " << fileName << ".\n" ENDLINE
	
			for(auto it = tracks.begin(); it != tracks.end(); ++it) {
				file << "Track " << (it - tracks.begin()) << ": Name: "
				<< it->name << " | Duration: " << it->duration ENDLINE
				auto& notes = it->GetNotes();
				// Should print all notes in a track.
				unsigned int failCount = 0;
				for( auto jt = notes.begin(); jt != notes.end(); ++jt ) {
					file << "\tNote " << jt - notes.begin() << "  Time: " 
					<< jt->GetTime() << " | String: " << (int)jt->GetString() ENDLINE
					if( jt->GetString() > 79 ) { ++failCount; }
				}
				file << "\nFail Count: " << failCount ENDLINE
				file << "\nMetadata:" ENDLINE
				file << "\nTempo Changes:" ENDLINE
				auto tempo = it->GetTempos();
				for( Tempo& t : tempo ) {
					file << "\tTime: " << t.GetTime() << " | Tempo: " << t.GetTempo() ENDLINE
				}
				file << "Techniques: " ENDLINE
				auto mTech = it->GetMetaStrings( eMeta::TECHNIQUE );
				for( MetaString& m : mTech) 
					{ file << "Technique: " << m.GetTime() << " | " << m.GetString() ENDLINE }
		
				/* file << "Phrases: " ENDLINE
				auto mPhrase = it->getMetas(eMeta::phrase);
				for(Meta& m : mPhrase) {
					file << "Phrase  Time: " << m.time << " | Name: " 
					<< m.text ENDLINE
				} */
				file << "END OF TRACK. \n" ENDLINE
			}
			file.close();
		};
	}
}

#endif