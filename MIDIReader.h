#ifndef READ_MIDI
#define READ_MIDI

#ifndef MIDI_TRACK
#include "MIDITrack.h"
#endif

#ifndef DEBUG_STUFF
#include "debug.h"
#endif

#include <iostream>
#include <fstream>
#include <sstream>

#define FILETYPEBYTE 9
#define HEADERLENGTH 14
#define TRACKHEADERLENGTH 8
#define MAXDELTA 4 // VLQ delta times may only encompass 4 bytes
#define MIDICONTROLLENGTH 3 // Length of MIDI Control Events.

/* enum eMidi { NORMAL, LOGIC, }; */

namespace MIDI {
	// Exceptions
	class FileReadError : public std::exception {
			virtual const char* what() const throw() 
				{ return "The file could not be opened."; };
		};

	class Reader {
		public:
			Reader( const std::string& fileName, const float& startOffset = 0.000f, const bool& debug = false ) 
				: fileName( fileName ), currentTempo( DEFAULTTEMPO ), startOffset( startOffset ), debug( debug ) { };
			
			void 						Process( unsigned int numArrs );
		
			const float& 				GetSongLength() const { return songLength; };	 
			const MIDI::Track&			GetTrack(int i) const { return tracks.at(i); };
			const std::vector<MIDI::Track>& GetTracks() const { return tracks; };
	
		protected:
			std::string					fileName;
			float 						songLength;
			unsigned char* 				memblock; 		// Holds the imported file.
			std::vector<MIDI::Track>	tracks; 		// Stores processed data.
			
			float						currentTempo;
			float						startOffset;
			unsigned int				division;

			const bool					debug;
			
			// Copies file to memblock.
			unsigned int 				GetMIDI( std::string midiName ); 
			
			// Reads the file; handles the 'time' component of MIDI messages.
			unsigned int 				ProcessDelta( const unsigned int& beginPoint );
			// All return new iterator position.
			// Handles the event of the MIDI message - determines which process to use.
			unsigned int 				ProcessContent( unsigned int it, float& timer, MIDI::Track& track );
			virtual unsigned int 		ProcessNote( unsigned int it, const float& timer, MIDI::Track& track, unsigned char c = 0 ) { return 0; };
			virtual unsigned int 		ProcessMeta( unsigned int it, float& timer, MIDI::Track& track ) { return 0; };
			virtual unsigned int 		ProcessSysEx( unsigned int it, float& timer, MIDI::Track& track ) { return 0; };	
				
			// Converters and stuff
			float		 				GetCurrentTempo( std::vector<Base::Tempo>::const_iterator& tCount, 
											const std::vector<Base::Tempo>::const_iterator& tEnd, 
											const float& timer ) const;
	};

	void Reader::Process( unsigned int numArrs ) {
		if(fileName == "")
			{ std::cout << "No MIDI file was selected to be read.\n"; }
		else {
			int fileSize = GetMIDI( fileName );
			division = (memblock[12] * 256) + memblock[13]; /* The time division 
			is stored as a short in bytes 12 and 13. This is an inelegant yet 
			accurate solution, code-reusability be damned. */
			// Single-track case.
			if( memblock[FILETYPEBYTE] == 0 ) { ProcessDelta( HEADERLENGTH ); }
			// Multiple tracks.
			else {
				// Global track. Holds information such as tempo and time signature.
				int filePos = ProcessDelta( HEADERLENGTH );
				bool moarTrack = true;
				while( filePos < fileSize && moarTrack ) {
					filePos = ProcessDelta( filePos );
					if( numArrs > 0 && tracks.size() - 1 >= numArrs )
						{ moarTrack = false; } 
				}
			}	
		}
	}
	
	unsigned int Reader::GetMIDI( std::string midiName ) {
		// Taking the content from a MIDI file.
		std::ifstream midi;
		unsigned int midiSize = 0;

		midiName += ".mid";
		try {
			midi.open( midiName.c_str(), std::ios::in | std::ios::binary | std::ios::ate );
			if( midi.is_open() ) {
				midiSize = midi.tellg();
				memblock = new unsigned char[midiSize];
				// creating a (signed) buffer so we can just transfer stuff across.
				char* membuffer = new char[midiSize];
				midi.seekg(0);
				midi.read( membuffer, midiSize );
				// Inelegant transfer of signed to unsigned.

				for( unsigned int i = 0; i < midiSize; ++i )
					{ memblock[i] = membuffer[i]; }

				for( unsigned int i = 0; i < midiSize; ++i ) { memblock[i] = membuffer[i]; }	

			}
			else { throw FileReadError(); } 
			midi.close();
		} catch ( std::exception& e ) { std::cout << e.what() << std::endl; }
		return midiSize;
	}
	
	unsigned int Reader::ProcessDelta( const unsigned int& startPoint ) {
		MIDI::Track track;
		if( tracks.size() > 0 ) { 
			track.SetTempos( tracks.at(0).GetTempos() ); 
			track.SetTimeSigs( tracks.at(0).GetTimeSigs() );
			track.SetMarkers( tracks.at(0).GetMetaStrings( Base::eMeta::MARKER ) );
		}
		
		std::vector<Base::Tempo>::const_iterator tCount = track.GetTempos().begin();
		std::vector<Base::Tempo>::const_iterator tEnd = track.GetTempos().end();
		currentTempo = DEFAULTTEMPO;
		float timer = startOffset;
		
		std::vector<unsigned char> time;
	
		unsigned int trackSize = ( memblock[startPoint + 6] * 256 ) + memblock[startPoint + 7];
		unsigned int iterator = startPoint + TRACKHEADERLENGTH;
		unsigned int endPoint = iterator + trackSize;
				
		/* std::cout << "Track " << tracks.size() << ": Begin: " << beginPoint 
		<< " Track size: " << trackSize << " End: " << endPoint ENDLINE */
		int debugIterator = 0;
		while( iterator < endPoint ) {
			unsigned char c = memblock[iterator]; // Read byte.
			time.push_back( c );		
			// Not the last delta-time byte
			if ( c > 0x7F ) {
				if( time.size() >= MAXDELTA ) { 
					std::cout << "Time Float is too large at: " << iterator << " | " 
					<< c << "\n"; return endPoint; 
				} 
				++iterator;
			}
			// Should be the last delta-time byte
			else if( c < 0x80 ) {
				unsigned int delta = ConvertBytes2VLQ( time );
				if( tracks.size() > 0 ) { currentTempo = GetCurrentTempo( tCount, tEnd, timer ); }
				timer += ConvertDelta2Time( delta, division, currentTempo );
				// std::cout << ++debugIterator << "\tTempo: " << currentTempo << "\tDelta: " << delta << "\tTime: " << timer ENDLINE
				time.clear();
				
				iterator = ProcessContent( iterator, timer, track );
			} 
		}
		if( timer > songLength ) { songLength = timer; }
		track.duration = timer;
		track.NormaliseDifficulties();
		tracks.push_back(track);

		return endPoint;
	}
	
	unsigned int Reader::ProcessContent( unsigned int it, float& timer, MIDI::Track& track ) {
		++it;
		static unsigned char lastEvent;
		unsigned char c = memblock[it]; // Read byte.
		if ( c == 0xFF ) { 
			++it;
			c = memblock[it];
			if( c == 0x2F ) { it += 2; }
			return ProcessMeta( it, timer, track ); 
		} else if( c == 0xF0 || c == 0xF7 ) {
			// SysEx. Not used by us. Because they are variable length, we're
			// kind of fucked until I bother do this shit.
			// processSysEx( it, timer, track );
		} else {
			if ( c >= 0x80 && c < 0xA0 ) {
				// Note-on and -off messages.
				lastEvent = c;
				return ProcessNote( it, timer, track );
			} else if( c >= 0xB0 && c < 0xC0 ) {
				/* Controller events. Don't really know how this will affect
				our charts but they may have implications down the line. */
			} else if( c >= 0xC0 && c < 0xE0 ) {
				// Program change and aftertouch events. Irrelevant to us.
			} else if( c >= 0xE0 && c < 0xF0 ) {
				/* Pitch Bend events. These will be necessary to parse to get
				a direct line to string-bend events. An intermediary for now 
				is using the 'T' text event to specify bends. */
				lastEvent = c;
				return it += MIDICONTROLLENGTH;
			} else if( c < 0x80 ) {
				/* This one requires an explanation. The 4-bit event-type sequence 
				doesn't show up if there is a zero-value delta-time AND the 
				event-type repeats. As such, this throws the otherwise 
				well-established pattern off. Fucking 80's. */
				if( lastEvent >= 0x80 && lastEvent < 0xA0 ) { 
					return ProcessNote( it, timer, track, lastEvent );
				} else if ( lastEvent >= 0xE0 && lastEvent < 0xF0 ) { 
					return it += MIDICONTROLLENGTH - 1;
				}
			}
		} /* else {
			std::cout << "BORK BORK BORK at: " 
			<< it << " | " << c << "\n"; 
		} */
		return it;
	}
	
	float Reader::GetCurrentTempo( std::vector<Base::Tempo>::const_iterator& tCount, 
		const std::vector<Base::Tempo>::const_iterator& tEnd, const float& timer ) const
		{
		float tempo = currentTempo;
		if( tCount != tEnd && timer >= tCount->GetTime() ) { tempo = tCount->GetTempo(); ++tCount; }
		return tempo;
	}
};	

#endif