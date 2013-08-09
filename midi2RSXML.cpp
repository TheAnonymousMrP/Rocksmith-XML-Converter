#include "MIDIReaderDefault.h"
#include "ARRCreateGuitar.h"
#include "ARRCreateVocals.h"
#include "RSXMLCreateGuitar.h"
#include "RSXMLWriter.h"

#ifndef DEBUG_MIDI
#include "MIDIDebug.h"
#endif

#include <cstring>
#include <string>
#include <vector>

// Flag-based variables
bool palmToggle = false;
bool externalLyrics = false; 
bool isDebug = false;

/* Argument handling.
	- Argument 1 must always be a filename. It should be in the 
	directory the program is in (probably). The filename should not
	contain a file extension - we're using the name to retrieve both
	the midi file and external lyrics as appropriate.
	- Flags:
		- "-title": This flag, and the subsequent argument, explains 
		the song title we wish to use. Use quotation marks if there's
		spaces involved.
		- "-arr": This flag, and the subsequent argument, reveal the 
		number of arrangements we wish to use. Without it, the program
		will read all tracks within the MIDI file. Default is '-1', which
		will, again, read all tracks.
		- "-extlyrics": This flag tells the program to ignore MIDI
		lyric events and look for a .txt file (with the same name) for 
		lyrics. These lyrics should be separated into syllables; 
		whitespace is used to delimit syllables.
		- "-palmtoggle": This flag alters the behaviour of the 
		palm-mute text event (TPM). By default, the program will assign 
		the palm-mute flag solely to a note which corresponds to the 
		palm-mute text event. With this flag, the first text event will 
		turn on palm-muting and a second will turn it off. This is 
		useful for songs with a long sequences of palm-muted strumming.
		- "-offset": The subsequent argument will alter the start offset 
		by a number, in seconds. This offset will be added to all events
		in tracks, so it is unnecessary if leading silence is already 
		present in the MIDI file.
		- "-debug": Enables a number of warning/error messages to be 
		written to the console. This can be useful for identifying 
		potential errors in reading/writing/conversion which may produce
		an incompatible .xml file. Not fully implemented.

	Not implemented:
		- "-midi": Indicates we're going to be reading from a MIDI
		file.
		- "-logic" - This flag triggers a few assumptions based on 
		how Logic prepares MIDI files. 
			
	Known bugs:
		- EDIT: This seems to be fixed now. Go me. / Additional arrangements 
		don't reset the ChordTemplate and Phrase IDs. This causes an error 
		when converting to .sng.
*/
int main(int argc, char* argv[]) {
	std::string midiName = argv[1]; /* If a filename isn't the first argument, 
	the program's going to fail. Also, don't get the filename wrong. 
	I know jack shit about exceptions and all that. */
	std::string title = "Title"; // Song name.
	
	unsigned int arrN = 0;
	std::string arrTitle = "Arrangement";
	float startOffset = 0.000f;
	
	for(int i = 2; i < argc; ++i) { 
		// Song title.
		if( !strcmp( argv[i],"-title" ) ) { if( argv[i+1] != NULL ) { title = argv[i+1]; } }
		// Number of arrangements.
		if( !strcmp(argv[i],"-arr") ) { 
			if( argv[i+1] != NULL ) { std::string arrS = argv[i+1]; arrN = atoi(arrS.c_str()); }  
		}
		// Offset time (seconds).
		if( !strcmp( argv[i], "-offset") ) { 
			if( argv[i+1] != NULL ) { 
				std::string offsetS = argv[ i+1 ]; startOffset = atof( offsetS.c_str() );
			} 
		} 
		// External Lyrics flag.
		if( !strcmp(argv[i],"-extlyrics") ) { externalLyrics = true; }
		// Palm-mute Toggle flag.
		if( !strcmp(argv[i],"-palmtoggle") ) { palmToggle = true; }
		// Debug flag.
		if( !strcmp( argv[i], "-debug" ) ) { isDebug = true; }
		/* Logic flag.
		if(!strcmp(argv[i],"-logic")) { midiMode = eMidi::logic; }
		if(!strcmp(argv[i],"-rb3")) { midiMode = eMidi::rb3; } */
	}
	
	MIDI::ReaderDefault midi( midiName, startOffset, isDebug );
	midi.Process( arrN );

	auto& tracks = midi.GetTracks();
	// MIDI::Debug::Tracks( midiName, tracks );

	RSXML::Writer rsxml( midiName, title, isDebug );
	for( auto it = tracks.begin(); it != tracks.end(); ++it ) {
		if( tracks.size() > 0 && (it - tracks.begin()) == 0 ) { }
		else if( it->name == "Vocals" || it->type == RSXML::eTrackType::VOCAL ) {
			ARR::CreateVocals a( isDebug );
			std::string extFileName = "";
			if ( externalLyrics ) { extFileName = midiName; }
			ARR::Vocals v = a.Create( *it, extFileName );
			std::cout << midiName << "-Vocals.xml" << "\n";
			rsxml.WriteVocals( v );
		}
		else {
			std::cout << "Track " << it - tracks.begin() << ": \n";
			ARR::CreateGuitar ac( isDebug );
			ARR::Guitar ag = ac.Create( *it );
			std::cout << midiName << "-" << ag.GetName() << ".xml\n";
			RSXML::CreateGuitar rc( isDebug );
			RSXML::Guitar rg = rc.Create( ag );
			rsxml.WriteGuitar( rg );
		}
	} 
	
	return 0;
}
	
