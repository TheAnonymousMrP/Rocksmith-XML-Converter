#ifndef _ARR_STRUCTURE_
#define _ARR_STRUCTURE_

#ifndef _ARR_OBJECTS
#include "ARRObjects.h"
#endif

#include <memory>
#include <string>

namespace ARR {
	class Beat : public Base::TimeObject {
		public:
			Beat( const float& time = 0.000f, const unsigned int& bar = 0, const unsigned char& beat = 0, 
				const unsigned char& subBeat = 0, const float& duration = 0.000f ) 
				: Base::TimeObject( time, duration ), bar( bar ), beat( beat ), subBeat( subBeat ) { };
			
			const unsigned int&		GetBar() const { return bar; };
			const unsigned char&	GetBeat() const { return beat; };
			const unsigned char&	GetSubBeat() const { return subBeat; };
				
		private:
			unsigned int			bar;
			unsigned char			beat; 
			unsigned char			subBeat;
	};

	class Chord : public Base::Chord {
		public:
			Chord( const float& time = 0.000f, const std::array<unsigned int, 6>& nIn = Base::DEFAULTINDEX, 
				const std::string& cN = "", const unsigned int& i = 0 ) : Base::Chord( time, nIn ), chordName( cN ), index( i ) { values.fill( false ); };
				
			std::string							chordName;
			unsigned int						index;
			std::array<bool,eChordTechniques::size>	values;
	};

	class Chord2 : public Base::Chord2<ARR::Note> { 
		public:
			Chord2() : Base::Chord2<ARR::Note>() { };
			Chord2( std::vector< std::shared_ptr<ARR::Note> > notePointers ) : Base::Chord2<ARR::Note>( notePointers ) { };
			Chord2( Base::TimeObjectVector<ARR::Note> notePointers ) : Base::Chord2<ARR::Note>( notePointers ) { };
	};

	class Phrase : public Base::TimeObject {
		public:
			Phrase( const float& time = 0.000f, const float& duration = 0.000f, const std::string& name = "" )
				: TimeObject( time, duration ), name( name ) { };

			const std::string&	GetName() const { return name; };
			
		private:
			std::string			name;
	};
	
	class Section : public Base::TimeObject {
		public:
			Section( const float& time = 0.000f, const float& duration = 0.000f, const std::string& name = "" )
				: TimeObject( time, duration ), name( name ) { };
			
			const std::string&	GetName() const { return name; };
				
		private:
			std::string			name;
	};
	
	class Difficulty {
		public:
			Difficulty( const float& length = 0.000f, const unsigned int& index = 0 ) : length( length ), difficulty( index ) { };
			
			const unsigned int&					GetDifficulty() const { return difficulty; }
			const float&						GetLength() const { return length; };
			
			const std::vector<unsigned int>&	GetNotesIndex() const { return notesIndex; };
			const std::vector<unsigned int>& 	GetChordsIndex() const { return chordsIndex; };
			
			void 								SetNotesIndex( const std::vector<unsigned int>& v ) 
													{ notesIndex = v; };
			void								SetChordsIndex( const std::vector<unsigned int>& v ) 
													{ chordsIndex = v; };
		
			// Too 'complex' to write ToXML(). Use Writer::WriteDifficulty() instead.

		private:
			unsigned int						difficulty;
			float								length;
		
			std::vector<unsigned int>			notesIndex;
			std::vector<unsigned int>			chordsIndex;
	};

	class Difficulty2 : public Base::Difficulty<ARR::Note, ARR::Chord2> {
		public:
			Difficulty2( const std::vector< std::shared_ptr<ARR::Note> >& notePointers, const std::vector< std::shared_ptr<ARR::Chord2> >& chordPointers, 
				const unsigned int& index = 0 ) : Base::Difficulty<ARR::Note, ARR::Chord2>( notePointers, chordPointers, index ) { };
			
			// Too 'complex' to write ToXML(). Use Writer::WriteDifficulty() instead.
	};
};

#endif