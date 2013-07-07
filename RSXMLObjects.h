#ifndef _RSXML_OBJECTS_
#define _RSXML_OBJECTS_

#ifndef _BASE_OBJECTS
#include "BaseObjects.h"
#endif

#ifndef _RSXML_STRUCTURE
#include "RSXMLStructure.h"
#endif

#include <array>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>

namespace RSXML {
	typedef	Base::Tempo		Tempo;
	typedef	Base::TimeSig	TimeSig;
	
	class Lyric : public Base::Lyric {
		public:
			Lyric( const Base::Lyric& l ) : Base::Lyric( l ) { };
		
			const std::string	ToXML() const;
	};
	
	const std::string Lyric::ToXML() const {
		std::stringstream ss;
		ss << "<vocal time=\"" << time << "\" note=\"" << pitch << "\" length=\"" 
		<< duration << "\" lyric=\"" << word << "\"/>\n";
		return ss.str();
	};
	
	class LevelObject : public virtual Base::BaseObject {
		public:
			LevelObject( const float& tim = 0.000f, const unsigned int& ind = 0 ) 
				: Base::BaseObject( tim ), index( ind ) { };
			
			const unsigned int& 	GetIndex() const { return index; };
			
		private:
			unsigned int			index;
	};	
	
	class Note : public Base::GuitarNote, public LevelObject { 
		public:
			Note( const float& tim = -1.0, const unsigned char& str = 0x00, 
				const unsigned char& pit = 0x00, const unsigned char& dif = 0x00,
				const unsigned int& in = 0 ) 
				: Base::GuitarNote( tim, pit, str, dif ), LevelObject( tim, in )
				{ for( auto& b : values ) { b = false; } };
			Note( const Base::GuitarNote& note, const unsigned int& index ) 
				: Base::GuitarNote( note ), LevelObject( note.GetTime(), index ) { };
			
			enum eValues {
				ACCENT,
				BEND_HALF,
				BEND_FULL,
				FRETHANDMUTE,
				HOPO,
				HOPO_ON, // 'Hammer on'
				HOPO_OFF, // 'Pull off'
				HARMONIC,
				IGNORE,
				LINKNEXT,
				PALMMUTE,
				PICK_DIRECTION,
				PINCHHARMONIC,
				SLIDE,
				SLIDE_UNPITCH,
				TAP,
				TAP_LEFT, // 'Left hand'
				TAP_RIGHT, // 'Right hand'
				TREMOLO,
				VIBRATO,
				BASS_PLUCK,
				BASS_SLAP,
				size,
			};
				
			bool					values[eValues::size];
			
			const std::string		ToXML( const bool& chord = false ) const;
			
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
	
	class Chord : public Base::Chord, public LevelObject, public TemplateSync {
		public:
			Chord( const float& tim = 0.000f, 
				const std::array<unsigned int, 6>& nIn = Base::DEFAULTINDEX,
				const unsigned int& i = 0, const unsigned int& in = 0 ) 
				: Base::Chord( tim, nIn ), LevelObject( tim, in ), TemplateSync( i )  
				{ values.fill( false ); };
		
			enum eValues {
				ACCENT,
				FRETHANDMUTE,
				HIGHDENSITY,
				IGNORE,
				LINKNEXT,
				PALMMUTE,
				STRUM,
				size,
			};
		
			std::array<bool,eValues::size>	values;
			
			// Doesn't write notes ToXML(). Pass via argument to insert where appropriate.
			const std::string				ToXML( const std::string& notes = "" ) const;
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
	
	class Anchor : public LevelObject {
		public:
			Anchor( const float& tim = 0.000f, const unsigned char& fre = 1,
				const float& wid = 4.000f, const unsigned int& in = 0 ) 
				: LevelObject( tim, in ), fret( fre ), width( wid ) { };
				
			const unsigned char&				GetFret() const { return fret; };
			const float&						GetWidth() const { return width; };
			
			const std::string					ToXML() const;
			
		private:
			unsigned char						fret;
			float								width;
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
				: LevelObject( tim, in ), TemplateSync( i ), duration( dur ) { };
				
			const float&					GetDuration() const { return duration; };
			
			const std::string				ToXML() const;	

		private:
			float							duration;
	};
	
	const std::string HandShape::ToXML() const {
		std::stringstream ss;
		ss << "<handShape startTime=\"" << time << "\" endTime=\"" << time + duration 
		<< "\" chordId=\"" << id << "\" />\n";
		return ss.str();
	};
	
	class Difficulty : public LevelObject {
		public:
			Difficulty( const float& len = 0.000f, const unsigned int& i = 0, 
				const bool& trans = false ) : LevelObject( 0.000f, i ), length( len ),
				transcription( trans ) { };
			
			const float& GetLength() const { return length; };
			
			const std::vector<unsigned int>& 
				GetNotesIndex() const { return notesIndex; };
			const std::vector<unsigned int>& 	
				GetChordsIndex() const { return chordsIndex; };
			const std::vector<unsigned int>& 	
				GetAnchorsIndex() const { return anchorsIndex; };
			const std::vector<unsigned int>&
				GetHandShapesIndex() const { return handsIndex; };
			
			const bool&		IsTranscription() const { return transcription; };
			
			void 			SetNotesIndex( const std::vector<unsigned int>& v ) 
								{ notesIndex = v; };
			void			SetChordsIndex( const std::vector<unsigned int>& v ) 
								{ chordsIndex = v; };
			void 			SetAnchorsIndex( const std::vector<unsigned int>& v ) 
								{ anchorsIndex = v; };
			void 			SetHandShapesIndex( const std::vector<unsigned int>& v ) 
								{ handsIndex = v; };
		
			// Too 'complex' to write ToXML(). Use Writer::WriteDifficulty() instead.

		private:
			float						length;
			bool						transcription;
		
			std::vector<unsigned int>	notesIndex;
			std::vector<unsigned int>	chordsIndex;
			std::vector<unsigned int>	anchorsIndex;
			std::vector<unsigned int>	handsIndex;
	};
};





#endif