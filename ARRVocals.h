#ifndef ARR_VOCALS
#define ARR_VOCALS

#ifndef BASE_NOTE
#include "BaseNote.h"
#endif

#include <string>
#include <vector>

namespace ARR {
	class Vocals {
		public:
			Vocals( const std::string& nam, std::vector<Base::Lyric>& lyr ) { name = nam; lyrics = lyr; };
			Vocals(const Vocals& a) : name( a.name ), lyrics( a.lyrics ) { };
			
			const Base::Lyric& 				GetLyric( unsigned int i ) const 
												{ return lyrics.at(i); };
			const std::vector<Base::Lyric>& GetLyrics() const { return lyrics; };
			
		private:
			std::string 					name;
			std::vector<Base::Lyric> 		lyrics;
	};
};



#endif