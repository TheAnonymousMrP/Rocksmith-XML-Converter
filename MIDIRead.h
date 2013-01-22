#include <cmath>
#include <algorithm>

#ifndef _BASE_TRACK_
#include "BaseTrack.h"
#endif

#define HEADER 14
#define TRACKHEAD 8
#define MAXDELTA 4
#define DEFAULTTEMPO 120
#define DEFAULTOFFSET 5.25 // Default offset (in seconds) for songs.
#define METALENGTH 4 // Offset to add to all meta-events

class MIDIRead
	{
	// Private
	std::string fileName; std::string midiMode;
	std::vector<Track> tracks;
	
	unsigned char* memblock; // unsigned for the later hex comparisons.
	float timer; float currentTempo; float endTime;
	int tempoCount; int division;
	
	// Process methods
	int getMIDI(std::string midiName); // Copies file to char array.
	int processTrack(int beginPoint); // Grabs a single track from the file.
	void addNote(int i, float deltaConv, Track& currentTrack);

	// Conversion methods
	float convertDelta2TimeFloat(int delta);
	float convertSMPTE2TimeFloat(int hour, int minute, 
		int second, int frame, int fraction);
	float convertBytes2Int(int offset, int length);
	float convertBytes2VLQ(int offset, int length);
	std:: string convertBytes2String(int offset, int length);
	
	public:
		MIDIRead();
		MIDIRead(std::string s);
		~MIDIRead() { };
		
		void process(int arrN); // 'Main' method for MIDIRead
		// 'arrN' is the number of arrangements we want to pull from the file.
		void debug(std::string s); /* Will generate a file listing the 
		contents of the pulled tracks. For debug purposes. */
		
		float getEndTime() { return endTime; }	
		Track getTrack(int i); // Copies a track for public modification.
		int getNumTracks() { return tracks.size(); }; 
		std::vector<Track> getTracks() { return tracks; };
		
		void setFileName(std::string s) { fileName = s; };
		void setMidiMode(std::string s) { midiMode = s; };
	};

// Public ====
	
MIDIRead::MIDIRead() { fileName = ""; timer = DEFAULTOFFSET; }
MIDIRead::MIDIRead(std::string s) { fileName = s; timer = DEFAULTOFFSET; }