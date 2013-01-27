#ifndef _BASE_STRUCTURE_
#define _BASE_STRUCTURE_

#ifndef _BASE-TRACK_
#include "BaseTrack.h"
#endif

// Methods
template <class T>
std::vector<int> getIsWithinTime(std::vector<T> source, float a, float b)
	{
	std::vector<int> i;
	float t = 0.0;
	for(typename std::vector<T>::iterator it = source.begin();
		it != source.end(); ++it)
		{
		T cX(*it);
		t = cX.getTime();
		if(t > b) { break; }
		if(t >= a) { i.push_back(it - source.begin()); }
		}
	return i;
	}
	
template <class X>
std::vector<X> getXsFromIsWithinTime(std::vector<X> xSource, 
	std::vector<int> iSource, float a, float b)
	{
	std::vector<X> x;
	float t = 0.0;
	for(typename std::vector<int>::iterator it = iSource.begin();
		it != iSource.end(); ++it)
		{ 
		int cI(*it);
		if(xSource.size() > cI)
			{
			X cX(xSource.at(cI));
			t = cX.getTime();
			if(t > b) { break; }
			if(xSource.size() > cI)
				{ x.push_back(xSource.at(cX)); }
			}
		else { break; }
		}
	return x;
	}
	
// Class declarations	
class Phrase
	{
	int id; static int count;
	std::string name; static std::vector<std::string> names;
	float time, duration;
	int startBeat, endBeat;
	int startNoteI, endNoteI;
	int maxDif;
	bool disparity, solo, ignore;
	
	// std::vector<Difficulty> contents;
	
	public:
		Phrase();
		Phrase(const Phrase& p); // Copy constructor
		Phrase(Meta m);
		~Phrase() { };
		
		// void addDifficulty(Difficulty x) { contents.push_back(x); };
		// void addDifficulties(std::vector<Difficulty> x) { addXs(x, contents); };
		
		int getID() { return id; };
		std::string getName() { return name; };
		float getTime() { return time; };
		float getDuration() { return duration; };
		int getStartNote() { return startNoteI; };
		int getMaxDif() { return maxDif; };
		bool getDisparity() { return disparity; };
		bool getSolo() { return solo; };
		bool getIgnore() { return ignore; };
		
		// Difficulty getDifficulty(int i) { return contents.at(i); };
		
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
	
	// std::vector<Difficulty> contents(p.contents);
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
	
	std::vector<Phrase> phrases;
	
	public:
		Section();
		Section(const Section& s); // Copy constructor
		Section(Meta m);
		~Section() { };
		
		void addPhrase(Phrase p) { phrases.push_back(p); };
		void addPhrases(std::vector<Phrase> p) { addXs(p, phrases); };
		
		std::string getName() { return name; }
		float getTime() { return time; }
		float getDuration() { return duration; }
		int getID() { return iteration; }
		std::vector<Phrase> getPhrases() { return phrases; };
		
		void setDuration(float x) { duration = x; };
		void setID() { iteration = 0; };
		void setID(int i) { iteration = ++i; };
	};
	
Section::Section()
	{
	name = "";
	time = 0;
	duration = 0;	
	iteration = 1;
	}
	
Section::Section(const Section& s)
	{
	name = s.name;
	time = s.time;
	duration = s.duration;
	iteration = s.iteration;
	
	std::vector<Phrase> phrases(s.phrases);
	}
	
Section::Section(Meta m)
	{
	name = m.text;
	time = m.time;
	duration = 0;
	
	iteration = 1;
	}
	
#endif