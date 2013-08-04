#ifndef BASE_NOTE
#define BASE_NOTE

#include <array>
#include <string>

#define NUMSTRINGS 6
#define BASSSTRINGS 4
#define NUMFRETS 22

namespace Base {
	class BaseObject {
		public:
			BaseObject( const float& tim = 0.000f ) { time = tim; }
			
			const float&	GetTime() const { return time; };
			
		protected:
			float			time;
	};

	class Note {
		public:
			Note( const float& tim = 0.000f, const unsigned char& pit = 0xFF ) 
				: time( tim ), pitch( pit ), duration( 0.000f ) { };
				
			const float&			GetTime() const { return time; };
			float 					duration;
			const unsigned char& 	GetPitch() const { return pitch; };
	
		protected:
			float					time;
			unsigned char 			pitch;
	};
	
	typedef struct {
		std::string 							name;
		std::array<unsigned char, NUMSTRINGS>	pitch;
	} Tuning;
	
	enum eTuning {
		STANDARD_E,
		DROP_D,
		STANDARD_EB,
		OPEN_G
	};
	
	const std::array<Tuning, 10> aTuning = { { 
			{ "Standard E", { { 52, 57, 62, 67, 71, 76 } } },
			{ "Drop D", { { 50, 57, 62, 67, 71, 76 } } },
			{ "Standard Eb", { { 51, 56, 61, 66, 70, 75 } } },
			{ "Open G", { { 50, 55, 62, 67, 71, 74 } } },
		} };
	
	class GuitarNote : public Base::Note {
		public:
			GuitarNote( const float& tim = 0.000f, const unsigned char& pit = 0xFF, const unsigned char& str = 0x00, 
				const unsigned char& dif = 0 ) : Base::Note( tim, pit ), string( str ), bend( 0.000f ), slide( 0 ), 
				normalisedDifficulty( dif), fret( 0xFF ) { };
		
			unsigned char			normalisedDifficulty;
			float					bend;
			unsigned char			slide;
		
			const unsigned char&	GetFret() const { return fret; }
			const unsigned char&	GetString() const { return string; };
			
			const bool				IsBend() const { 
										if( bend > 0 ) { return true; } 
										else { return false; }
									};
			
			void					SetFret( const Tuning& tun = aTuning[ eTuning::STANDARD_E ] ) { 
										if( pitch != 0xFF ) { fret = pitch - tun.pitch[ string ]; }
									}
			
		protected:
			unsigned char			fret;
			unsigned char			string;
	};
	
	struct Lyric : public Base::Note {
		Lyric( const float& tim = 0.000f, const unsigned int& pit = 0, 
			const std::string& w = "" ) : Base::Note( tim, pit ), word( w ) { };
		
		std::string word;
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