#ifndef _BASE_DIFFICULTY_
#define _BASE_DIFFICULTY_

#ifndef _BASE_TRACK
#include "BaseTrack.h"
#endif

#define DEFAULTANCHORWIDTH 4

// Methods
template <class X>
void addIs(std::vector<X> source, std::vector<int>& dest)
	{
	for(typename std::vector<X>::iterator it = source.begin();
		it != source.end(); ++it)
		{ dest.push_back(it - source.begin()); }
	}
	
template <class X>
std::vector<int> getIs(std::vector<X> source) {
	std::vector<int> i;
	for(auto it = source.begin(); it != source.end(); ++it)
		{ i.push_back(it - source.begin()); }
	return i;
}
	
template <class X>
const std::vector<X> getXsFromIs(const std::vector<X>& xSource, 
	const std::vector<unsigned int>& iSource)
	{
	std::vector<X> x;
	for(const unsigned int& i : iSource) { 
		if(i < xSource.size()) { x.push_back(xSource.at(i)); }
		else { break; }
	}
	return x;
	}
	
template <class X>
const std::vector<X> getXsWithinRange(const std::vector<X>& xSource, 
	typename std::vector<X>::iterator it, const int range)
	{
	std::vector<X> x;
	for(it = it; it != it + range && it != xSource.end(); ++it)
		{ x.push_back((*it)); }
	return x;
	}
	
int isChord(const std::vector<Note>& nSource, 
	std::vector<Note>::iterator it, int dif) 
	{
	// Recursive. Returns number of notes if a chord, 1 otherwise.
	// Known bug. See 'top' file.
	float time = it->getTime(); int minDif = it->getDif(); ++it;
	if(it != nSource.end() && time == it->getTime() && minDif <= dif) 
//		&& it->getDif() <= dif) { return isChord(nSource, it, dif); }
		{ 
		int i = isChord(nSource, it, dif);
		if(it->getDif() <= dif) { return i; }
		else if(i > 0) { return i; }
		}
	return 1;
	}
	
template <class T>
int findMaxDif(const std::vector<T>& source) {
	int max = 0, a = 0;
	for(auto it = source.begin(); it != source.end(); ++it)
		{ a = it->normalisedDif; if(max < a) { max = a; } }
	return max;
}

// Class declarations
struct Anchor
	{
	float time;
	int fret;
	float width;
	};

class Chord {
	// Private
	float time;
	int id; 
	bool accent, fretHandMute, highDensity, ignore, linkNext, palmMute, strum;
	
	std::vector<unsigned int> vNotesI;
	
	public:
		Chord(float t = -1.0, int i = -1) { time = t; id = i; };
		Chord(const std::vector<unsigned int>& iSource, float t = -1.0, 
			int i = -1) : vNotesI(iSource)
			{ time = t; id = i; };
		Chord(const Chord& c) : vNotesI(c.vNotesI) { 
			time = c.time; id = c.id;
			linkNext = c.linkNext; accent = c.accent;
			fretHandMute = c.fretHandMute; highDensity = c.highDensity;
			ignore = c.ignore; palmMute = c.palmMute; strum = c.strum; 
		};
		~Chord() { };
		
		void addNotesI(int i) { vNotesI.push_back(i); };
		const std::vector<unsigned int> getNotesI() const { return vNotesI; };
		
		const float& getTime() const { return time; };
		const int& getID() const { return id; };
		
		bool getAccent() const { return accent; }			
		bool getFretHandMute() const { return fretHandMute; };
		bool getHighDensity() const { return highDensity; };
		bool getIgnore() const { return ignore; };
		bool getLinkNext() const { return linkNext; };
		bool getPalmMute() const { return palmMute; }
		bool getStrum() const { return strum; };	
		
		void setID(int i) { id = i; } 
		/*
		void setPalmMute() // Call after filling with notes.
			{
			for(std::vector<Note>::iterator it = vNotes.begin();
				it != vNotes.end(); ++it)
				{
				Note cN(*it);
				if(!cN.getPalmMute()) { palmMute = false; break; }
				}
			};
		*/
		void setStrum(bool s) { strum = s; };
};
	
struct HandShape
	{
	float time;
	float duration;
	int id;	
	};	
	
class Difficulty {
	// Private
	int difficulty; static int count; 
	
	/*
	std::vector<Note> notes;
	std::vector<Chord> chords;
	std::vector<Anchor> anchors;
	std::vector<HandShape> hands;
	*/
	
	std::vector<Note*> notesP;
	
	std::vector<unsigned int> notesI;
	std::vector<unsigned int> chordsI;
	std::vector<unsigned int> anchorsI;
	std::vector<unsigned int> handsI;
	
	public:
		Difficulty() { };
		Difficulty(const Difficulty& d) :
			notesI(d.notesI), chordsI(d.chordsI),
			anchorsI(d.anchorsI), handsI(d.handsI)
			{ difficulty = d.difficulty; };
		/* Difficulty operator=(const Difficulty& d)
			{
			difficulty = d.difficulty;
			} */
		~Difficulty() { };
		
		void addNoteI(unsigned int i) { notesI.push_back(i); };
		void addChordI(unsigned int i) { chordsI.push_back(i); };
		void addAnchorI(unsigned int i) { anchorsI.push_back(i); };
		void addHandI(unsigned int i) { handsI.push_back(i); };
		
		void addNotesI(std::vector<unsigned int> sI) 
			{ addXs(sI, notesI); };
		void addChordsI(std::vector<unsigned int> sI) 
			{ addXs(sI, chordsI); };
		void addAnchorsI(std::vector<unsigned int> sI) 
			{ addXs(sI, anchorsI); };
		void addHandsI(std::vector<unsigned int> sI) 
			{ addXs(sI, handsI); };
		
		const int& getDifficulty() const { return difficulty; };
		
		/* std::vector<Note> getNotes(std::vector<Note> sX) const
			{ return getXsFromIs(sX, notesPointer); };
		std::vector<Chord> getChords(std::vector<Chord> sX) const
			{ return getXsFromIs(sX, chordsPointer); };
		std::vector<Anchor> getAnchors(std::vector<Anchor> sX) const
			{ return getXsFromIs(sX, anchorsPointer); };
		std::vector<HandShape> getHands(std::vector<HandShape> sX) const
			{ return getXsFromIs(sX, handsPointer); }; */
			
		const std::vector<unsigned int>& getNotesI() const 
			{ return notesI; };
		const std::vector<unsigned int>& getChordsI() const 
			{ return chordsI; };
		const std::vector<unsigned int>& getAnchorsI() const 
			{ return anchorsI; };
		const std::vector<unsigned int>& getHandsI() const 
			{ return handsI; };
		
		int getNotesSize() const { return notesI.size(); };
		int getChordsSize() const { return chordsI.size(); };
		int getAnchorsSize() const { return anchorsI.size(); };
		int getHandsSize() const { return handsI.size(); };
		
		void setDifficulty() { difficulty = count; ++count; };
		
		void reset() { count = 0; }
};

int Difficulty::count = 0;

#endif