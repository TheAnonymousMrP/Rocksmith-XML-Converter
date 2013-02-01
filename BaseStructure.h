#ifndef _BASE_STRUCTURE_
#define _BASE_STRUCTURE_

#ifndef _BASE_TRACK
#include "BaseTrack.h"
#endif

// Methods
template <class T>
std::vector<T> getXsWithinTime(std::vector<T> source, float a, float b)
	{
	std::vector<T> x;
	float t = 0.0;
	for(auto it = source.begin(); it != source.end(); ++it)
		{
		t = it->getTime();
		if(t >= b) { break; }
		if(t >= a) { x.push_back((*it)); }
		}
	return x;
	}

template <class T>
const std::vector<int> getIsWithinTime(std::vector<T> source, 
	float a, float b)
	{
	std::vector<int> i;
	float t = 0.0;
	for(auto it = source.begin(); it != source.end(); ++it)
		{
		t = it->getTime();
		if(t > b) { break; }
		if(t >= a) { i.push_back(it - source.begin()); }
		}
	return i;
	}
	
template <class X>
const std::vector<X> getXsFromIsWithinTime(std::vector<X> xSource, 
	std::vector<int> iSource, float a, float b)
	{
	GOTHERE
	std::vector<X> x;
	float t = 0.0;
	for(auto it = iSource.begin(); it != iSource.end(); ++it)
		{ 
		int cI(*it);
		if(xSource.size() > cI)
			{
			GOTHERE
			X cX(xSource.at(cI));
			t = cX.getTime();
			if(t > b) { break; }
			if(xSource.size() > cI)
				{ x.push_back(xSource.at(cX)); }
			}
		else { break; }
		}
	GOTHERE
	return x;
	}
	
// Class declarations	
class Phrase {
	int id; static int count;
	std::string name;
	float time;
	int startBeat, endBeat;
	
	int maxDif;
	bool disparity, solo, ignore;
	
	public:
		Phrase();
		Phrase(const Phrase& p); // Copy constructor
		Phrase(Meta m);
		~Phrase() { };
		
		float duration;
		int startNoteI, endNoteI;
		
		const int getID() const { return id; };
		std::string getName() { return name; };
		float getTime() { return time; };
		int getMaxDif() { return maxDif; };
		bool getDisparity() { return disparity; };
		bool getSolo() { return solo; };
		bool getIgnore() { return ignore; };
		
		void setID() { id = count; ++count; }
		void setID(int i) { id = i; } 
		void setMaxDif(int i) { maxDif = i; };
		
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

class Section {
	std::string name;
	float time;
	int startBeat, endBeat;
	int iteration;
	
	std::vector<Phrase> phrases;
	
	public:
		Section();
		Section(const Section& s) : phrases(s.phrases) { 
			name = s.name; time = s.time; duration = s.duration; 
			iteration = s.iteration; 
		}; // Copy constructor
		Section(Meta m, float d = 0.0, int i = 1);
		~Section() { };
		
		float duration;
		
		void addPhrase(Phrase p) { phrases.push_back(p); };
		void addPhrases(std::vector<Phrase> p) { addXs(p, phrases); };
		
		const std::string& getName() const { return name; }
		const float& getTime() const { return time; }
		const int& getID() const { return iteration; }
		std::vector<Phrase> getPhrases() const { return phrases; };
		
		void setID() { iteration = 1; };
		void setID(int i) { iteration = ++i; };
};
	
Section::Section() {
	name = "";
	time = 0;
	duration = 0;	
	iteration = 1;
}

Section::Section(Meta m, float d, int i) {
	name = m.text;
	time = m.time;
	duration = d;
	
	iteration = i;
}
	
#endif