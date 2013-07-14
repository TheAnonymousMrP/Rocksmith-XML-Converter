#ifndef RSXML_OBJECTS
#include "RSXMLObjects.h"
#endif

namespace RSXML {
	// Objects definitions

	const std::string Lyric::ToXML() const {
		std::stringstream ss;
		ss << "<vocal time=\"" << time << "\" note=\"" << pitch << "\" length=\"" 
		<< duration << "\" lyric=\"" << word << "\"/>\n";
		return ss.str();
	};

	const std::string Note::ToXML( const bool& chord ) const {
		/* Shit's getting real, and everything is subject to change. My bet is
		Rocksmith 2, but fuck me if they shouldn't wait for next-gen. Of course,
		it could just be shit for in-game videos, but whatever.
		
		Right, bend is probably changing to a bool and expanded upon by the new
		bend fields. It's hard to tell whether it's going to work like the leaked
		Incubus song does, where the bend fields are in the next bit, or like in
		current DLC such as Cliffs of Dover, where they are just two concatenated
		floats in one value at the end. 
		
		'mute' could be a replacement for the unimplemented fretHandMutes. */
		std::stringstream ss("");

		if(chord) { ss << "\t<chordN"; }
		else { ss << "<n"; }
		ss << "ote time=\"" << time << "\" sustain=\"" << duration << "\" string=\"" 
		<< string << "\" fret=\"" << fret << "\" ignore=\"" 
		<< (int)values[eValues::IGNORE] 
		<< "\" linkNext=\"" << (int)values[eValues::LINKNEXT] 
		<< "\" bend=\"" << IsBend() 
		<< "\" harmonic=\"" << (int)values[eValues::HARMONIC] 
		<< "\" hopo=\"" << (int)values[eValues::HOPO] 
		<< "\" hammerOn=\"" << (int)values[eValues::HOPO_ON] 
		<< "\" pullOff=\"" << (int)values[eValues::HOPO_OFF] 
		<< "\" palmMute=\"" << (int)values[eValues::PALMMUTE] 
		<< "\" slideTo=\""; ( slide ) ? ss << slide : ss << -1; 
		ss << "\" tremolo=\"" << (int)values[eValues::TREMOLO]
		// New stuff
		<< "\" accent=\"" << (int)values[eValues::ACCENT] 
		<< "\" leftHand=\"" << ( values[eValues::TAP_LEFT] * 2 - 1 )
		<< "\" rightHand=\"" << ( values[eValues::TAP_RIGHT] * 2 - 1 )
		<< "\" harmonicPinch=\"" << (int)values[eValues::PINCHHARMONIC] 
		<< "\" mute=\"" << (int)values[eValues::FRETHANDMUTE] 
		<< "\" pickDirection=\"" << (int)values[eValues::PICK_DIRECTION] 
		<< "\" slideUnpitchTo=\"" << ( values[eValues::SLIDE_UNPITCH] * 2 - 1 )
		<< "\" tap=\"" << (int)values[eValues::TAP] 
		<< "\" vibrato=\"" << (int)values[eValues::VIBRATO]
		// Bass	
		<< "\" pluck=\"" << ( values[eValues::BASS_PLUCK] * 2 - 1 ) 
		<< "\" slap=\"" << ( values[eValues::BASS_SLAP] * 2 - 1 )
		<< "\" bendValues=\""; ( IsBend() )? ss << time << "," << bend : ss << -1;
		ss << "\" />\n";
		
		return ss.str();
	};

	const std::string Chord::ToXML( const std::string& notes ) const {
		std::stringstream ss("");
		ss << "<chord time=\"" << time 
		<< "\" linkNext=\"" << (int)values[eValues::LINKNEXT] 
		<< "\" accent=\"" << (int)values[eValues::ACCENT] 
		<< "\" chordId=\"" << id 
		<< "\" fretHandMute=\"" << (int)values[eValues::FRETHANDMUTE] 
		<< "\" highDensity=\"" << (int)values[eValues::HIGHDENSITY] 
		<< "\" ignore=\"" << (int)values[eValues::IGNORE] 
		<< "\" palmMute=\"" << (int)values[eValues::PALMMUTE] 
		<< "\" strum=\""; 
		( values[eValues::STRUM] ) ? ss << "up" : ss << "down";
		ss << "\">\n";
		
		if( notes == "" ) { ss << notes; }
		
		ss << "</chord>\n";
		
		return ss.str();
	};

	const std::string Anchor::ToXML() const {
		std::stringstream ss;
		ss << "<anchor time=\"" << time << "\" fret=\"" << (unsigned int)fret 
		<< "\" width=\"" << width << "\" />\n";
		return ss.str();
	};

	const std::string HandShape::ToXML() const {
		std::stringstream ss;
		ss << "<handShape startTime=\"" << time << "\" endTime=\"" << time + duration 
		<< "\" chordId=\"" << id << "\" />\n";
		return ss.str();
	};

	// Structure definitions

	const std::string Beat::ToXML() const {
		std::stringstream ss;
		ss << "\t\t<ebeat time=\"" << time << "\" measure=\"" << bar << "\" />\n";
		return ss.str();
	};

	const std::string ChordTemplate::ToXML() const {
		std::stringstream ss;
		ss << "\t\t<chordTemplate chordName=\"" << chordName
		<< "\" displayName=\"" << displayName << "\" ";
		// Frets
		for( auto f = frets.begin(); f != frets.end(); ++f )
			{ ss << "fret" << (f - frets.begin()) << "=\"" << *f << "\" "; }
		// Fingers
		for( auto f = fingers.begin(); f != fingers.end(); ++f )
			{ ss << "finger" << (f - fingers.begin()) << "=\"" << *f << "\" "; }
		ss << " />\n";
		return ss.str();
	};

	const std::string Event::ToXML() const { 
		std::stringstream ss;
		ss << "\t\t<event time=\"" << time << "\" code=\"" << text << "\" />\n";
		return ss.str();
	};

	const std::string PhraseTemplate::ToXML() const {
		std::stringstream ss;
		ss << "\t\t<phrase name=\"" << name << "\" maxDifficulty=\""
		<< maxDifficulty << "\" disparity=\"" << disparity << "\" ignore=\"" 
		<< ignore << "\" solo=\"" << solo << "\" />\n";
		return ss.str();
	};

	const std::string Phrase::ToXML() const {
		/* Variation always seems to be incremented. This may be dependent
		on whether the contents are changed, but maybe not. Slightly 
		complicating matters is that the 'official' way seems to be to use
		roman numerals. */
		std::string var = std::to_string( (unsigned int)variation );
		std::stringstream ss;
		ss << "\t\t<phraseIteration time=\"" << time << "\" phraseId=\"" << id 
		<< "\" variation=\"" << var << "\" />\n";
		return ss.str();
	};

	const std::string Section::ToXML() const {
		std::stringstream ss;
		ss << "\t\t<section name=\"" << name << "\" number=\"" << iteration 
		<< "\" startTime=\"" << time << "\" />\n";
		return ss.str();
	};
};