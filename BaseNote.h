#ifndef _BASE_NOTE_
#define _BASE_NOTE_

#include <array>
#include <string>

#define NUMSTRINGS 6
#define NUMFRETS 22

namespace Base {
	class BaseObject {
		public:
			BaseObject( const float& tim = 0.000f ) { time = tim; }
			
			const float&	GetTime() const { return time; };
			
		protected:
			float			time;
	};

	class Note : public virtual BaseObject {
		public:
			Note( const float& tim = 0.000f, const unsigned char& pit = 0x00 ) 
				: BaseObject( tim ) { pitch = pit; duration = 0.000f; };
				
			float 			duration;
	
			const unsigned char& 	GetPitch() const { return pitch; };
	
		protected:
			unsigned char 	pitch;
	};
	
	typedef struct {
		std::string 							name;
		std::array<unsigned char, NUMSTRINGS>	pitch;
	} Tuning;
	
	enum eTuning {
		STANDARD_E,
		STANDARD_D,
		DROP_D,
	};
	
	const std::array<Tuning, 10> aTuning { { 
			{ "Standard E", { { 52, 57, 62, 67, 71, 76 } } },
			{ "Standard D", { { 50, 55, 60, 65, 69, 74 } } },
			{ "Drop D", { { 50, 57, 62, 67, 71, 76 } } }
		} };
	
	class GuitarNote : public Base::Note {
		public:
			GuitarNote( const float& tim = 0.000f, const unsigned char& pit = 0x00, 
				const unsigned char& str = 0x00, const unsigned int& dif = 0x00 ) 
				: Base::Note( tim, pit ) { string = str; bend = 0.000f; slide = 0; };
		
			unsigned char		normalisedDifficulty;
			float				bend;
			unsigned char		slide;
		
			const unsigned char& GetFret() const { return fret; }
			const unsigned char& GetString() const { return string; };
			
			const bool			IsBend() const { 
									if( bend > 0 ) { return true; } 
									else { return false; }
								};
			
			void				SetFret() { fret = tuning.pitch[string] - pitch; }
			
			static void			SetTuning( const Tuning& tun 
									= aTuning[eTuning::STANDARD_E] ) { tuning = tun; };
			
		protected:
			unsigned char		fret;
			unsigned char		string;
			
			static Base::Tuning	tuning;
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
	};
};

#endif