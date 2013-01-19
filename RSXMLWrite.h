#include "GenArr.cpp"
#include "GenArrVocal.cpp"

class RSXMLWrite
	{
	// Private:
	std::string fileName; std::string arrName;
	Arrangement arr; 
	Vocal vocals;
	
	public:
		RSXMLWrite();
		RSXMLWrite(std::string file);
		~RSXMLWrite() { };
		
		void process(); // Main process function for RSXMLWrite.
		
		void setArrangement(Arrangement a) { arr = a; };
		void setVocals(Vocal v) { vocals = v; };
	};
	
RSXMLWrite::RSXMLWrite() { fileName = ""; arrName = ""; }
RSXMLWrite::RSXMLWrite(std::string file) { fileName = file; } 