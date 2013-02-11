#ifndef _BASE_READ_
#define _BASE_READ_

#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

#ifndef _BASE_TRACK
#include "BaseTrack.h"
#endif

#define FILETYPEBYTE 9
#define HEADERLENGTH 14
#define TRACKHEADERLENGTH 8
#define MIDICONTROLLENGTH 3 // Length of MIDI Control Events.
#define DEFAULTTEMPO 120

class BaseRead {
	
	unsigned char* memblock; // Holds the imported file.
	float endTime;
	std::vector<Track> tracks; // stores processed data.
	
	// Copies file to memblock.
	unsigned int getMIDI( std::string midiName ); 
	
	// Reads the file; handles the 'time' component of MIDI messages.
	unsigned int processDelta( const unsigned int& beginPoint );
	// All return new iterator position.
	// Handles the event of the MIDI message - determines which process to use.
	unsigned int processContent( const unsigned int& it, float& timer, Track& track );
	unsigned int processNote( const unsigned int& it, float& timer, Track& track ) 
		{ /* Overload me! */ };
	unsigned int processMeta( const unsigned int& it, float& timer, Track& track ) 
		{ /* Overload me! */ };
	unsigned int processSysEx( const unsigned int& it, float& timer, Track& track ) 
		{ /* Overload me! */ };
	
	
	public:
	
		const float& getEndTime() const { return endTime; };	 
		const Track& getTrack(int i) const { return tracks.at(i); };
		const std::vector<Track>& getTracks() const { return tracks; };

};

unsigned int MIDIRead::getMIDI( std::string midiName ) {
	// Taking the content from a MIDI file.
	std::ifstream midi;
	std::ifstream::pos_type midiSize;
	midiName += ".mid";
	midi.open(midiName.c_str(), 
		std::ios::in | std::ios::binary | std::ios::ate);
	if(midi.is_open())
		{
		midiSize = midi.tellg();;
		memblock = new unsigned char[midiSize];
		// creating a (signed) buffer so we can just transfer stuff across.
		char* membuffer = new char[midiSize];
		midi.seekg(0);
		midi.read(membuffer, midiSize);
		// Inelegant transfer of signed to unsigned.
		for(int i = 0; i < midiSize; i++)
			{ memblock[i] = membuffer[i]; }
		}
	else { std::cout << "File could not be opened." ENDLINE } 
	midi.close();
	
	return midiSize;
}	

unsigned int MIDIRead::processDelta( const unsigned int& beginPoint ) {
	Track nT;
	
	if(tracks.size() > 0) { 
		nT.setTempos( tracks.at(0).getTempos() ); 
		nT.setMetas( tracks.at(0).getMetas( eMeta::marker ),eMeta::marker );
	}
	std::vector<Tempo>::const_iterator tCount = nT.getTempos().begin();
	std::vector<Tempo>::const_iterator tEnd = nT.getTempos().end();
	currentTempo = 120.0;
	float timer = DEFAULTOFFSET;
	
	std::vector<unsigned char> time;

	unsigned int trackSize = ( memblock[beginPoint + 6] * 256 );
	trackSize += memblock[beginPoint + 7];
	unsigned int it = beginPoint + TRACKHEADERLENGTH;
	unsigned int endPoint = it + trackSize;
	
	/* std::cout << "Track " << tracks.size() << ": Begin: " << beginPoint 
	<< " Track size: " << trackSize << " End: " << endPoint ENDLINE */
	while( it < endPoint ) {
		// Unsigned chars are necessary for the hex-based comparisons work.
		unsigned char c = memblock[it]; // Read byte.
		time.push_back( c );		
		// Not the last delta-time byte
		if ( c > 0x7F ) {
			if( time.size() >= MAXDELTA ) { 
				// || convertBytes2VLQ2(time) > 0x0FFFFFFF || )  
				std::cout << "Time Float is too large at: " << it << " | " 
				<< c << "\n"; return endPoint; 
			} 
			++it;
		}
		// 'Should' be the last delta-time byte
		else if( c < 0x80 ) {
			int delta = convertBytes2VLQ( time );
			if( tracks.size() > 0 ) { 
				currentTempo = getCurrentTempo( tCount, tEnd, timer );
			}
			float deltaConv = convertDelta2Time( delta, currentTempo );
			timer += deltaConv;
			time.clear();
			
			processContent( it, timer, nT );
			} 
		}
	if( timer > endTime ) { endTime = timer; }
	nT.duration = timer;
	nT.sortNotes();
	tracks.push_back( nT );
	return endPoint;
}

unsigned int MIDIRead::processContent( const unsigned int& it, float& timer, 
	Track& track ) 
	{
	++it;
	static unsigned char lastEvent;
	unsigned char c = memblock[it]; // Read byte.
	std::vector<unsigned char> contents;
	if ( c == 0xFF ) { 
		// std::cout << "\n" << it;
		++it;
		c = memblock[it];
		if( c == 0x2F ) { it += 2; return; }
		return processMeta( it, timer, track ); 
	} else if( c == 0xF0 || c == 0xF7 ) {
		// SysEx. Not used by us. Because they are variable length, we're
		// kind of fucked until I bother do this shit.
		// processSysEx( it, timer, track );
	} else {
		// std::cout << "\t" << it;
		if ( c >= 0x80 && c < 0xA0 ) {
			// Note-on and -off messages.
			unsigned int range = it + MIDICONTROLLENGTH;
			for(it = it; it != range; ++it) 
				{ contents.push_back(memblock[it]); }
			return processNote( it, timer, track );
			lastEvent = c;
		} else if( c >= 0xB0 && c < 0xC0 ) {
			/* Controller events. Don't really know how this will affect
			our charts but they may have implications down the line. */
		} else if( c >= 0xC0 && c < 0xE0 ) {
			// Program change and aftertouch events. Irrelevant to us.
		} else if( c >= 0xE0 && c < 0xF0 ) {
			/* Pitch Bend events. These will be necessary to parse to get
			a direct line to string-bend events. An intermediary for now 
			is using the 'T' text event to specify bends. */
			unsigned int range = it + MIDICONTROLLENGTH;
			for(it = it; it != range; ++it) 
				{ contents.push_back(memblock[it]); }
			lastEvent = c;
		} else if( c < 0x80 ) {
			/* This one requires an explanation. The 4-bit event-type sequence 
			doesn't show up if there is a zero-value delta-time AND the 
			event-type repeats. As such, this throws the otherwise 
			well-established pattern off. Fucking 80's. */
			contents.push_back(lastEvent);
			unsigned int range = it + MIDICONTROLLENGTH - 1;
			for(it = it; it != range; ++it) 
				{ contents.push_back(memblock[it]); }
			if( lastEvent >= 0x80 && lastEvent < 0xA0 ) { 
				return processNote( contents, timer, track );
			} else if ( lastEvent >= 0xE0 && lastEvent < 0xF0 ) { }
		}
	} /* else {
		std::cout << "BORK BORK BORK at: " 
		<< it << " | " << c << "\n"; 
	} */
	return it;
}

#endif