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
	bendHalf, bendFull,
	fretHandMute,
	hammerOn,
	harmonic,
	palmMute,
	pickUp, pickDown,
	pinchHarmonic,
	pullOff,
	slide, slideUnpitch,
	// I'm guessing 'leftHand' and 'rightHand' are related to tapping.
	tapLeft, tapRight, 
	tremolo,
	vibrato,
	// Bass
	slap, pluck,
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
	
	// Techniques
	eTechnique technique; int techDif; // Difficulty when technique is added.
	
	float bendTime, bendStep; // if the bend is expanded as expected.
	bool fretHandMute;
	bool hammerOn;
	bool harmonic;
	bool palmMute;
	int slide;
	
	public:
		Note(float t = -1.0, int s = -1, int p = -1, int d = -1);
		// Note(float t, float dur, int s, int p, int dif);
		Note(const Note& n); // Copy Constructor
		~Note() { };
		
		float getTime() { return time; };
		float getDuration() { return duration; };
		int getString() { return string; };
		int getPitch() { return pitch; };
		int getFret() { return fret; };
		int getDif() { return minDif; };
		// Techniques
		bool getFretHandMute() 
			{ return (technique == fretHandMute)? true : false; }
		bool getHammerOn() { return (technique == hammerOn)? true : false; }
		bool getHarmonic() { return (technique == harmonic)? true : false; }
		bool getPalmMute() { return (technique == palmMute)? true : false; }
		 
		void setDuration(float d) { duration = d; };
		// This should only be called after we have the right tuning.
		void setFret() { fret = pitch - stringPitch[string]; };	
		void setTechnique(eTechnique t, int d) 
			{ 
			technique = t; techDif = d; 
			if (t == bendHalf) setBend(1.000);
			else if(t == bendFull) setBend(2.000);	
			else if(t == fretHandMute) fretHandMute = true;
			else if(t == hammerOn) hammerOn = true;
			else if(t == harmonic) harmonic = true;
			else if(t == palmMute) palmMute = true;
	/*pickUp, pickDown,
	pinchHarmonic,
	pullOff,
	slide, slideUnpitch,
	// I'm guessing 'leftHand' and 'rightHand' are related to tapping.
	tapLeft, tapRight, 
	tremolo,
	vibrato,
	// Bass
	slap, pluck, */
			};
		void setSlide(int fret) { slide = fret; };
		// void setBend(float bT, float bS) { bendTime = bT; bendStep = bS; };
		void setBend(float b) { bendTime = time; bendStep = b; };
	};
	
Note::Note(float t, int s, int p, int d)
	{
	time = t;
	duration = 0;
	string = s;
	pitch = p;
	fret = -1;
	minDif = d;
	
	technique = none; techDif = -1;
	slide = -1;
	bendTime = -1.0;
	bendStep = -1.0;
	}
	
Note::Note(const Note& n)
	{
	time = n.time;
	duration = n.duration;
	string = n.string;
	pitch = n.pitch;
	fret = n.fret;
	minDif = n.minDif;
	
	technique = n.technique; techDif = n.techDif;
	slide = n.slide;
	bendTime = n.bendTime;
	bendStep = n.bendStep;
	}
	
// Functions
float convertTempo2TimeFloat(float tempo)
	{
	float f = 0.0;
	float beat = ONEMINUTEMILLI / tempo;
	f = beat / ONESECONDMILLI;
	return f;
	};
	
// Common methods
/* template <class T>	
void addX(T source, std::vector<T>& dest) { dest.push_back(source); } */
	
template <class T>
void addXs(const std::vector<T>& source, std::vector<T>& dest)
	{
	for(typename std::vector<T>::const_iterator it = source.begin();
		it != source.end(); ++it)
		{ dest.push_back(*it); }
	}

/* template <class X>
X getX(std::vector<X> source, int i)
	{ return source.at(i); } */
	
/* template <class X>
std::vector<X> getXs(std::vector<X> source)
	{ return source; } */

template <class T>
std::vector<T> getXsWithinTime(std::vector<T> source, float a, float b)
	{
	std::vector<T> x;
	float t = 0.0;
	for(typename std::vector<T>::iterator it = source.begin();
		it != source.end(); ++it)
		{
		T cX(*it);
		t = cX.getTime();
		if(t > b) { break; }
		if(t >= a) { x.push_back(cX); }
		}
	return x;
	}

	
/* template <class T>
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
	} */
	
#endif