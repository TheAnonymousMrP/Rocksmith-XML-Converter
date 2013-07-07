#ifndef _BASE_OBJECTS_
#define _BASE_OBJECTS_

#ifndef _BASE_NOTE
#include "BaseNote.h"
#endif

#ifndef _BASS_META
#include "BaseMeta.h"
#endif

#include <array>
#include <string>
#include <vector>

namespace Base {
	const std::array<unsigned int, NUMSTRINGS> DEFAULTINDEX { { 0, 0, 0, 0, 0, 0 } };

	class Chord : public virtual BaseObject {
		public:
			Chord( const float& tim = 0.000f, 
				std::array<unsigned int, NUMSTRINGS> nIn = DEFAULTINDEX ) 
				: BaseObject( tim ), notesIndex( nIn ) { };
			
			const std::array<unsigned int, NUMSTRINGS>&	
				GetNotesIndex() const { return notesIndex; };
			
			const unsigned int GetSize() const { 
				for( auto it = notesIndex.begin(); it != notesIndex.end(); ++it ) { 
					if ( *it == 0 && ( it - notesIndex.begin() ) != 0 )
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
			Guitar( const float& dur = 0.000f, const std::string& nam = "", 
				const bool& bas = false ) : Arrangement( dur, nam ) 
				{ tuning = Base::aTuning[Base::eTuning::STANDARD_E], bass = bas; };
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