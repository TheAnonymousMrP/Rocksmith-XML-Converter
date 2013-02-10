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
const std::vector<unsigned int> getIsWithinTime(std::vector<T> source, 
	float a, float b)
	{
	std::vector<unsigned int> i;
	float t = 0.0;
	for(auto it = source.begin(); it != source.end(); ++it) {
		t = it->getTime();
		if( t > b ) { break; }
		if( t >= a ) { i.push_back( it - source.begin() ); }
	}
	return i;
}
	
template <class X>
const std::vector<X> getXsFromIsWithinTime(std::vector<X> xSource, 
	std::vector<unsigned int> iSource, float a, float b)
	{
	std::vector<X> x;
	for(auto& i : iSource) { 
		if( xSource.size() > i ) {
			if( xSource.at( i ).getTime() > b ) { break; }
			if( xSource.at( i ).getTime() >= a )
				{ x.push_back( xSource.at( i ) ); }
		} else { break; }
	}
	return x;
}
	
// Class declarations	
class Phrase {
	unsigned int id; unsigned int variation;
	std::string name;
	float time;
	
	public:
		Phrase();
		Phrase(Meta m, unsigned int i = 0, unsigned int c = 0);
		
		float duration;
		unsigned int startNoteI, endNoteI;
		unsigned int startBeat, endBeat;
		unsigned int maxDif;
		
		const unsigned int& getID() const { return id; };
		const std::string& getName() const { return name; };
		const float& getTime() const { return time; };
		const unsigned int& getVariation() const { return variation; };
};
	
Phrase::Phrase() { 
	name = "";
	time = 0;
	duration = 0;
}	
	
Phrase::Phrase(Meta m, unsigned int i, unsigned int c) {
	id = i;
	name = m.text;
	time = m.time;
	duration = 0;
	variation = c;
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