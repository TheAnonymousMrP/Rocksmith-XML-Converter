#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

#ifndef _BASE_TRACK_
#include "BaseTrack.h"
#endif

#define ENDLINE << "\n";

#define FILETYPEBYTE 9
#define HEADERLENGTH 14
#define TRACKHEADERLENGTH 8
#define MAXDELTA 4
#define DEFAULTTEMPO 120
#define DEFAULTOFFSET 5.250 // Default offset (in seconds) for songs.
#define DEFAULTOFFSETCENT 0.000
#define METALENGTH 4 // Offset to add to all meta-events
#define MIDICONTROLLENGTH 3 // Length of MIDI Control Events.

enum eMidi {
	normal,
	logic,
	rb3,
};

class MIDIRead {
	// Private
	std::string fileName; eMidi midiMode;
	std::vector<Track> tracks;
	
	unsigned char* memblock; // unsigned for the later hex comparisons.
	float currentTempo; float endTime; 
	int division;
	unsigned char lastEvent;
	
	// Process methods
	int getMIDI(std::string midiName); // Copies file to char array.
	
	// Mark 2
	int processDelta(const unsigned int& beginPoint);
	void processContent(unsigned int& it, float& timer, Track& nT);
	void processNote(const std::vector<unsigned char>& contents, 
		const float& timer, Track& nT);
	void processMeta(unsigned int& it, float& timer, Track& nT);
	// Converters and stuff
	float getCurrentTempo(std::vector<Tempo>::const_iterator& tCount, 
		const std::vector<Tempo>::const_iterator& tEnd, const float& timer);
	float convertDelta2Time(const int& delta, const float& tempo);
	int convertBytes2VLQ(const std::vector<unsigned char>& vlq);
	float convertBytes2Float(const std::vector<unsigned char>& b);
	float convertSMPTE2Time(const std::vector<unsigned char>& b);
	std::string convertBytes2String(const std::vector<unsigned char>& b);
	
	
	
	public:
		MIDIRead(std::string s = "", eMidi m = eMidi::normal);
		~MIDIRead() { };
		
		void process(unsigned int numArrs); // 'Main' method for MIDIRead
		void debug(); /* Will generate a file listing the contents of the 
		pulled tracks. For debug purposes. */
		
		const float& getEndTime() const { return endTime; };	 
		const Track& getTrack(int i) const { return tracks.at(i); };
		int getNumTracks() const { return tracks.size(); }; 
		const std::vector<Track>& getTracks() const { return tracks; };
};

// Public ====
MIDIRead::MIDIRead(std::string s, eMidi m) { 
	fileName = s; 
	if(m == eMidi::logic) { midiMode = eMidi::logic; } 
	else { midiMode = eMidi::normal; }
	currentTempo = DEFAULTTEMPO;
}