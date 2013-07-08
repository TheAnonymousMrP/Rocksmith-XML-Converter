#ifndef BASE
#define BASE

#include <string>
#include <vector>

#define NUMSTRINGS 6
#define NUMFRETS 24

#define ONEMINUTE 60.000
#define ONEMINUTEMILLI 60000
#define ONEMINUTEMICRO 60000000
#define ONESECONDMILLI 1000
#define DEFAULTTIMESIGNUM 4 // Default numerator for the time signature
#define DEFAULTTIMESIGDEN 4 // Default denominator for the time signature

// #define GOTHERE std::cout << "Got here!\n";

enum eMeta {
	anchor,
	bend,
	chord,
	event,
	lyrics,
	marker,
	phrase,
	tech,
	special,
};
	
enum eTechnique {
	none = -1,
	accent,
	// Bends may need to be expanded in the future with new bend additions.
	bendHalf = 1, bendFull,
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
	
enum eTuning {
	standardE,
	dropD,
	standardD,
};

namespace tuning {
	const int standardE[NUMSTRINGS] = { 52, 57, 62, 67, 71, 76 }; 
	const int dropD[NUMSTRINGS] = { 50, 57, 62, 67, 71, 76 };
};

// Functions
float convertTempo2Beat(const float& tempo) {
	return ONEMINUTE / tempo;
};
	
int getTuning(eTuning tuning, int i) {
	switch(tuning) {
		case eTuning::standardE: return tuning::standardE[i]; 
		case eTuning::dropD: return i = tuning::dropD[i];
		default: return i = tuning::standardE[i];
	}
};
	
struct Meta {
	float time;
	std::string text;
};
	
struct Tempo {
	float time;
	float tempo;
};
	
struct TimeSig {
	float time;
	int num; // Numerator. 
	int denom; // Denominator. Negative power of 2.
	int clock; // MIDI clocks in a metronome click.
	int quart; // 32nd notes per quarter-note.
};
	
class BaseNote {
	protected:
		float time;
		float duration;
		int pitch;
		
	public:
		BaseNote(float t = -1.0, float d = -1.0, int p = -1)
			{ time = t; duration = d; pitch = p; };
	
		const float& getTime() const { return time; };
		const float& getDuration() const { return duration; };
		const int& getPitch() const { return pitch; };
		
		void setDuration(float d) { duration = d; };
};

class Note : public BaseNote {
	// Private
	int string;
	int fret;
	// int maxDif;
	
	// Techniques
	int techDif; // Difficulty when technique is added.
	
	static eTuning tuning;
	
	public:
		Note(float t = -1.0, int s = -1, int p = -1, int d = -1);
		// Note(float t, float dur, int s, int p, int dif);
		// Note(const Note& n); // Copy Constructor
		~Note() { };
		
		unsigned int minDif;
		
		// Techniques
		bool accent, fretHandMute, hammerOn, harmonic, palmMute, 
		pinchHarmonic, pullOff, tremolo, vibrato;
		int pick, tapLeft, tapRight, slide, slideUnpitch, slap, pluck;
		float bendTime, bendStep; // if the bend is expanded as expected.
		
		const int& getString() const { return string; };
		const int& getFret() const { return fret; };
		// Techniques
		const bool isBend() const { return (bendStep > 0)? true: false; };
		 
		void setBend(float b) { bendTime = time; bendStep = b; }; 
		// This should only be called after we have the right tuning.
		void setFret() { fret = pitch - getTuning(tuning, string); };	
		void setTechnique(eTechnique t, int d) { 
			techDif = d; 
			switch(t) {
				case eTechnique::accent: accent = true; break;
				case eTechnique::bendHalf: 
				case eTechnique::bendFull: setBend(t); break;
				case eTechnique::fretHandMute: fretHandMute = true; break;
				case eTechnique::hammerOn: hammerOn = true; break;
				case eTechnique::harmonic: harmonic = true; break;
				case eTechnique::palmMute: palmMute = true; break;
				case eTechnique::pickUp: pick = 1; break;
				case eTechnique::pickDown: pick = 0; break;
				case eTechnique::pinchHarmonic: pinchHarmonic = true; break;
				case eTechnique::pullOff: pullOff = true; break;
				case eTechnique::tapLeft: tapLeft = 1; break;
				case eTechnique::tapRight: tapRight = 1; break;
				case eTechnique::tremolo: tremolo = true; break;
				case eTechnique::vibrato: vibrato = true; break;
				case eTechnique::slap: slap = 1; break;
				case eTechnique::pluck: pluck = 1; break;
				
				// No effect;
				case eTechnique::none: break;
				case eTechnique::slide: break;
				case eTechnique::slideUnpitch: break;
				
			}
		};
		
		static void setTuning(eTuning t) { tuning = t; }
		static int findLowestFret(const std::vector<Note>& source, 
			const int& min = 0) 
			{
			int low = NUMFRETS;
			for(const Note& n : source) 
				{ (n.fret < low && n.fret >= min)? low = n.fret: low; }
			return low;
		}
		
		// Debug only.
		const int& getPitch() const { return pitch; }
};

eTuning Note::tuning = eTuning::standardE;
	
Note::Note(float t, int s, int p, int d) {
	time = t;
	duration = 0;
	string = s;
	pitch = p;
	fret = -1;
	minDif = d;
	
	techDif = -1;
	accent = false; fretHandMute = false; hammerOn = false; harmonic = false;
	palmMute = false; pinchHarmonic = false; pullOff = false; tremolo = false;
	vibrato = false;
	pick = 0; tapLeft = -1; tapRight = -1; slide = -1; slideUnpitch = -1;
	slap = -1; pluck = -1;
	
	bendTime = -1.0; bendStep = -1.0;
}
	
/* Note::Note(const Note& n) {
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
} */
	
// Common methods
/* template <class T>	
void addX(T source, std::vector<T>& dest) { dest.push_back(source); } */
	
template <class T>
void addXs(const std::vector<T>& source, std::vector<T>& dest) {
	for(auto it = source.begin(); it != source.end(); ++it)
		{ dest.push_back(*it); }
}

/* template <class X>
X getX(std::vector<X> source, int i)
	{ return source.at(i); } */
	
/* template <class X>
std::vector<X> getXs(std::vector<X> source)
	{ return source; } */
	
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