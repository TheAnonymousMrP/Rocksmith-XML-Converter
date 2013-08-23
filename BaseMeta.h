#ifndef BASE_META
#define BASE_META

#include <array>
#include <exception>
#include <string>

#define DEFAULTTEMPO 120.000f
#define ONEMINUTE 60.000f
#define ONEMINUTEMILLI 60000
#define ONEMINUTEMICRO 60000000
#define ONESECONDMILLI 1000

#define DEFAULTTIMESIGNUM 4
#define DEFAULTTIMESIGDENOM 4
#define DEFAULTTIMESIGPULSE 24
#define DEFAULTTIMESIG32ND 8

namespace Base {
	enum class eMeta {
		ANCHOR,
		BEND,
		CHORD,
		EVENT,
		KEYSIG,
		LYRICS,
		MARKER,
		PHRASE,
		TECHNIQUE,
		/*
		T_ACCENT,
		T_BEND,
		T_HAMMERON,
		T_HARMONIC,
		T_PALMMUTE,
		T_PINCHHARMONIC,
		T_PULLOFF,
		T_SLIDE,
		T_TREMOLO,
		T_TAP,
		T_VIBRATO,
		T_BASS_PLUCK,
		T_BASS_SLAP,
		*/
		TEMPO,
		TIMESIG,
		TUNING,
		SPECIAL,
	};
	/* switch(type) {
			case eMeta::ANCHOR: 
			case eMeta::BEND: 
			case eMeta::T_BEND: 
			case eMeta::CHORD: 
			case eMeta::EVENT: 
			case eMeta::LYRICS: 
			case eMeta::MARKER: 
			case eMeta::PHRASE: 
			case eMeta::KEYSIG: 
			case eMeta::TUNING: 
			case eMeta::SPECIAL: 
			case eMeta::T_ACCENT: 
			case eMeta::T_HAMMERON: 
			case eMeta::T_HARMONIC: 
			case eMeta::T_PALMMUTE: 
			case eMeta::T_PINCHHARMONIC: 
			case eMeta::T_PULLOFF: 
			case eMeta::T_SLIDE: 
			case eMeta::T_TREMOLO: 
			case eMeta::T_TAP: 
			case eMeta::T_VIBRATO: 
			case eMeta::T_BASS_PLUCK: 
			case eMeta::T_BASS_SLAP: 
			case eMeta::TEMPO: 
			case eMeta::TIMESIG:
		} */

	class Meta {
		public:
			Meta( const eMeta& typ = eMeta::SPECIAL, const float& tim = 0.000f ) 
				: time( tim ), type( typ ) { };
			
			const float&			GetTime() const { return time; };
			const eMeta&			GetType() const { return type; }; 
			
		protected:
			float					time;
			eMeta					type;
	};
	
	class MetaFloat : public Base::Meta {
		public:
			MetaFloat( const eMeta& typ = eMeta::SPECIAL, const float& tim = 0.000f, 
				const float& val = 0.000f ) : Base::Meta( typ, tim )
				{ value = val; };
				
			const float&			GetFloat() const { return value; };
			
		private:
			float					value;
	
	};
	
	class MetaString : public Base::Meta {
		public:
			MetaString( const eMeta& typ = eMeta::SPECIAL, const float& tim = 0.000f, 
				const std::string& tex = "" ) : Base::Meta( typ, tim )
				{ text = tex; };
				
			const std::string&		GetString() const { return text; };
			
		protected:
			std::string				text;
	};
	
	class MetaUInt : public Base::Meta {
		public:
			MetaUInt( const eMeta& typ = eMeta::SPECIAL, const float& tim = 0.000f, 
				const unsigned int& num = 0 ) : Base::Meta( typ, tim )
				{ number = num; };
				
			const unsigned int&		GetUInt() const { return number; };
			
		private:
			unsigned int			number;
	
	};
	
	class Tempo : public Base::Meta {
		public:
			Tempo( const float& tim = 0.000f, const float& val = 120.000f ) : Base::Meta( Base::eMeta::TEMPO, tim ), value( val ) { };
				
			const float&			GetTempo() const { return value; };
			float					Convert2Beat() const { return ONEMINUTE / value; };
									
		private:
			float					value;
	
	};
	
	const std::array<unsigned int, 4> DEFAULTTIMESIG = { 
		{ DEFAULTTIMESIGNUM, DEFAULTTIMESIGDENOM, 
		DEFAULTTIMESIGPULSE, DEFAULTTIMESIG32ND } 
	};
	
	class TimeSig : public Base::Meta {
		public:
			TimeSig( const float& tim = 0.000f, const std::array<unsigned int, 4>& list = DEFAULTTIMESIG ) 
				: Base::Meta( Base::eMeta::TIMESIG, tim ) { 
				numerator = list[0];
				denominator = list[1]; 
				clocks = list[2];
				quarter = list[3];
			};
			
			unsigned int	numerator;
			unsigned int	denominator;
			unsigned int	clocks;
			unsigned int	quarter;
			
		/* From http://www.sonicspot.com/guide/midifiles.html:
			This meta event is used to set a sequences time signature. The time signature 
			defined with 4 bytes, a numerator, a denominator, a metronome pulse and number 
			of 32nd notes per MIDI quarter-note. The numerator is specified as a literal 
			value, but the denominator is specified as (get ready) the value to which the 
			power of 2 must be raised to equal the number of subdivisions per whole note. 
			For example, a value of 0 means a whole note because 2 to the power of 0 is 1 
			(whole note), a value of 1 means a half-note because 2 to the power of 1 is 2 
			(half-note), and so on. The metronome pulse specifies how often the metronome 
			should click in terms of the number of clock signals per click, which come at 
			a rate of 24 per quarter-note. For example, a value of 24 would mean to click 
			once every quarter-note (beat) and a value of 48 would mean to click once every 
			half-note (2 beats). And finally, the fourth byte specifies the number of 32nd 
			notes per 24 MIDI clock signals. This value is usually 8 because there are 
			usually 8 32nd notes in a quarter-note. At least one Time Signature Event should 
			appear in the first track chunk (or all track chunks in a Type 2 file) before any 
			non-zero delta time events. If one is not specified 4/4, 24, 8 should be assumed.
		*/
	};
	
	class MetaException : public std::exception {
		virtual const char* what() const throw() { return "Invalid eMeta type for context."; };
	};
	
	
};

#endif