#ifndef _BASE_
#define _BASE_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#define ENDLINE << "\n";
#define NUMSTRINGS 6

#define ONEMINUTE 60.000
#define ONEMINUTEMILLI 60000
#define ONEMINUTEMICRO 60000000
#define ONESECONDMILLI 1000
#define DEFAULTTIMESIGNUM 4 // Default numerator for the time signature
#define DEFAULTTIMESIGDEN 4 // Default denominator for the time signature

int stringPitch[NUMSTRINGS]; // Holds the open-string pitch values.

enum eMeta
	{
	anchor = 0,
	bend,
	chord,
	lyrics,
	marker,
	phrase,
	tech,
	special,
	};
	
enum eTechnique
	{
	none,
	// Bends may need to be expanded in the future with new bend additions.
	bendHalf,
	bendFull,
	harmonic,
	hammerOn,
	pullOff,
	palmMute,
	slide,
	tremolo,
	};
	
struct Meta
	{
	float time;
	std::string text;
	};
	
struct Tempo
	{
	float time;
	float tempo;
	};
	
struct TimeSig
	{
	float time;
	int num; // Numerator. 
	int denom; // Denominator. Negative power of 2.
	int clock; // MIDI clocks in a metronome click.
	int quart; // 32nd notes per quarter-note.
	};

class Note
	{
	// Private
	float time;
	float duration;
	int string;
	int pitch;
	int fret;
	int minDif;
	// int maxDif;
	
	eTechnique technique;
	int slide;
	
	public:
		Note();
		Note(float t, int s, int p, int d);
		// Note(float t, float dur, int s, int p, int dif);
		Note(const Note& n); // Copy Constructor
		~Note() { };
		
		float getTime() { return time; };
		float getDuration() { return duration; };
		int getString() { return string; };
		int getPitch() { return pitch; };
		int getFret() { return fret; };
		int getDif() { return minDif; };
		 
		void setDuration(float d) { duration = d; };
		// This should only be called after we have the right tuning.
		void setFret() { fret = pitch - stringPitch[string]; };	
		void setTechnique(eTechnique t)	
			{
			technique = t;
			/* switch(i)
				{
				case eBend1: techni = 1; break;
				case eBend2: bend = 2; break;
				case eHar: har = 1; break;
				case eHam: ham = 1; hopo = 1; break;
				case ePull: pull = 1; hopo = 1; break;
				case ePalm: palm = 1; break;
				case eTrem: trem = 1; break;
				} */
			}
		void setSlide(int fret) { slide = fret; }
	};
	
Note::Note()
	{
	time = -1.0;
	duration = 0;
	string = -1;
	pitch = -1;
	fret = -1;
	minDif = -1;
	
	technique = none;
	}
	
Note::Note(float t, int s, int p, int d)
	{
	time = t;
	duration = 0;
	string = s;
	pitch = p;
	fret = -1;
	minDif = d;
	
	technique = none;
	}
	
Note::Note(const Note& n)
	{
	time = n.time;
	duration = n.duration;
	string = n.string;
	pitch = n.pitch;
	fret = n.fret;
	minDif = n.minDif;
	
	technique = n.technique;
	}
	
// Functions
float convertTempo2TimeFloat(float tempo)
	{
	float f = 0.0;
	float beat = ONEMINUTEMILLI / tempo;
	f = beat / ONESECONDMILLI;
	return f;
	};

template <class T>
std::vector<T> getXsWithinTime(std::vector<T> source, 
	float a, float b)
	{
	std::vector<T> x;
	float t;
	for(typename std::vector<T>::iterator it = source.begin();
		it != source.end(); ++it)
		{
		T& cX = *it;
		t = cX.getTime();
		if(t > b) { return x; }
		if(t >= a) { x.push_back(cX); }
		}
	}
	
template <class T>
std::vector<T> getXsCopyWithinTime(const std::vector<T> source, 
	float a, float b)
	{
	std::vector<T> x;
	float t;
	for(typename std::vector<T>::const_iterator it = source.begin();
		it != source.end(); ++it)
		{
		T cX = *it;
		t = cX.getTime();
		if(t > b) { return x; }
		if(t >= a) { x.push_back(cX); }
		}
	}
	
#endif