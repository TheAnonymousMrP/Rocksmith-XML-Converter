#ifndef RSXML_OBJECTS
#define RSXML_OBJECTS

#ifndef RSXML_STRUCTURE
#include "RSXMLStructure.h"
#endif

#include <array>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>

namespace RSXML {
	template <class X>
	const std::vector<X> getXsFromIsWithinTime(std::vector<X> xSource, std::vector<unsigned int> iSource, float a, float b) {
		std::vector<X> x;
		for(auto& i : iSource) { 
			if( xSource.size() > i ) {
				if( xSource.at( i ).GetTime() >= b ) { break; }
				if( xSource.at( i ).GetTime() >= a )
					{ x.push_back( xSource.at( i ) ); }
			} else { break; }
		}
		return x;
	}
	
	class Lyric : public Base::Lyric {
		public:
			Lyric( const Base::Lyric& l ) : Base::Lyric( l ) { };
		
			const std::string	ToXML() const;
	};
	
	class LevelObject {
		public:
			LevelObject( const unsigned int& ind = 0 ) 
				: index( ind ) { };
			
			const unsigned int& 	GetIndex() const { return index; };

		protected:
			unsigned int			index;
	};	
	
	class Note : virtual public Base::GuitarNote, virtual public LevelObject { 
		public:
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

			Note( const float& time = -1.0, const unsigned char& string = 0x00, 
				const unsigned char& pitch = 0xFF, const unsigned char& dif = 0xFF,
				const unsigned int& index = 0 ) 
				: Base::GuitarNote( time, pitch, string, dif ), LevelObject( index )
				{ for( auto& b : values ) { b = false; } };
			Note( const Base::GuitarNote& note, const unsigned int& index ) 
				: Base::GuitarNote( note ), LevelObject( index ) { };
				
			bool					values[eValues::size];
			
			const std::string		ToXML( const bool& chord = false ) const;
			
	};
	
	class Chord : public Base::Chord, public LevelObject, public Template {
		public:
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

			Chord( const float& time = 0.000f, const std::array<unsigned int, 6>& notesIndex = Base::DEFAULTINDEX,
				const unsigned int& chordID = 0, const unsigned int& index = 0 ) 
				: Base::Chord( time, notesIndex ), LevelObject( index ), chordID( chordID )  
				{ values.fill( false ); };
			Chord( const Base::Chord& g, const std::array<bool,eValues::size> values, const unsigned int& chordID = 0, const unsigned int& index = 0 ) 
				: Base::Chord( g ), LevelObject( index ), chordID( chordID ), values( values ) { };
				
			std::array<bool,eValues::size>	values;

			const unsigned int&				GetChordID() { return chordID.id; };
			
			// Doesn't write ToXML() for notes. Pass these via argument to insert where appropriate.
			const std::string				ToXML( const std::string& notes = "" ) const;

		private:
			Template						chordID;
	};
	
	class Anchor : public LevelObject {
		public:
			Anchor( const float& tim = 0.000f, const unsigned char& fre = 1,
				const float& wid = 4.000f, const unsigned int& in = 0 ) 
				: LevelObject( in ), time( tim ), fret( fre ), width( wid ) { };
			
			const float&					GetTime() const { return time; };	
			const unsigned char&			GetFret() const { return fret; };
			const float&					GetWidth() const { return width; };
			
			const std::string				ToXML() const;
			
		private:
			float							time;
			unsigned char					fret;
			float							width;
	};
	
	class HandShape : public LevelObject, public Template { 
		public:
			HandShape( const float& tim = -1.0, const unsigned int& i = 0, 
				const float& dur = 0.000f, const unsigned int& in = 0 )
				: LevelObject( in ), Template( i ), time( tim ), duration( dur ) { };
			
			const float&					GetTime() const { return time; };	
			const float&					GetDuration() const { return duration; };
			
			const std::string				ToXML() const;	

		private:
			float							time;
			float							duration;
	};
	
	class Difficulty : public LevelObject {
		public:
			Difficulty( const float& len = 0.000f, const unsigned int& i = 0, 
				const bool& trans = false ) : LevelObject( i ), length( len ),
				transcription( trans ) { };
			
			const float& GetLength() const { return length; };
			
			const std::vector<unsigned int>&	GetNotesIndex() const { return notesIndex; };
			const std::vector<unsigned int>& 	GetChordsIndex() const { return chordsIndex; };
			const std::vector<unsigned int>& 	GetAnchorsIndex() const { return anchorsIndex; };
			const std::vector<unsigned int>&	GetHandShapesIndex() const { return handsIndex; };
			
			const bool&		IsTranscription() const { return transcription; };
			
			void 			SetNotesIndex( const std::vector<unsigned int>& v ) { notesIndex = v; };
			void			SetChordsIndex( const std::vector<unsigned int>& v ) { chordsIndex = v; };
			void 			SetAnchorsIndex( const std::vector<unsigned int>& v ) { anchorsIndex = v; };
			void 			SetHandShapesIndex( const std::vector<unsigned int>& v ) { handsIndex = v; };
		
			// Too 'complex' to write ToXML(). Use Writer::WriteDifficulty() instead.

		private:
			float							length;
			bool							transcription;
		
			std::vector<unsigned int>		notesIndex;
			std::vector<unsigned int>		chordsIndex;
			std::vector<unsigned int>		anchorsIndex;
			std::vector<unsigned int>		handsIndex;
	};
};

#endif