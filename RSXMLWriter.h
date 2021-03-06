#include "RSXMLCreateGuitar.h"

#ifndef DEBUG_STUFF
#include "debug.h"
#endif

#ifndef _ARR_VOCALS
#include "ARRVocals.h"
#endif

#ifndef _RSXML_GUITAR
#include "RSXMLGuitar.h"
#endif

#include <iostream>
#include <fstream>
#include <sstream>

#define DEFAULTOFFSET 5.250f /* Default offset (in seconds) for songs.
An arbitrary value; still need to fully comprehend how it works. */
#define DEFAULTOFFSETCENT 0.000f // No idea.
#define DEFAULTSTARTBEAT 0.000f // No idea.

namespace RSXML {
	
	class Writer {
		public:
			Writer( const std::string& file, const std::string& title = "TITLE", const bool& debug = false )
				: fileName( file ), songName( title ), avgTempo( DEFAULTTEMPO ), debug( debug ) { };
			
			void WriteGuitar( const RSXML::Guitar& g ); 
			void WriteVocals( const ARR::Vocals& v ); 
			
		private:
			std::string 					fileName, songName;
			const bool						debug;
			
			RSXML::Guitar					arrangement;
			
			float 							avgTempo; // May have no in-game effect.
			
			const std::string				WriteStructure( const RSXML::Guitar& g ) const;
			const std::string				WriteDifficulties( const RSXML::Guitar& g ) const;
			const std::string 				WriteDifficulty( const RSXML::Difficulty& d ) const;
			
			const std::vector<RSXML::Lyric>	ConvertARR2RSXMLLyrics( const std::vector<Base::Lyric> source ) const;

			/* class VectorEmptyException : Base::VectorEmptyException {
				public:
					VectorEmptyException( std::string vectorType, std::string location ) : Base::VectorEmptyException( vectorType, location ) { };

					std::string				what() const throw() { 
						return "RSXML Writer error: " + Base::VectorEmptyException::what();
					}; 
			}; */
	};
};

