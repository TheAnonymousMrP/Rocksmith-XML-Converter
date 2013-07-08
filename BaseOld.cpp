#ifndef BASE
#include "BaseOld.h"
#endif

// Functions
float convertTempo2Beat(const float& tempo) {
	return ( float )ONEMINUTE / tempo;
};

int tuning::getTuningForString(eTuning tuning, int i) {
	switch(tuning) {
		case eTuning::standardE: return tuning::standardE[i]; 
		case eTuning::dropD: return i = tuning::dropD[i];
		default: return i = tuning::standardE[i];
	}
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

void Note::setFret() { fret = pitch - tuning::getTuningForString(tuning, string); };