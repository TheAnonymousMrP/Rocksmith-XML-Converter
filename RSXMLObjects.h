#ifndef _RSXML_OBJECTS_
#define _RSXML_OBJECTS_

#ifndef _BASE_OBJECTS
#include "BaseObjects.h"
#endif

#ifndef _RSXML_STRUCTURE
#include "RSXMLStructure.h"
#endif

#include <string>
#include <sstream>
#include <vector>

enum eNoteValues {
	ACCENT,
	BEND_HALF,
	BEND_FULL,
	FRETHANDMUTE,
	HOPO,
	HAMMERON,
	HARMONIC,
	IGNORE,
	LINKNEXT,
	PALMMUTE,
	PICK_DIRECTION,
	PINCHHARMONIC,
	PULLOFF,
	SLIDE,
	SLIDE_UNPITCH,
	TAP,
	TAP_LEFT,
	TAP_RIGHT,
	TREMOLO,
	VIBRATO,
	BASS_PLUCK,
	BASS_SLAP,
};

enum eChordValues {
	ACCENT,
	FRETHANDMUTE,
	HIGHDENSITY,
	IGNORE,
	LINKNEXT,
	PALMMUTE,
	STRUM,
};

namespace RSXML {
	class LevelObject : public virtual BaseObject {
		public:
			RSXMLObject( const float& tim = 0.000f, const unsigned int& ind = 0 ) 
				: BaseObject( tim ) { index = id; };
			
			const unsigned int& 	GetIndex() const { return index; };
			
		private:
			unsigned int			index;
	};	
	
	class Note : public Base::GuitarNote, public LevelObject { 
		public:
			Note( const float& tim = -1.0, const unsigned char& str = 0x00, 
				const unsigned char& pit = 0x00, const unsigned char& dif = 0x00,
				const unsigned int& in = 0 ) 
				: GuitarNote( tim, pit, str, dif ), RSXMLObject( tim, in ) {
				for( auto it = values.begin(); it != values.end(); ++it ) { *it = false; }  
			};
				
			bool							values[eNoteValues];
			
			const std::string				ToXML( const bool& chord = false );
			
			static const std::vector<Note>&	GetVector() const { return vector; };
			static void						SetVector( const std::vector<Note>& v ) 
												{ vector = v; };
		private:
			static const std::vector<Note>	vector;
	};
	
	const std::string Note::ToXML( const bool& chord = false ) const {
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
		<< (int)values[eNoteValues::IGNORE] << "\" linkNext=\"" << (int)LINKNEXT 
		<< "\" bend=\"" << IsBend() 
		<< "\" harmonic=\"" << (int)values[eNoteValues::HARMONIC] 
		<< "\" hopo=\"" << (int)values[eNoteValues::HOPO] 
		<< "\" hammerOn=\"" << (int)values[eNoteValues::HAMMERON] 
		<< "\" pullOff=\"" << (int)values[eNoteValues::PULLOFF] 
		<< "\" palmMute=\"" << (int)values[eNoteValues::PALMMUTE] 
		<< "\" slideTo=\""; ( slide ) ? ss << slide : ss << -1; 
		ss << "\" tremolo=\"" << (int)values[eNoteValues::TREMOLO]
		// New stuff
		<< "\" accent=\"" << (int)values[eNoteValues::ACCENT] 
		<< "\" leftHand=\"" << ( values[eNoteValues::TAPLEFT] * 2 - 1 )
		<< "\" rightHand=\"" << ( values[eNoteValues::TAPRIGHT] * 2 - 1 )
		<< "\" harmonicPinch=\"" << (int)values[eNoteValues::PINCHHARMONIC] 
		<< "\" mute=\"" << (int)values[eNoteValues::FRETHANDMUTE] 
		<< "\" pickDirection=\"" << (int)values[eNoteValues::PICK_DIRECTION] 
		<< "\" slideUnpitchTo=\"" << ( values[eNoteValues::SLIDEUNPITCH] * 2 - 1 )
		<< "\" tap=\"" << (int)values[eNoteValues::TAP] 
		<< "\" vibrato=\"" << (int)values[eNoteValues::VIBRATO]
		// Bass
		<< "\" pluck=\"" << ( values[eNoteValues::PLUCK] * 2 - 1 ) 
		<< "\" slap=\"" << ( values[eNoteValues::SLAP] * 2 - 1 )
		<< "\" bendValues=\""; << ( IsBend ) : ss << time << "," << bend : ss << -1;
		ss << "\" />\n";
		
		return ss.str();
	};
	
	class Chord : public Base::Chord, public LevelObject, public TemplateSync {
		public:
			Chord( const float& tim = -1.0, const array<unsigned int, 6>& nIn
				const unsigned int& i = 0, const unsigned int& in = 0 ) 
				: Base::Chord( tim, nIn ), LevelObject( tim, in ), TemplateSync( i ) { 
				for( auto it = values.begin(); it != values.end(); ++it ) { *it = false; } 
			};
		
			bool					values[eChordValues];
			
			const std::string		ToXML();
	};
	
	const std::string Chord::ToXML() const {
		std::stringstream ss("");
		ss << "<chord time=\"" << time 
		<< "\" linkNext=\"" << (int)values[eChordValues::LINKNEXT] 
		<< "\" accent=\"" << (int)values[eChordValues::ACCENT] 
		<< "\" chordId=\"" << id 
		<< "\" fretHandMute=\"" << (int)values[eChordValues::FRETHANDMUTE] 
		<< "\" highDensity=\"" << (int)values[eChordValues::HIGHDENSITY] 
		<< "\" ignore=\"" << (int)values[eChordValues::IGNORE] 
		<< "\" palmMute=\"" << (int)values[eChordValues::PALMMUTE] 
		<< "\" strum=\""; ( values[eChordValues::STRUM] ) ? ss << "up" : ss << "down";
		ss << "\">\n";
		
		auto& nSource = Note::GetVector();
		auto& nIndex = GetNoteIndexes();
		for( auto& it = nIndex.begin(); it != nIndex.end(); ++it ) 
			{ ss << nSource.at( *it ).ToXML( true ); };
		
		ss << "</chord>\n";
		
		return ss.str();
	};
	
	class Anchor : public LevelObject {
		public:
			Anchor( const float& tim = 0.000f, const unsigned char& fre = 1,
				const float& wid = 4.000f, const unsigned int& in = 0 ) 
				: LevelObject( tim, in ) { fret = fre; width = wid; };
				
			const unsigned char&	GetFret() const { return fret; };
			const float&			GetWidth() const { return width; };
			
			const std::string		ToXML() const;
			
		private:
			unsigned char&			fret;
			float					width;
	};
	
	const std::string Anchor::ToXML() const {
		std::stringstream ss;
		ss << "<anchor time=\"" << time << "\" fret=\"" << (unsigned int)fret 
		<< "\" width=\"" << width << "\" />\n";
		return ss.str();
	};
	
	class HandShape : public LevelObject, public TemplateSync { 
		public:
			HandShape( const float& tim = -1.0, const unsigned int& i = 0, 
				const float& dur = 0.000f, const unsigned int& in = 0 )
				: LevelObject( tim, in ), TemplateSync( i ) { duration = dur; };
				
			const float&		GetDuration() const { return duration; };
			
			const std::string	ToXML() const;	
		
		private:
			float				duration;
	
	};
	
	const std::string HandShape::ToXML() const {
		std::stringstream ss;
		ss << "<handShape startTime=\"" << time << "\" endTime=\"" << time + duration 
		<< "\" chordId=\"" << id << "\" />\n";
		return ss.str();
	};
};





#endif