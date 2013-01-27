#ifndef _BASE_ARRANGE_
#define _BASE_ARRANGE_

#ifndef _BASE-DIFFICULTY_
#include "BaseDifficulty.h"
#endif

#ifndef _BASE-STRUCTURE_
#include "BaseStructure.h"
#endif

#ifndef _BASE-TRACK_
#include "BaseTrack.h"
#endif

#define DEFAULTSTRINGS 6

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
	
// Class declarations


struct Beat
	{
	// int bar, beat;
	int bar; // The current bar. If not the first beat, '-1'.
	float time;
	float tempo;
	// TimeSig timeSig;
	};

class ChordTemplate
	{
	int id; static int count; 
	std::string name; 
	std::string display; // Display name.
	
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
		std::string getDisplayName() { return display; };
		int getFirstString() { return firstString; };
		int getMinDif() { return minDif; };
		int getMaxDif() { return maxDif; };
		int getDif() { return maxDif; };
		
		void setID() { id = count; count++; };
		void setName(std::string s) { name = s; };
		void setDisplayName(std::string s) { display = s; };
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
	
class Arrangement
	{
	// Private
	std::string name;
	float duration;
	int tuning[DEFAULTSTRINGS];	
	std::vector<Section> sections;
	std::vector<Difficulty> difficulties;
	std::vector<Note> vNotes;
	std::vector<Chord> vChords;
	std::vector<Anchor> vAnchors;
	std::vector<HandShape> vHands;
	std::vector<Beat> beats;
	Track trackCopy; // redundant, but maybe helpful?
	
	public:
		Arrangement() { name = ""; duration = 0.0; };
		Arrangement(Track t); // For 'slimline' version
		Arrangement(const Arrangement& a); // Copy constructor
		// Arrangement operator=(const Arrangement& a); // Assignment operator.
		~Arrangement();
		
		void addSection(Section s) { sections.push_back(s); }
		void addSections(std::vector<Section> s) 
			{ addXs(s, sections); };
		
		void addDifficulty(Difficulty d) { difficulties.push_back(d); };
		void addDifficulties(std::vector<Difficulty> d) 
			{ addXs(d, difficulties); };
		
		void addNotes(std::vector<Note> n) { addXs(n, vNotes); };
		void addChords(std::vector<Chord> c) { addXs(c, vChords); };
		void addAnchors(std::vector<Anchor> a) { addXs(a, vAnchors); };
		void addHands(std::vector<HandShape> h) { addXs(h, vHands); };
		
		std::string getName() { return name; };
		float getDuration() { return duration; };
		int getTuning(int i) { return tuning[i]; };
		// Returns number of difficulties.
		int getNumDifficulties(int i) { return difficulties.size(); }; 
		int getNumNotes(int i) { return vNotes.size(); };
		
		std::vector<Section> getSections() { return sections; };
		std::vector<Difficulty> getDifficulties() { return difficulties; };
		std::vector<Note> getNotes() { return vNotes; };
		std::vector<Chord> getChords() { return vChords; };
		std::vector<Anchor> getAnchors() { return vAnchors; };
		std::vector<HandShape> getHands() { return vHands; };
		std::vector<Beat> getBeats() { return beats; };
		
		// void setDifficulties(int i) { difficulties = i; };
	};
	
Arrangement::Arrangement(Track t)
	{
	name = t.getName();
	duration = t.getDuration();
	std::vector<Tempo> tempo(t.getTempos());
	Track trackCopy(t);
	}
	
Arrangement::Arrangement(const Arrangement& a)
	{
	name = a.name;
	duration = a.duration;
	std::vector<Section> sections(a.sections);
	std::vector<Beat> beats(a.beats);
	trackCopy = a.trackCopy;
	}
	
/* Arrangement::Arrangement operator=(const Arrangement& a)
	{
	
	} */
	
Arrangement::~Arrangement()
	{
	
	}


#endif