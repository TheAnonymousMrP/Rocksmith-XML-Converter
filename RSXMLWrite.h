#include "arrProcess.cpp"

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
		
		int process(); // Main process function for RSXMLWrite.
		int processTrack(); // Slimline version for development purposes.
		
		void setArrangement(Arrangement a, bool v) { arr = a; };
		void setVocals(Vocal v) { vocals = v; };
	};
	
RSXMLWrite::RSXMLWrite() { fileName = ""; arrName = ""; }
RSXMLWrite::RSXMLWrite(std::string file) { fileName = file; }