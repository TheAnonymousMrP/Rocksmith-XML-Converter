#include <cmath>
#include <algorithm>

#ifndef _BASE_TRACK_
#include "BaseTrack.h"
#endif

#define ENDLINE << "\n";

#define HEADER 14
#define TRACKHEAD 8
#define MAXDELTA 4
#define DEFAULTTEMPO 120
#define DEFAULTOFFSET 5.25 // Default offset (in seconds) for songs.
#define METALENGTH 4 // Offset to add to all meta-events

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
	float timer; float currentTempo; float endTime;
	int tempoCount; int division;
	std::vector<Tempo>::const_iterator tCount, tEnd;
	
	// Process methods
	int getMIDI(std::string midiName); // Copies file to char array.
	int processTrack(int beginPoint); // Grabs a single track from the file.
	void addNote(int i, float deltaConv, Track& currentTrack);
	bool processMeta(int& i, const float& deltaConv, Track& currentTrack);

	// Conversion methods
	float convertDelta2Time(int delta);
	float convertSMPTE2TimeFloat(int hour, int minute, 
		int second, int frame, int fraction);
	float convertBytes2Int(int offset, int length);
	float convertBytes2VLQ(int offset, int length);
	std::string convertBytes2String(int offset, int length);
	
	// Mark 2
	int processDelta(const unsigned int& beginPoint);
	unsigned int processContent(unsigned int it, const float& timer);
	// Converters and stuff
	int convertBytes2VLQ2(const std::vector<unsigned char>& vlq);
	float getCurrentTempo(float tempo, 
	std::vector<Tempo>::iterator& tCount, const Track& track, 
	const float& timer);
	float convertDelta2Time2(const int& delta, const float& tempo);
	
	public:
		MIDIRead(std::string s = "", eMidi m = eMidi::normal);
		~MIDIRead() { };
		
		void process(int arrN); // 'Main' method for MIDIRead
		// 'arrN' is the number of arrangements we want to pull from the file.
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
	timer = DEFAULTOFFSET; 
	tempoCount = 0;
}