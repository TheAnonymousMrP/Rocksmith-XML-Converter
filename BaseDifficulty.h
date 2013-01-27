#ifndef _BASE_DIFFICULTY_
#define _BASE_DIFFICULTY_

#ifndef _BASE-TRACK_
#include "BaseTrack.h"
#endif

// Methods
template <class X>
void addIs(std::vector<X> source, std::vector<int>& dest)
	{
	for(typename std::vector<X>::iterator it = source.begin();
		it != source.end(); ++it)
		{ dest.push_back(it - source.begin()); }
	}
	
template <class X>
std::vector<int> getIs(std::vector<X> source)
	{
	std::vector<int> i;
	for(typename std::vector<X>::iterator it = source.begin();
		it != source.end(); ++it)
		{ i.push_back(it - source.begin()); }
	return i;
	}
	
template <class X>
std::vector<X> getXsFromIs(std::vector<X> xSource, std::vector<int> iSource)
	{
	std::vector<X> x;
	for(typename std::vector<int>::iterator it = iSource.begin();
		it != iSource.end(); ++it)
		{ 
		int cI(*it);
		if(xSource.size() > cI)
			{ x.push_back(xSource.at(cI)); }
		else { break; }
		}
	return x;
	}

// Class declarations
struct Anchor
	{
	float time;
	int fret;
	float width;
	};

class Chord
	{
	// Private
	float time;
	int id; int difficulty; // Not sure if difficulty is needed.
	bool accent, fretHandMute, highDensity, ignore, linkNext, palmMute, strum;
	
	std::vector<Note> vNotes;
	
	public:
		Chord() { time = 0.0; id = 0; difficulty = 0; };
		Chord(const Chord& c);
		~Chord() { };
		
		void addNote(Note n) { vNotes.push_back(n); };
		std::vector<Note> getNotes() { return vNotes; };
		
		float getTime() { return time; };
		int getID() { return id; };
		int getDifficulty() { return difficulty; };	
		
		bool getAccent() { return accent; }			
		bool getFretHandMute() { return fretHandMute; };
		bool getHighDensity() { return highDensity; };
		bool getIgnore() { return ignore; };
		bool getLinkNext() { return linkNext; };
		bool getPalmMute() { return palmMute; }
		bool getStrum() { return strum; };	
		
		void setDifficulty() // After getting notes. Highest difficulty.
			{
			for(std::vector<Note>::iterator it = vNotes.begin();
				it != vNotes.end(); ++it)
				{
				Note cN(*it);
				if(cN.getDif() > difficulty) 
					{ difficulty = cN.getDif(); }
				}				
			};
		void setPalmMute() // Call after filling with notes.
			{
			for(std::vector<Note>::iterator it = vNotes.begin();
				it != vNotes.end(); ++it)
				{
				Note cN(*it);
				if(!cN.getPalmMute()) { palmMute = false; break; }
				}
			};
		void setStrum(bool s) { strum = s; };

	};
	
Chord::Chord(const Chord& c)
	{
	time = c.time;
	id = c.id;
	linkNext = c.linkNext;
	accent = c.accent;
	fretHandMute = c.fretHandMute;
	highDensity = c.highDensity;
	ignore = c.ignore;
	palmMute = c.palmMute;
	strum = c.strum;
	
	vNotes = c.vNotes;
	}
	
struct HandShape
	{
	float time;
	float duration;
	int id;	
	};	
	
class Difficulty
	{
	// Private
	int difficulty; static int count; 
	
	/*
	std::vector<Note> notes;
	std::vector<Chord> chords;
	std::vector<Anchor> anchors;
	std::vector<HandShape> hands;
	*/
	
	std::vector<Note*> notesP;
	
	std::vector<int> notesPointer;
	std::vector<int> chordsPointer;
	std::vector<int> anchorsPointer;
	std::vector<int> handsPointer;
	
	public:
		Difficulty() { };
		Difficulty(const Difficulty& d)
			{ 
			difficulty = d.difficulty; 
			/* std::vector<Note> notes(d.notes);
			std::vector<Chord> chords(d.chords);
			std::vector<Anchor> anchors(d.anchors);
			std::vector<HandShape> hands(d.hands); */
			std::vector<int> notesPointer(d.notesPointer);
			std::vector<int> chordsPointer(d.chordsPointer);
			std::vector<int> anchorsPointer(d.anchorsPointer);
			std::vector<int> handsPointer(d.handsPointer);
			}
		/* Difficulty operator=(const Difficulty& d)
			{
			difficulty = d.difficulty;
			} */
		~Difficulty() { };
		
		// void addNote(Note n) { notes.push_back(n); };
		void addNoteI(int i) { notesPointer.push_back(i); };
		// void addNotes(std::vector<Note> n) { addXs(n, notes); };
		// void addChord(Chord c) { chords.push_back(c); };
		void addChordI(int i) { chordsPointer.push_back(i); };
		// void addChords(std::vector<Chord> c) { addXs(c, chords); };
		void addAnchorI(int i) { anchorsPointer.push_back(i); };
		void addHandI(int i) { handsPointer.push_back(i); };
		
		void addNotesI(std::vector<int> sI) { addXs(sI, notesPointer); };
		void addChordsI(std::vector<int> sI) { addXs(sI, chordsPointer); };
		void addAnchorsI(std::vector<int> sI) { addXs(sI, anchorsPointer); };
		void addHandsI(std::vector<int> sI) { addXs(sI, handsPointer); };
		
		int getDifficulty() { return difficulty; };
		
		std::vector<Note> getNotes(std::vector<Note> sX) 
			{ return getXsFromIs(sX, notesPointer); };
		std::vector<Chord> getChords(std::vector<Chord> sX) 
			{ return getXsFromIs(sX, chordsPointer); };
		std::vector<Anchor> getAnchors(std::vector<Anchor> sX) 
			{ return getXsFromIs(sX, anchorsPointer); };
		std::vector<HandShape> getHands(std::vector<HandShape> sX) 
			{ return getXsFromIs(sX, handsPointer); };
		
		std::vector<int> getNotesPointer() { return notesPointer; };
		std::vector<int> getChordsPointer() { return chordsPointer; };
		std::vector<int> getAnchorsPointer() { return anchorsPointer; };
		std::vector<int> getHandsPointer() { return handsPointer; };
		
		int getNotesSize() { return notesPointer.size(); };
		int getChordsSize() { return chordsPointer.size(); };
		int getAnchorsSize() { return anchorsPointer.size(); };
		int getHandsSize() { return handsPointer.size(); };
		
		void setDifficulty() { difficulty = count; ++count; };
		
		void reset() { count = 0; }
	};

int Difficulty::count = 0;

#endif