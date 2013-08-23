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

#define DEFAULTANCHORWIDTH 4.000f

namespace RSXML {
	template<class X>
	const std::vector<X> GetLevelObjectsWithinTime( std::vector<X> source, float start, float end ) {
		std::vector<X> dest;
		if( source.empty() ) { throw Base::VectorEmptyException( "Source", "RSXML::GetLevelObjectsWithinTime" ); }
		for(auto& x : source) { 
			if( x.GetTime() >= end ) { break; }
			if( x.GetTime() >= start )
				{ dest.push_back( x ); }
		}
		return dest;
	}

	template <class X>
	const std::vector<X> GetLevelObjectsFromIndexesWithinTime( std::vector<X> xSource, std::vector<unsigned int> iSource, float a, float b ) {
		std::vector<X> x;
		if( xSource.empty() ) { throw Base::VectorEmptyException( "Source", "RSXML::GetLevelObjectsFromIndexesWithinTime" ); }
		else if( iSource.empty() ) { throw Base::VectorEmptyException( "Index", "RSXML::GetLevelObjectsFromIndexesWithinTime" ); }
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
			LevelObject( const unsigned int& index = 0 ) : index( index ) { };
			
			const unsigned int& 	GetIndex() const { return index; };

			class IndexRangeError {
				public:
					IndexRangeError( std::string vectorType, std::string location = "", unsigned int lineNumber = 0 ) 
						: vectorType( vectorType ), location( location ), lineNumber( lineNumber ) { };

					std::string				what() const throw() { 
						std::string buffer = "RSXML Error: Index not within bounds for " + vectorType + " vector";
						if( location != "" ) { 
							buffer += " in "; buffer += location; 
							if( lineNumber != 0 ) { buffer += " at line " ; buffer += (unsigned int)lineNumber; }
						} 
						buffer += ".";
						return buffer.c_str();
					};

				private:
					const std::string		vectorType;
					const std::string		location;
					unsigned int			lineNumber;
			};

		protected:
			unsigned int			index;
	};	
	
	class Note : public Base::GuitarNote, public LevelObject { 
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

			Note( const float& time = -1.0, const unsigned char& string = 0x00, const unsigned char& pitch = 0xFF, 
				const unsigned char& dif = 0xFF, const unsigned int& index = 0 ) : Base::GuitarNote( time, pitch, string, dif ), 
				LevelObject( index ) { values.fill( false ); };
			Note( const Note& note ) : Base::GuitarNote( note ), LevelObject( note ), values( note.values ) { };
			Note( const Base::GuitarNote& note, const unsigned int& index ) : Base::GuitarNote( note ), LevelObject( index ) { values.fill( false ); };
				
			std::array<bool,eValues::size>	values;
			
			void							SetDuration( const float& dur = 0.000f ) { duration = dur; };

			const std::string				ToXML( const float& quantize = 0.000f, const bool& chord = false ) const;
			
	};

	enum eChordTechniques {
		ACCENT,
		FRETHANDMUTE,
		HIGHDENSITY,
		IGNORE,
		LINKNEXT,
		PALMMUTE,
		STRUM,
		size,
	};
	
	class Chord : public Base::Chord, public LevelObject {
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

			Chord( const float& time = 0.000f, const std::array<unsigned int, 6>& notesIndex = Base::DEFAULTINDEX, const unsigned int& chordID = 0, 
				const unsigned int& index = 0 ) : Base::Chord( time, notesIndex ), LevelObject( index ), chordID( chordID ) { values.fill( false ); };
			Chord( const Chord& chord ) : Base::Chord( chord ), LevelObject( chord ), values( chord.values ), chordID( chord.chordID ) { };
			Chord( const Base::Chord& g, const std::array<bool,eValues::size> values, const unsigned int& chordID = 0, const unsigned int& index = 0 ) 
				: Base::Chord( g ), LevelObject( index ), chordID( chordID ), values( values ) { };
				
			std::array<bool,eValues::size>	values;

			const unsigned int&				GetChordID() { return chordID.id; };
			
			// Doesn't write ToXML() for notes. Pass these via argument to insert where appropriate.
			const std::string				ToXML( const std::string& notes = "" ) const;

		private:
			Template						chordID;
	};

	class Chord2 : public Base::Chord2<RSXML::Note> {
		public:
			Chord2( std::vector< std::shared_ptr<RSXML::Note> > notePointers, const unsigned int& chordID ) 
				: Base::Chord2<RSXML::Note>( notePointers ), chordID( chordID ) { values.fill( false ); };

			std::array<bool, eChordTechniques::size>			values;

			const unsigned int&									GetID() const { return chordID.id; }

		private:
			Template											chordID;
	};
	
	class Anchor : public LevelObject {
		public:
			Anchor( const float& time = 0.000f, const unsigned char& fret = 1,
				const float& width = 4.000f, const unsigned int& index = 0 ) 
				: LevelObject( index ), time( time ), fret( fret ), width( width ) { };
			
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
			HandShape( const float& time = -1.0, const unsigned int& chordID = 0, 
				const float& duration = 0.000f, const unsigned int& index = 0 )
				: LevelObject( index ), Template( chordID ), time( time ), duration( duration ) { };
			
			const float&					GetTime() const { return time; };	
			const float&					GetDuration() const { return duration; };
			
			const std::string				ToXML() const;	

		private:
			float							time;
			float							duration;
	};
	
	class Difficulty : public LevelObject {
		public:
			Difficulty( const float& length = 0.000f, const unsigned int& index = 0, const bool& isTranscription = false ) 
				: LevelObject( index ), length( length ), transcription( isTranscription ) { };
			Difficulty( const Difficulty& d, const bool& isTranscription ) : LevelObject( d ), length( d.length), transcription( isTranscription ), 
				notesIndex( d.notesIndex ), chordsIndex( d.chordsIndex ), anchorsIndex( d.anchorsIndex ), handsIndex( d.handsIndex ) { };
			
			const float&						GetLength() const { return length; };
			
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