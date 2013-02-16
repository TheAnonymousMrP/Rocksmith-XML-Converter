#ifndef _BASE_NOTE_
#define _BASE_NOTE_

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
	
	namespace nTuning {
		const int STANDARDE[NUMSTRINGS] = { 52, 57, 62, 67, 71, 76 }; 
		const int DROPD[NUMSTRINGS] = { 50, 57, 62, 67, 71, 76 };
	};
	
	class GuitarNote : public Base::Note {
		public:
			GuitarNote( const float& tim = 0.000f, const unsigned char& pit = 0x00, 
				const unsigned char& str = 0x00, const unsigned int& dif = 0x00 ) 
				: Base::Note( tim, pit ) { string = str; bend = 0.000f; slide = 0; };
		
			unsigned char			normalisedDifficulty;
			float					bend;
			unsigned char			slide;
		
			const unsigned char&	GetFret() const { return fret; }
			const unsigned char&	GetString() const { return string; };
			
			const bool				IsBend() const { ( bend > 0 )? true : false; };
			
			void					SetFret() 
										{ fret = nTuning::STANDARDE[string] - pitch; }
			
			
		protected:
			unsigned char			fret;
			unsigned char			string;
	};
};

#endif