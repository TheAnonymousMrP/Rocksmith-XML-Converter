#include "MIDIReaderDefault.h"
#include "ARRCreateGuitar.cpp"
#include "ARRCreateVocals.h"
#include "RSXMLCreateGuitar.h"
#include "RSXMLWriter.cpp"

#include <cstring>
#include <string>
#include <vector>

// Flag-based variables
bool palmToggle = false;
bool externalLyrics = false; 

int main(int argc, char* argv[]) {
	/* Argument handling.
		- Argument 1 must always be a filename. It should be in the 
		directory the program is in (probably). The filename should not
		contain a file extension - we're using the name to retrieve both
		the midi file and external lyrics as appropriate.
		- Flags:
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
			- "-title": This flag, and the subsequent argument, explains 
			the song title we wish to use. Use quotation marks if there's
			spaces involved.
			- "-arr": This flag, and the subsequent argument, reveal the 
			number of arrangements we wish to use. Without it, the program
			will read all tracks within the MIDI file. Default is '-1', which
			will, again, read all tracks.
			- "-midi": Indicates we're going to be reading from a MIDI
			file. Not implemented yet. 
			- "-logic" - This flag triggers a few assumptions based on 
			how Logic prepares MIDI files. 
			
	Known bugs:
		- Additional arrangements don't reset the ChordTemplate and Phrase IDs.
		This causes an error when converting to .sng.
	*/
			
	std::string midiName = argv[1]; /* If a filename isn't the first argument, 
	the program's going to fail. Also, don't get the filename wrong. 
	I know jack shit about exceptions and all that. */
	std::string title = "Title"; // Song name.
	
	unsigned int arrN = 0;
	std::string arrTitle = "Arrangement";
	
	for(int i = 2; i < argc; i++) { 
		// Song title.
		if(!strcmp(argv[i],"-title")) 
			{ if(argv[i+1] != NULL) { title = argv[i+1]; } }
		// Number of arrangements.
		if(!strcmp(argv[i],"-arr"))
			{ 
			if(argv[i+1] != NULL) 
				{ std::string arrS = argv[i+1]; arrN = atoi(arrS.c_str()); } 
			}
		// External Lyrics flag.
		if(!strcmp(argv[i],"-extlyrics")) { externalLyrics = 1; }
		// Palm-mute Toggle flag.
		if(!strcmp(argv[i],"-palmtoggle")) { palmToggle = 1; }
		/* Logic flag.
		if(!strcmp(argv[i],"-logic")) { midiMode = eMidi::logic; }
		if(!strcmp(argv[i],"-rb3")) { midiMode = eMidi::rb3; } */
	}
	
	MIDI::ReaderDefault midi( midiName );
	midi.Process( arrN );
	// midi.debug();
	
	auto& tracks = midi.GetTracks();

	RSXML::Writer rsxml( midiName, title );
	for( auto it = tracks.begin(); it != tracks.end(); ++it ) {
		if( tracks.size() > 0 && (it - tracks.begin()) == 0 ) { }
		else if( it->name == "Vocals" || it->type == RSXML::eTrackType::VOCAL ) {
			ARR::CreateVocals a;
			std::string extFileName = "";
			if ( externalLyrics ) { extFileName = midiName; }
			ARR::Vocals v = a.Create( *it, extFileName );
			rsxml.WriteVocals( v );
		}
		else {
			ARR::CreateGuitar ac;
			ARR::Guitar ag = ac.Create( *it );
			RSXML::Guitar rg;
			rsxml.WriteGuitar( rg );
		}
	} 
	
	return 0;
}
	
