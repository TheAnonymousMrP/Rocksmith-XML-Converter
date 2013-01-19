#ifndef _BASE_VOCAL_
#include "BaseVocals.h"
#endif

class CreateVocalArrangement
	{
	Track t;
	Vocal v;
	
	public:
		CreateVocalArrangement() { };
		CreateVocalArrangement(Track t) { };
		~CreateVocalArrangement() { };
		
		void process();
		
		Vocal getArrangement() { return v; };
	
	};