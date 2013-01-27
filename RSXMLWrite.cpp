#include "RSXMLWrite.h"

void RSXMLWrite::processArrangement()
	{
	std::string file = fileName + "-" + arr.getName() + ".xml";
	ofstream arrangement;
	arrangement.open(file.c_str()); // This is the file we're writing to.
	cout << file ENDLINE
	arrangement.open(file.c_str());
	arrangement << "<?xml version='1.0' encoding='UTF-8'?> \n"
	<< "<song>\n" << "\t<title>" << songName << "</title>\n"
	<< "\t<arrangement>" << arr.getName() << "</arrangement>\n"
	<< "\t<part>1</part>\n" // This should be something.
	/* Specifies a negative value (in seconds) for the song to be offset.
	Eg. -10 seconds if the first beat is at 10 seconds. Doesn't seem to work
	right for me, though. It may correlated with startBeat, though, 
	which seems to always be 0 despite where the real start time is. */
	<< "\t<offset>" << DEFAULTOFFSET << "</offset>\n"
	<< "\t<centOffset>" << DEFAULTOFFSETCENT << "</offset>\n" // No idea.
	<< "\t<songLength>" << arr.getDuration() << "</songLength>\n" 
	<< "\t<startBeat>" << DEFAULTSTARTBEAT << "</startbeat>\n"
	<< "\t<averageTempo>" << DEFAULTTEMPO << "</averageTempo>\n"
	// Tuning information. May not be implemented yet.
	<< "\t<tuning ";
	for(int i = 0; i < DEFAULTSTRINGS; ++i)
		{ arrangement << "string" << i << "=\"" << arr.getTuning(i) << "\" "; }
	arrangement << "/>\n"
	// Song metadata. Not sure how to grab this at present.
	<< "\t<artistName></artistName>\n"
	<< "\t<albumName></albumName>\n"
	<< "\t<albumYear></albumYear>\n"
	<< "\t<albumArt></albumArt>\n"
	<< "\t<crowdSpeed>1</crowdSpeed>\n" // This could probably be changed.
	/* 'arrangementProperties' holds a number of values describing the 
	contents of the arrangement. At a push, I'd guess this may relate to the
	dynamic difficulty of the game. Alternatively, it could provide a 
	reference for the game to recommend the training videos for the techniques.
	Interesting to note is a number of additional techniques which aren't yet 
	implemented in-game, such as pinch-harmonics. Some of these do have 
	videos, though.
	For now, magic numbers ahoy! In the future, I'll make more of an effort 
	to implement this stuff.*/
	<< "\t<arrangementProperties represent=\"0\" standardTuning=\"" << 1 
	<< "\" nonStandardChords=\"" << 0 << "\" barreChords=\"" << 0 << "\" "
	<< "doubleStops=\"" << 0 << "\" dropDPower=\"" << 0 << "\" "
	<< "fifthsAndOctaves=\"" << 0 << "\" powerChords=\"" << 0 << "\" " 
	<< "openChords=\"" << 0 << "\" "
	// Techniques
	<< "bends=\"" << 0 << "\" fretHandMutes=\"" << 0 << "\" harmonics=\"" 
	<< 0 << "\" hopos=\"" << 0 << "\" palmMutes=\"" << 0 << "\" "
	<< "pinchHarmonics=\"" << 0 << "\" slides=\"" << 0 << "\" sustain=\""
	<< 0 << "\" tapping=\"" << 0 << "\" tremolo=\"" << 0 << "\" "
	<< "unpitchedSlides=\"" << 0 << "\" vibrato=\"" << 0 << "\" "
	// Bass stuff. Should probably try to exclude these from guitar charts.
	// Finger picking may not be a bass value.
	<< "bassPick=\"" << 0 << "\" fingerPicking=\"" << 0 << "\" "
	<< "twoFingerPicking=\"" << 0 << "\" slapPop=\"" << 0 << "\" "
	// Misc
	<< "pickDirection=\"" << 0 << "\" syncopation=\"" << 0 << "\" />\n"
	// Irrelevant data
	<< "\t<internalName>" << fileName << "</internalName>\n" // Project name?
	<< "\t<lastConversionDateTime>" << "01-01-1970 00:01" 
	<< "</lastConversionDateTime>\n";
	
	// Phrases - 'phrase' and 'phraseIterations' ---
	
	// Beat Grid
	std::vector<Beat> beats = arr.getBeats();
	arrangement << "\t<ebeats count=\"" << beats.size() << "\" />\n";
	for(std::vector<Beat>::iterator it = beats.begin();
		it != beats.end(); ++it)
		{
		Beat cB(*it);
		arrangement << "\t\t<ebeat time=\"" << cB.time << "\" measure=\""
		<< cB.bar << "\"/>\n";
		}
	arrangement << "\t</ebeats>\n";
	
	// Sections
	const std::vector<Section>& sections = arr.getSections();
	arrangement << "\t<sections count=\"" << sections.size() << "\" />\n";
	for(std::vector<Section>::const_iterator it = sections.begin();
		it != sections.end(); ++it)
		{
		Section cS(*it);
		arrangement << "\t\t<section name=\"" << cS.getName() 
		<< "\" number=\"" << cS.getID() << "\" startTime=\"" 
		<< cS.getTime() << "\" />\n";
		}
	arrangement << "\t</sections>\n";
	
	// Events
	/* Transcription Track -- whatever that is. It seems to be one full 
	difficulty of a song. Maybe max? */
	
	// Levels -- where shit gets real.
	
	}
	
void RSXMLWrite::processVocals()
	{
	std::string file = fileName + "-Vocals.xml";
	ofstream vocals;
	vocals.open(file.c_str()); // This is the file we're writing to.
	
	std::vector<Lyric> vLyrics(arrV.getLyrics());
	
	vocals << "<?xml version='1.0' encoding='UTF-8'?> \n"
	<< "<vocals count=\"" << vLyrics.size() << "\">\n";
	
	for(std::vector<Lyric>::iterator it = vLyrics.begin();
		it != vLyrics.end(); ++it)
		{
		Lyric cL(*it);
		
		vocals << "\t<vocal time=\"" << cL.time << "\" note=\"" << cL.pitch
		<< "\" length=\"" << cL.duration << "\" lyric=\"" 
		<< cL.word << "\"/>\n";
		
		/* Debug
		cout << "WOOP WOOP - Note ID: " << it - vLyrics.begin() << " | " 
		<< cL.pitch << " | " << cL.time << " | " << cL.duration << " | " 
		<< cL.word << "\n"; */
		}
		
	vocals << "</vocals>";
	vocals.close();
	}
	
// Write methods 

void RSXMLWrite::writeDifficulty(ofstream& arrangement, int dif)
	{
	
	}

// Sub-level write methods - Pass one of the object. ===

void RSXMLWrite::writeAnchor(ofstream& arrangement, Anchor a, int indent)
	{
	/* <anchor time="11.579" fret="8" width="4.000"/> */
	std::string t = "";
	switch(indent)
		{
		case 4: "\t\t";
		case 2: "\t\t"; break;
		}
	arrangement << t << "<anchor time=\"" << a.time << "\" fret=\""
	<< a.fret << "\" width=\"" << a.width << "\" />\n";
	}
	
void RSXMLWrite::writeHand(ofstream& arrangement, HandShape h, int indent)
	{
	/* <handShape chordId="27" endTime="26.958" startTime="26.409"/> */
	std::string t = "";
	switch(indent)
		{
		case 4: "\t\t";
		case 2: "\t\t"; break;
		}
	arrangement << t << "<handShape startTime=\"" << h.time << "\" endTime=\"" 
	<< h.duration << "\" chordId=\"" << h.id << "\" />\n";
	}	

void RSXMLWrite::writeChord(ofstream& arrangement, Chord c, int indent)
	{
	/* <chord time="48.379" linkNext="0" accent="0" chordId="0" 
	fretHandMute="0" highDensity="0" ignore="0" palmMute="0" strum="down">
	*/
	std::string t = "";
	switch(indent)
		{
		case 4: "\t\t";
		case 2: "\t\t"; break;
		}
	arrangement << t << "<chord time=\"" << c.getTime() << "\" linkNext=\""
	<< c.getLinkNext() << "\" accent=\"" << c.getAccent() << "\" chordId=\"" 
	<< c.getID() << "\" fretHandMute=\"" << c.getFretHandMute() 
	<< "\" highDensity=\"" << c.getHighDensity() << "\" ignore=\"" 
	<< c.getIgnore() << "\" palmMute=\"" << c.getPalmMute() << "\" strum=\""
	<< c.getStrum() << "\" />\n";
	for(std::vector<Note>::iterator it = c.getNotes().begin();
		it != c.getNotes().end(); ++it)
		{
		Note cN(*it);
		writeNote(arrangement, cN, (indent+1));
		}
	arrangement << t << "</chord>\n";
	}

void RSXMLWrite::writeNote(ofstream& arrangement, Note n, int indent)
	{
	/* <note time="111.460" linkNext="0" accent="0" bend="1" fret="17" 
	hammerOn="0" harmonic="0" hopo="0" ignore="0" leftHand="-1" mute="0" 
	palmMute="0" pluck="-1" pullOff="0" slap="-1" slideTo="-1" string="5" 
	sustain="0.291" tremolo="0" harmonicPinch="0" pickDirection="0" 
	rightHand="-1" slideUnpitchTo="-1" tap="0" vibrato="0" 
	bendValues="111.460,2.000"/> */
	
	/* Shit's getting real, and everything is subject to change. My bet is
	Rocksmith 2, but fuck me if they shouldn't wait for next-gen. Of course,
	it could just be shit for in-game videos, but whatever.
	
	Right, bend is probably changing to a bool and expanded upon by the new
	bend fields. It's hard to tell whether it's going to work like the leaked
	Incubus song does, where the bend fields are in the next bit, or like in
	current DLC such as Cliffs of Dover, where they are just two concatenated
	floats in one value at the end. 
	
	'mute' could be a replacement for the unimplemented fretHandMutes. */
	
	int bend = 0, slide = -1, // old school
	pluck = -1, slap = -1, // bass
	leftHand = -1, rightHand = -1, slideUnpitch = -1; // new school
	bool harmonic, hopo, hammerOn, pullOff, palmMute, tremolo, // old.
	accent, harmonicPinch, mute, pickDirection, tap, vibrato, // new.
	ignore, linkNext; // Miscellaneous. The latter is new, but neither are in.
	
	std::string t = "";
	// We can for now assume that an indent of 5 or 3 means it's in a chord.
	switch(indent) 	
		{
		case 5: t += "\t\t";
		case 3: t += "\t\t\t<chordN"; break;
		case 4: t += "\t\t";
		case 2: t += "\t\t<n"; break;
		}
	arrangement << t << "ote time=\"" << n.getTime() << "\" duration=\"" 
	<< n.getDuration() << "\" string=\"" << n.getString() << "\" fret=\"" 
	<< n.getFret() << "\" ignore=\"" << ignore << "\" linkNext=\""
	<< linkNext << "\" harmonic=\"" << harmonic << "\" hopo=\"" << hopo
	<< "\" hammerOn=\"" << hammerOn << "\" pullOff=\"" << pullOff
	<< "\" palmMute=\"" << palmMute << "\" tremolo=\"" << tremolo
	// New stuff
	<< "\" accent=\"" << accent << "\" leftHand=\"" << leftHand 
	<< "\" rightHand=\"" << rightHand << "\" harmonicPinch=\"" << harmonicPinch
	<< "\" mute=\"" << mute << "\" pickDirection=\"" << pickDirection
	<< "\" tap=\"" << tap << "\" vibrato=\"" << vibrato
	// Bass
	<< "\" pluck=\"" << pluck << "\" slap=\"" << slap
	<< "\" bendValues=\"-1\" />\n";
	}