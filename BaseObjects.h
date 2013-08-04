#ifndef BASE_OBJECTS
#define BASE_OBJECTS

#ifndef BASE_NOTE
#include "BaseNote.h"
#endif

#ifndef BASE_META
#include "BaseMeta.h"
#endif

#include <array>
#include <string>
#include <vector>

#define CHORDERROR 65365

namespace Base {
	class VectorEmptyException {
		public:
			VectorEmptyException( std::string vectorType ) : vectorType( vectorType ) { };

			std::string				what() const throw() { 
				return "Attempted to parse an empty " + vectorType + " vector.";
			};

		private:
			const std::string		vectorType;
		};

	const std::array<unsigned int, NUMSTRINGS> DEFAULTINDEX = { { CHORDERROR, CHORDERROR, CHORDERROR, CHORDERROR, CHORDERROR, CHORDERROR } };

	class Chord : public virtual Base::BaseObject {
		public:
			Chord( const float& tim = 0.000f, 
				std::array<unsigned int, NUMSTRINGS> nIn = DEFAULTINDEX ) 
				: BaseObject( tim ), notesIndex( nIn ) { };
			
			const std::array<unsigned int, NUMSTRINGS>&	
				GetNotesIndex() const { return notesIndex; };
			
			const unsigned int GetSize() const { 
				for( auto it = notesIndex.begin(); it != notesIndex.end(); ++it ) { 
					if ( *it == CHORDERROR && ( it - notesIndex.begin() ) != 0 )
						{ return ( it - notesIndex.begin() + 1 ); }
				}
				return NUMSTRINGS;
			};
													
		protected:
			std::array<unsigned int, NUMSTRINGS>	notesIndex;
	};
	
	class Arrangement {
		public:
			Arrangement( const float& dur = 0.000f, const std::string& nam = "" ) 
				: duration( dur ), name( nam ) { };
	
			const float&		GetDuration() const { return duration; };
			const std::string&	GetName() const { return name; };		
		
		private:
			float				duration;
			std::string			name;
	};
	
	class Guitar : public Base::Arrangement {
		public:
			Guitar( const float& dur = 0.000f, const std::string& nam = "", const bool& bas = false ) 
				: Arrangement( dur, nam ), tuning( Base::aTuning[eTuning::STANDARD_E] ) { bass = bas; };
			Guitar( const Guitar& g ) : Arrangement( g ), tuning( g.tuning ), 
				bass( g.bass ), tempos( g.tempos ), timeSigs( g.timeSigs ) { };
			
			Base::Tuning					tuning;
			
			const std::vector<Base::Tempo>&	GetTempos() const { return tempos; };
			const std::vector<Base::TimeSig>& GetTimeSigs() const { return timeSigs; };
			
			const bool&						IsBass() const { return bass; };
			
			void	SetTempos( const std::vector<Base::Tempo>& v ) { tempos = v; };
			void	SetTimeSigs( const std::vector<Base::TimeSig>& v ) { timeSigs = v; };
		
		protected:
			bool							bass;	
			
			std::vector<Base::Tempo>		tempos;
			std::vector<Base::TimeSig>		timeSigs;
	};
};

#endif