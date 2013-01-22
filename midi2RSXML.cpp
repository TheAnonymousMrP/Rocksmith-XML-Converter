#include "midi2RSXML.h"
using namespace std;

int main(int argc, char* argv[])
	{
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
			how Logic prepares MIDI files. */
			
	std::string midiName = argv[1]; /* If a filename isn't the first argument, 
	the program's going to fail. Also, don't get the filename wrong. 
	I know jack shit about exceptions and all that. */
	std::string title = "Title"; // Song name.
	
	int arrN = -1;
	std::string arrTitle = "Arrangement";
	
	std::string midiMode = "";
	for(int i = 2; i < argc; i++)
		{ 
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
		// Logic flag.
		if(!strcmp(argv[i],"-logic")) { midiMode = "logic"; }
		}
		
	MIDIRead midi;
	midi.setFileName(midiName);
	
	midi.process(arrN);
	midi.debug(midiName);
	
	for(std::vector<Track>::iterator it = midi.getTracks().begin();
		it != midi.getTracks().end(); ++it)
		{
		Track& cT(*it);
		if(cT.getName() == "Vocals" || cT.getType() == vocal)
			{
			ArrVocal v;
			if(externalLyrics)
				{ ArrVocal v(cT, midiName); }
			else { ArrVocal v(cT); }
			RSXMLWrite rsxml(midiName, v);
			rsxml.processVocals();
			}
		else
			{
			CreateArrangement create(cT);
			RSXMLWrite rsxml(midiName, title, create.getArrangement());
			rsxml.processArrangement();
			}
		}
	
	return 0;
	}
	
