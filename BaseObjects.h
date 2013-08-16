#ifndef BASE_OBJECTS
#define BASE_OBJECTS

#ifndef BASE_NOTE
#include "BaseNote.h"
#endif

#ifndef BASE_META
#include "BaseMeta.h"
#endif

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

	const std::array<unsigned int, NUMSTRINGS> DEFAULTINDEX = { { CHORDERROR, CHORDERROR, CHORDERROR, CHORDERROR, CHORDERROR, CHORDERROR } };

	class Chord : public Base::BaseObject {
		public:
			Chord( const float& time = 0.000f, std::array<unsigned int, NUMSTRINGS> notesIndex = DEFAULTINDEX ) 
				: BaseObject( time ), notesIndex( notesIndex ) { };
			
			const std::array<unsigned int, NUMSTRINGS>&		GetNotesIndex() const { return notesIndex; };
			
			const unsigned int								GetSize() const { 
				for( auto it = notesIndex.begin(); it != notesIndex.end(); ++it ) { 
					if ( *it == CHORDERROR && ( it - notesIndex.begin() ) != 0 )
						{ return ( it - notesIndex.begin() ); }
				}
				return NUMSTRINGS;
			};
													
		protected:
			std::array<unsigned int, NUMSTRINGS>			notesIndex;
	};
	
	class Arrangement {
		public:
			Arrangement( const float& duration = 0.000f, const std::string& name = "" ) 
				: duration( duration ), name( name ) { };
	
			const float&		GetDuration() const { return duration; };
			const std::string&	GetName() const { return name; };		
		
		private:
			float				duration;
			std::string			name;
	};
	
	class Guitar : public Base::Arrangement {
		public:
			Guitar( const float& duration = 0.000f, const std::string& name = "", const bool& isBass = false ) 
				: Arrangement( duration, name ), tuning( Base::aTuning[eTuning::STANDARD_E] ), bass( isBass ) { };
			Guitar( const Guitar& g ) : Arrangement( g ), tuning( g.tuning ), 
				bass( g.bass ), tempos( g.tempos ), timeSigs( g.timeSigs ) { };
			
			Base::Tuning					tuning;
			unsigned int					quantize;
			
			const bool& IsBass() const { return bass; };
			const std::vector<Base::Tempo>&	GetTempos() const { return tempos; };
			const std::vector<Base::TimeSig>& GetTimeSigs() const { return timeSigs; };
			
			void	SetAsBass( const bool& isBass = true ) { bass = isBass; };
			void	SetTempos( const std::vector<Base::Tempo>& v ) { tempos = v; };
			void	SetTimeSigs( const std::vector<Base::TimeSig>& v ) { timeSigs = v; };
		
		protected:		
			bool							bass;

			std::vector<Base::Tempo>		tempos;
			std::vector<Base::TimeSig>		timeSigs;
	};

}

#endif