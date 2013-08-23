#ifndef BASE_NOTE
#define BASE_NOTE

#include <array>
#include <string>
#include <iostream>

#define GUITARSTRINGS 6
#define BASSSTRINGS 4
#define NUMFRETS 22

namespace Base {
	// Basic time structure to reduce redundancy across objects (including Chord's?).
	class TimeObject {
		public:
			TimeObject( const float& time = 0.000f, const float& duration = 0.000f ) : time( time ), duration( duration ) { };

			const float&	GetTime() const { return time; };
			const float&	GetDuration() const { return duration; };

		protected:
			float			time;
			float			duration;

	};

	class Note : public TimeObject {
		public:
			Note( const float& time = 0.000f, const unsigned char& pitch = 0xFF, const float& duration = 0.000f ) 
				: TimeObject( time, duration ), pitch( pitch ) { };
				
			const unsigned char& 	GetPitch() const { return pitch; };
	
		protected:
			unsigned char 			pitch;
	};

	class Lyric : public Base::Note {
		public:
			Lyric( const float& tim = 0.000f, const unsigned int& pit = 0, const std::string& w = "" ) : Base::Note( tim, pit ), word( w ) { };
		
			std::string word;
	};

	enum class eInstrument {
		GUITAR6,
		BASS
	};
	
	typedef struct {
		eInstrument									instrument;
		std::string 								name;
		std::array<unsigned char, GUITARSTRINGS>	pitch;
	} Tuning;
	
	enum eTuning {
		STANDARD_E,
		DROP_D,
		STANDARD_EB,
		OPEN_G,
		size
	};
	
	const std::array<Tuning, eTuning::size> aTuning = { { 
			{ eInstrument::GUITAR6, "Standard E", { { 52, 57, 62, 67, 71, 76 } } },
			{ eInstrument::GUITAR6, "Drop D", { { 50, 57, 62, 67, 71, 76 } } },
			{ eInstrument::GUITAR6, "Standard Eb", { { 51, 56, 61, 66, 70, 75 } } },
			{ eInstrument::GUITAR6, "Open G", { { 50, 55, 62, 67, 71, 74 } } },
		} };

	const std::array<Tuning, eTuning::size> bassTuning = { { 
			{ eInstrument::BASS, "Standard E", { { 40, 45, 50, 55 } } },
			{ eInstrument::BASS, "Drop D", { { 38, 45, 50, 55 } } },
			{ eInstrument::BASS, "Standard Eb", { { 39, 44, 49, 54 } } },
			{ eInstrument::BASS, "Open G", { { 38, 43, 50, 55, 59, 62 } } },
		} };
	
	class GuitarNote : public Base::Note {
		public:
			GuitarNote( const float& time = 0.000f, const unsigned char& pitch = 0xFF, const unsigned char& string = 0x00, const float& duration = 0.000f ) 
				: Base::Note( time, pitch, duration ), normalisedDifficulty( 0x00 ), bend( 0.000f ), slide( 0 ), packed( 0xFF ), fret( 0xFF ), string( string ) { };
		
			unsigned char			normalisedDifficulty;
			float					bend;
			unsigned char			slide;
			
			const unsigned char&	GetPacked() const { return packed; };
			
			const unsigned char&	GetFret() const { return fret; };
			const unsigned char&	GetString() const { return string; };
			
			const bool				IsBend() const { 
										if( bend > 0 ) { return true; } 
										else { return false; }
									};
			
			void					SetFret( const Tuning& tun = aTuning[ eTuning::STANDARD_E ] ) { 
										if( pitch != 0xFF ) { fret = pitch - tun.pitch[ string ]; }
										/* std::cerr << "Pitch: " << (unsigned int)pitch << " String: " << (unsigned int)string << " Fret: " 
											<< (unsigned int)fret << " Tuning: " << (unsigned int)tun.pitch[ string ] << "\n"; */
									};
			// Sets 'packed', a single-char amalgamation of string and fret.
			void					SetPacked( const unsigned char& string = 0x00, const unsigned char& fret = 0xFF, const Tuning& tun = aTuning[ eTuning::STANDARD_E ] ) {
										packed = string << 5;
										packed += ( fret & 0xE0 );
									};
			
		protected:
			unsigned char			packed;

			unsigned char			fret;
			unsigned char			string;
	};
	
};

namespace RSXML {
	enum eTrackType {
		VOCAL,
		SINGLE,
		COMBO,
		CHORDS,
		BASS,
		BASS_PICK,
	};
	
	enum eTuning {
		STANDARD_E,
		DROP_D,
		STANDARD_EB,
		OPEN_G
	};
};

#endif