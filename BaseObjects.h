#ifndef BASE_OBJECTS
#define BASE_OBJECTS

#ifndef BASE_NOTE
#include "BaseNote.h"
#endif

#ifndef BASE_META
#include "BaseMeta.h"
#endif

#include <memory>
#include <vector>

#define CHORDERROR 65365

namespace Base {
	class VectorEmptyException {
		public:
			VectorEmptyException( std::string vectorType, std::string location = "", unsigned int lineNumber = 0 ) 
					: vectorType( vectorType ), location( location ), lineNumber( lineNumber ) { };

			std::string 			what() const throw() { 
				std::string buffer = "Attempted to parse an empty " + vectorType + " vector";
				if( location != "" ) { 
					buffer += " in "; buffer += location; 
					if( lineNumber != 0 ) { buffer += " at line " ; buffer += (unsigned int)lineNumber; }
				} 
				buffer += ".";
				return buffer;
			};

		private:
			const std::string		vectorType;
			const std::string		location;
			unsigned int			lineNumber;
	};

	const std::array<unsigned int, GUITARSTRINGS> DEFAULTINDEX = { { CHORDERROR, CHORDERROR, CHORDERROR, CHORDERROR, CHORDERROR, CHORDERROR } };

	class Chord {
		public:
			Chord( const float& time = 0.000f, std::array<unsigned int, GUITARSTRINGS> notesIndex = DEFAULTINDEX ) 
				: time( time ), notesIndex( notesIndex ) { };
			
			const float&									GetTime() const { return time; };
			const std::array<unsigned int, GUITARSTRINGS>&		GetNotesIndex() const { return notesIndex; };
			
			const unsigned int								GetSize() const { 
																for( auto it = notesIndex.begin(); it != notesIndex.end(); ++it ) { 
																	if ( *it == CHORDERROR && ( it - notesIndex.begin() ) != 0 )
																		{ return ( it - notesIndex.begin() ); }
																}
																return GUITARSTRINGS;
															};
													
		protected:
			float											time;
			std::array<unsigned int, GUITARSTRINGS>			notesIndex;
	};

	template <class T>
	class TimeObjectVector {
		public:
			TimeObjectVector() : pointers() { };
			TimeObjectVector( const std::vector< std::shared_ptr<T> >& timeObjectPointers ) : pointers( timeObjectPointers ) { };

			// Creates a pointer based on passed object. Probably broken.
			virtual void										Add( const T& object ) { 
																	std::shared_ptr<T> temp( new T(object) );
																	pointers.push_back( temp );
																};
			// Returns time of first note. Not infallible, but should be reliable.
			virtual const float&								GetTime() const { return pointers.front()->GetTime(); };
			// Returns duration of last note. Not infallible, but will usually be overridden.
			virtual const float&								GetDuration() const { return pointers.back()->GetDuration(); };

			virtual const std::vector< std::shared_ptr<T> >&	GetPointers() const { return pointers; };

		protected:
			std::vector< std::shared_ptr<T> >					pointers;

	};

	template <class T>
	class Chord2 : public TimeObjectVector<T> {
		public:
			Chord2() : TimeObjectVector() { };
			Chord2( std::vector< std::shared_ptr<T> > notePointers ) : TimeObjectVector( notePointers ) { };
			Chord2( TimeObjectVector<T> notePointers ) : TimeObjectVector( notePointers ) { };

			// Returns copy of vector of note-pointers. Will resize vector if it exceeds max supported strings.
			std::vector< std::shared_ptr<T> >					GetPointersResized( const unsigned int& maxNotes = GUITARSTRINGS ) { 
																	if( pointers.size() > maxNotes ) {
																		auto buffer = pointers;
																		buffer.resize( maxNotes );
																		pointers.resize( maxNotes ); 
																	} else { return pointers; }
																};
	};

	class GuitarChord : public Chord2<GuitarNote> { };

	template <class N, class C>
	class Difficulty {
		public:
			Difficulty( const unsigned int& index = 0 ) : index( index ), duration( 0.000f ) { };
			Difficulty( const std::vector< std::shared_ptr<N> >& notePointers, const std::vector< std::shared_ptr<C> >& chordPointers, const unsigned int& index = 0 ) 
				: notePointers( notePointers ), chordPointers( chordPointers ), index( index ), duration( 0.000f ) { };

			// Returns difficulty index.
			const unsigned int&									GetIndex() const { return index; }

			// Returns duration of arrangement by subtracting front().time from back().time + back().duration.
			virtual const float&								GetDuration() { 
																	if( duration == 0 && ( !notePointers.empty() || !chordPointers.empty() ) ) {
																		float end = 0.000f;
																		float endC = 0.000f;
																		if( !notePointers.empty() ) { 
																			end = notePointers.back()->GetTime() + notePointers.back()->GetDuration();
																		}
																		if( !chordPointers.empty() ) {
																			endC = chordPointers.back()->GetTime() + chordPointers.back()->GetDuration(); 
																		}
																		if( end < endC ) { end = endC; }
																		duration = end;																		
																	}
																	return duration; 
																};

			virtual const std::vector< std::shared_ptr<N> >&	GetNotePointers() const { return notePointers; }
			virtual const std::vector< std::shared_ptr<C> >&	GetChordPointers() const { return chordPointers; }

			virtual void										SetNotePointers( const std::vector< std::shared_ptr<N> >& pointers ) { notePointers = pointers; };
			virtual void										SetChordPointers( const std::vector< std::shared_ptr<C> >& pointers ) { chordPointers = pointers; }; 

		protected:
			std::vector< std::shared_ptr<N> >					notePointers;
			std::vector< std::shared_ptr<C> >					chordPointers;

			unsigned int										index;
			float												duration;
	
	};
	
	class Arrangement {
		public:
			Arrangement( const std::string& name = "", const float& duration = 0.000f ) 
				: duration( duration ), name( name ) { };
	
			const float&		GetDuration() const { return duration; };
			const std::string&	GetName() const { return name; };		
		
		private:
			float				duration;
			std::string			name;
	};
	
	class Guitar : public Base::Arrangement {
		public:
			Guitar( const float& duration = 0.000f, const std::string& name = "", const unsigned char& numStrings = GUITARSTRINGS ) 
				: Arrangement( name, duration ), tuning( Base::aTuning[eTuning::STANDARD_E] ), strings( numStrings ) { };
			Guitar( const Guitar& g ) : Arrangement( g ), tuning( g.tuning ), strings( g.strings ), tempos( g.tempos ), timeSigs( g.timeSigs ) { };
			
			Base::Tuning						tuning;
			unsigned int						quantize;
			
			bool								IsBass() const { if( strings == BASSSTRINGS ) { return true; } else { return false; } };
			const std::vector<Base::Tempo>&		GetTempos() const { return tempos; };
			const std::vector<Base::TimeSig>&	GetTimeSigs() const { return timeSigs; };
			
			void	SetAsBass() { strings = BASSSTRINGS; };
			void	SetAsGuitar( const unsigned char& numStrings = GUITARSTRINGS ) { strings = numStrings; };
			void	SetTempos( const std::vector<Base::Tempo>& v ) { tempos = v; };
			void	SetTimeSigs( const std::vector<Base::TimeSig>& v ) { timeSigs = v; };
		
		protected:		
			unsigned char						strings;

			std::vector<Base::Tempo>			tempos;
			std::vector<Base::TimeSig>			timeSigs;
	};

	class Vocals : public Base::Arrangement {
		public:
			Vocals( std::vector<Base::Lyric>& lyricVector, const std::string& name = "" ) : Arrangement( name ), lyrics( lyricVector ) { };
			Vocals(const Vocals& a) : Arrangement( a ), lyrics( a.lyrics ) { };
			
			const Base::Lyric& 				GetLyric( unsigned int index ) const { return lyrics.at( index ); };
			const std::vector<Base::Lyric>& GetLyrics() const { return lyrics; };
			
		private:
			std::vector<Base::Lyric> 		lyrics;
	};
}

#endif