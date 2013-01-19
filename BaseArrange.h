#ifndef _BASE_ARRANGE_
#define _BASE_ARRANGE_

#ifndef _BASE-TRACK_
#include "BaseTrack-2.0.1.h"
#endif

// Functions
template <class T>
int findMaxDif(const std::vector<T>& source)
	{
	int max = 0, a = 0;
	for(typename std::vector<T>::const_iterator it = source.begin(); 
		it != source.end(); ++it)
		{ T x = *it; a = x.getDif(); if(max < a) { max = a; } }
	return max;
	}
	
/* template <class T>	
void addX(T source, std::vector<T>& dest) { dest.push_back(source); } */
	
template <class T>
void addXs(const std::vector<T>& source, std::vector<T>& dest)
	{
	for(typename std::vector<T>::const_iterator it = source.begin();
		it != source.end(); ++it)
		{ dest.push_back(*it); }
	}
	
// Class declarations
struct Beat
	{
	// int bar, beat;
	bool bar; // First beat in the bar.
	float time;
	float tempo;
	// TimeSig timeSig;
	};

struct Chord
	{
	float time;
	float duration;
	int id;
	};

class ChordTemplate
	{
	int id; static int count; 
	std::string name; 
	
	int firstString;
	int finger[NUMSTRINGS]; 
	int index[NUMSTRINGS]; // In co-operation with chordRef.
	int minDif; // The lowest difficulty amongst the notes.
	int maxDif; // The highest difficulty amongst the notes.
	
	std::vector<Note> notes;
	
	public:
		ChordTemplate();
		ChordTemplate(const ChordTemplate&); // Copy constructor
		~ChordTemplate() { };
		
		void addNote(Note n) { notes.push_back(n); }
		
		int getID() { return id; };
		std::string getName() { return name; };
		int getFirstString() { return firstString; };
		int getMinDif() { return minDif; };
		int getMaxDif() { return maxDif; };
		int getDif() { return maxDif; };
		
		void setID() { id = count; count++; };
		void setName(std::string s) { name = s; };
		void setMinDif() 
			{
			int min, a;
			for(std::vector<Note>::iterator it = notes.begin();
				it != notes.end(); ++it)
				{ Note x = *it; a = x.getDif(); if(a < min) { min = a; } }
			minDif = min;
			};
		void setMaxDif() { maxDif = findMaxDif(notes); };
			 
		
		void reset() { count = 0; }; // Resets count for new arrangement.
		
	};

int ChordTemplate::count = 0;
	
ChordTemplate::ChordTemplate() 
	{ }
	
ChordTemplate::ChordTemplate(const ChordTemplate& c)
	{
	id = c.id;
	name = c.name;
	firstString = c.firstString;
	for(int i = 0; i < NUMSTRINGS; i++)
		{ finger[i] = c.finger[i]; index[i] = c.index[i]; }
	minDif = c.minDif;
	
	std::vector<Note> notes(c.notes);
	}	
	
class Difficulty
	{
	// Private
	int difficulty; static int count; 
	
	std::vector<Note> notes;
	std::vector<Chord> chords;
	
	public:
		Difficulty() { };
		Difficulty(const Difficulty& d)
			{ 
			difficulty = d.difficulty; 
			std::vector<Note> notes(d.notes);
			std::vector<Chord> chords(d.chords);
			}
		~Difficulty() { };
		
		void addNote(Note n) { notes.push_back(n); };
		void addNotes(std::vector<Note> n) { addXs(n, notes); };
		void addChord(Chord c) { chords.push_back(c); };
		void addChords(std::vector<Chord> c) { addXs(c, chords); };
		
		int getDifficulty() { return difficulty; };
		
		void setDifficulty() { difficulty = count; ++count; };
		
		void reset() { count = 0; }
	};

int Difficulty::count = 0;
	
class Phrase
	{
	int id; static int count;
	std::string name; static std::vector<std::string> names;
	float time, duration;
	int startBeat, endBeat;
	int maxDif;
	bool disparity, solo, ignore;
	
	std::vector<Difficulty> contents;
	
	public:
		Phrase();
		Phrase(const Phrase& p); // Copy constructor
		Phrase(Meta m);
		~Phrase() { };
		
		void addDifficulty(Difficulty x) { contents.push_back(x); };
		void addDifficulties(std::vector<Difficulty> x) { addXs(x, contents); };
		
		int getID() { return id; };
		std::string getName() { return name; };
		float getTime() { return time; };
		float getDuration() { return duration; };
		int getMaxDif() { return maxDif; };
		bool getDisparity() { return disparity; };
		bool getSolo() { return solo; };
		bool getIgnore() { return ignore; };
		
		Difficulty getDifficulty(int i) { return contents.at(i); };
		
		void setID() { id = count; ++count; }
		void setID(int i) { id = i; } 
		void setDuration(float d) { duration = d; };
		void setMaxDif(int i) { maxDif = i; };
		/* This should only be called after the vectors are 'full'.
		void setMaxDif()
			{
			int maxN = findMaxDif(notes);
			int maxC = findMaxDif(chords);
			if(maxN < maxC) { maxDif = maxN; } else { maxDif = maxC; }
			}; */
		
		void reset() { count = 0; }; // Resets count for new arrangement.
	};
	
int Phrase::count = 0;
	
Phrase::Phrase()
	{ 
	name = "";
	time = 0;
	duration = 0;
	
	disparity = 0;
	solo = 0;
	ignore = 0;
	}	
	
Phrase::Phrase(const Phrase& p)
	{
	id = p.id;
	name = p.name;
	time = p.time;
	duration = p.duration;
	maxDif = p.maxDif;
	disparity = p.disparity;
	solo = p.solo; 
	ignore = p.ignore;
	
	std::vector<Difficulty> contents(p.contents);
	}
	
Phrase::Phrase(Meta m)
	{
	name = m.text;
	time = m.time;
	duration = 0;
	
	disparity = 0;
	solo = 0;
	ignore = 0;
	}
	
class Section
	{
	std::string name;
	float time, duration;
	int startBeat, endBeat;
	int iteration;
	
	std::vector<Phrase> contents;
	
	public:
		Section();
		Section(const Section& s); // Copy constructor
		Section(Meta m);
		~Section() { };
		
		void addPhrase(Phrase p) { contents.push_back(p); };
		void addPhrases(std::vector<Phrase> p) { addXs(p, contents); };
		
		std::string getName() { return name; }
		float getTime() { return time; }
		float getDuration() { return duration; }
		int getID() { return iteration; }
		Phrase getPhrase(int i) { return contents.at(i); };
		
		void setDuration(float x) { duration = x; };
		void setID() { iteration = 0; };
		void setID(int i) { iteration = ++i; };
	};
	
Section::Section()
	{
	name = "";
	time = 0;
	duration = 0;
	}
	
Section::Section(const Section& s)
	{
	name = s.name;
	time = s.time;
	duration = s.duration;
	iteration = s.iteration;
	
	std::vector<Phrase> contents(s.contents);
	}
	
Section::Section(Meta m)
	{
	name = m.text;
	time = m.time;
	duration = 0;
	
	iteration = 0;
	}
	
class Arrangement
	{
	// Private
	std::string name;
	int tuning;	
	std::vector<Section> contents;
	std::vector<Beat> beats;
	Track trackCopy; // redundant, but maybe helpful?
	
	public:
		Arrangement() { name = ""; };
		Arrangement(Track t); // For 'slimline' version
		Arrangement(const Arrangement& a); // Copy constructor
		~Arrangement() { };
		
		void addSection(Section s) { contents.push_back(s); }
		void addSections(std::vector<Section> s) { addXs(s, contents); };
		
		std::string getName() { return name; };
		int getTuning() { return tuning; };
		
		Beat getBeat(int i) { return beats.at(i); };
		std::vector<Beat> getBeats() { return beats; };
	};
	
Arrangement::Arrangement(Track t)
	{
	name = t.getName();
	std::vector<Tempo> tempo(t.getTempos());
	Track trackCopy(t);
	}
	
Arrangement::Arrangement(const Arrangement& a)
	{
	name = a.name;
	std::vector<Section> contents(a.contents);
	std::vector<Beat> beats(a.beats);
	trackCopy = a.trackCopy;
	}


#endif