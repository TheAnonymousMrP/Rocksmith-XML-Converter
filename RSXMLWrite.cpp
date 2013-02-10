#include "RSXMLWrite.h"

void RSXMLWrite::processArrangement() {
	std::cout << "Name: " << arr.getName() ENDLINE
	std::string file = fileName + "-" + arr.getName() + ".xml";
	std::cout << file ENDLINE
	write.open(file.c_str());
	write << "<?xml version='1.0' encoding='UTF-8'?> \n"
	<< "<song>\n" << "\t<title>" << songName << "</title>\n"
	<< "\t<arrangement>" << arr.getName() << "</arrangement>\n"
	<< "\t<part>1</part>\n" // This should be something.
	/* Specifies a negative value (in seconds) for the song to be offset.
	Eg. -10 seconds if the first beat is at 10 seconds. Doesn't seem to work
	right for me, though. It may correlated with startBeat, though, 
	which seems to always be 0 despite where the real start time is. */
	<< "\t<offset>" << 0 - DEFAULTOFFSET << "</offset>\n"
	<< "\t<centOffset>" << DEFAULTOFFSETCENT << "</centOffset>\n" // No idea.
	<< "\t<songLength>" << arr.getDuration() << "</songLength>\n" 
	<< "\t<startBeat>" << DEFAULTSTARTBEAT << "</startBeat>\n"
	<< "\t<averageTempo>" << DEFAULTTEMPO << "</averageTempo>\n"
	// Tuning information. May not be implemented yet.
	<< "\t<tuning ";
	for(int i = 0; i < NUMSTRINGS; ++i) { 
		write << "string" << i << "=\"" 
		<< (getTuning(eTuning::standardE,i) - getTuning(arr.getTuning(), i))
		<< "\" ";
	}
	write << "/>\n"
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
	
	writeStructure();

	/* Transcription Track -- whatever that is. It seems to be one full 
	difficulty of a song. Maybe max? */
	const std::vector<Difficulty>& difficulties = arr.getDifficulties();
	writeDifficulty(difficulties.back(), -1, true); 
	
	// Levels -- where shit gets real.
    write << "\t<levels count=\"" << difficulties.size() << "\" >\n";
    for(auto it = difficulties.begin(); it != difficulties.end(); ++it)
    	{ writeDifficulty((*it), (it - difficulties.begin())); } 
    write << "\t</levels>\n";
    write << "</song>";
    write.close();
	}
	
void RSXMLWrite::processVocals() {
	std::string file = fileName + "-Vocals.xml";
	write.open(file.c_str()); // This is the file we're writing to.
	
	std::vector<Lyric> vLyrics(arrV.getLyrics());
	
	write << "<?xml version='1.0' encoding='UTF-8'?> \n"
	<< "<vocals count=\"" << vLyrics.size() << "\">\n";
	
	for(Lyric& l : vLyrics)
		{
		write << "\t<vocal time=\"" << l.getTime() << "\" note=\"" 
		<< l.getPitch() << "\" length=\"" << l.getDuration() << "\" lyric=\"" 
		<< l.word << "\"/>\n";
		
		/* Debug
		cout << "WOOP WOOP - Pitch: " << cL.pitch << " | " << cL.time 
		<< " | " << cL.duration << " | " << cL.word << "\n"; */
		}
		
	write << "</vocals>";
	write.close();
}
	
// Write methods 

void RSXMLWrite::writeStructure() {
	// Phrases - 'phrase' and 'phraseIterations' ---
	/* <phrase disparity="0" ignore="0" maxDifficulty="0" name="INTROBAR" 
	solo="0"/> */
	auto& phrases = arr.getPhraseTemplates();
	write << "\t<phrases count=\"" << phrases.size() << "\">\n";
	for(auto& t : phrases) {
		write << "\t\t<phrase name=\"" << t.name << "\" maxDifficulty=\""
		<< t.maxDif << "\" disparity=\"" << t.disparity << "\" ignore=\"" 
		<< t.ignore << "\" solo=\"" << t.solo << "\" />\n";
	}
	write << "\t</phrases>\n";
	// <phraseIteration time="10.000" phraseId="0" variation=""/>
	auto& phraseIt = arr.getPhrases();
	write << "\t<phraseIterations count=\"" << phraseIt.size() << "\">\n";
	for(auto& p : phraseIt) {
		/* Variation always seems to be incremented. This may be dependent
		on whether the contents are changed, but maybe not. Slightly 
		complicating matters is that the 'official' way seems to be to use
		roman numerals. */
		std::string var = std::to_string(p.getVariation());
		write << "\t\t<phraseIteration time=\"" << p.getTime()
		<< "\" phraseId=\"" << p.getID() << "\" variation=\"" << var 
		<< "\" />\n";
	}
	write << "\t</phraseIterations>\n";
	
	// Linked Diffs
	/*	<newLinkedDiff phrases="15,17" levelBreak="-1" ratio="1.000"/>
		<linkedDiff childId="17" parentId="15"/> */
	write << "\t<newLinkedDiffs count=\"" << 0 << "\" />\n";
	write << "\t<linkedDiffs count=\"" << 0 << "\" />\n";
	write << "\t<phraseProperties count=\"" << 0 << "\" />\n";
	
	// Chord Templates
	/* <chordTemplate chordName="" displayName="ggd" finger0="-1" finger1="-1" 
	finger2="-1" finger3="-1" finger4="1" finger5="3" fret0="-1" fret1="-1" 
	fret2="-1" fret3="0" fret4="8" fret5="10"/> */
	auto& chords = arr.getChordTemplates();
	write << "\t<chordTemplates count=\"" << chords.size() << "\">\n";
	for(auto& t : chords) {
		write << "\t\t<chordTemplate chordName=\"" << t.name
		<< "\" displayName=\"" << t.display << "\" ";
		// Frets
		for(int i = 0; i < NUMSTRINGS; ++i) {
			write << "fret" << i << "=\"" << t.getFret(i) << "\" ";
		}
		// Fingers
		for(int i = 0; i < NUMSTRINGS; ++i) {
			write << "finger" << i << "=\"" << t.getFinger(i) << "\" ";
		}
		write << " />\n";
	}
	write << "\t</chordTemplates>\n";
	
	// Fret Hand Mutes
	write << "\t<fretHandMuteTemplates count=\"0\" />\n";
	
	// Events
	unsigned int events = 0;
	write << "\t<events count=\"" << events << "\"";
	if( events == 0 ) { write << " />\n"; }
	else { 
		write << ">\n";	
		
		write << "\t</events>\n";
	}
	
	// Beat Grid
	auto& beats = arr.getBeats();
	write << "\t<ebeats count=\"" << beats.size() << "\">\n";
	for(auto& b : beats) {
		write << "\t\t<ebeat time=\"" << b.time << "\" measure=\""
		<< b.bar << "\" />\n";
	}
	write << "\t</ebeats>\n";
	// Sections
	auto& sections = arr.getSections();
	write << "\t<sections count=\"" << sections.size() << "\">\n";
	for(auto& s : sections) {
		write << "\t\t<section name=\"" << s.getName() 
		<< "\" number=\"" << s.getID() << "\" startTime=\"" 
		<< s.getTime() << "\" />\n";
	}
	write << "\t</sections>\n";
}

void RSXMLWrite::writeDifficulty(const Difficulty& d, int dif, bool trans) {
	/* <level difficulty="0"> */
	std::string t = "";
	if(trans) { t += "\t"; write << t << "<transcriptionTrack "; }
	else { t += "\t\t"; write << t << "<level "; }
	
	write << "difficulty=\"" << dif << "\">\n"; 
	
	auto& phrases = arr.getPhrases();	
	// Notes
	auto& nSource(arr.getNotes());
	auto& nPointer(d.getNotesI());
	// Chords
	auto& cSource(arr.getChords());
	auto& cPointer(d.getChordsI());
	// Anchors
	auto& aSource(arr.getAnchors());
	auto& aPointer(d.getAnchorsI());
	// Handshapes
	auto& hSource(arr.getHands());
	auto& hPointer(d.getHandsI());
	
	/* In the interest of not repeating code, we're going to use 
	stringstreams, and pop the results in after. */
	std::ostringstream nS, cS, aS, hS;
	for( auto& p : phrases ) {
		if( dif == -1 || (unsigned)dif <= p.maxDif ) { 
			float start = p.getTime();
			float end = start + p.duration;
			
			auto& notes(getXsFromIsWithinTime(nSource, nPointer, start, end));
			for( const Note& n : notes ) { writeNote( nS, n, trans ); }
			
			auto& chords(getXsFromIsWithinTime(cSource, cPointer, start, end));
			for( const Chord& c : chords ) { writeChord( cS, c, trans ); }
			
			auto& ans(getXsFromIsWithinTime(aSource, aPointer, start, end));
			for( const Anchor& a : ans ) { writeAnchor( aS, a, trans ); }
			
			auto& hands(getXsFromIsWithinTime(hSource, hPointer, start, end));
			for( const HandShape& h : hands ) { writeHand( hS, h, trans ); }		
		}
	}
	
	// Notes
	write << t << "\t<notes count=\"" << nPointer.size() << "\">\n";
	write << nS.str();
	write << t << "\t</notes>\n";
	
	// Chords
	write << t << "\t<chords count=\"" << cPointer.size() << "\">\n";
	write << cS.str();
	write << t << "\t</chords>\n";
	
	// Anchors
	write << t << "\t<anchors count=\"" << aPointer.size() << "\">\n";
	write << aS.str();
	write << t << "\t</anchors>\n";

	// Handshapes	
	write << t << "\t<handShapes count=\"" << hPointer.size() << "\">\n";
	write << hS.str();
	write << t << "\t</handShapes>\n";
	if(trans) { write << t << "</transcriptionTrack>\n"; }
	else { write << t << "</level>\n"; }
}

void RSXMLWrite::writeNote(std::ostream& dest, const Note& n, bool trans, 
	bool chord) 
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
	
	bool hopo = false, tap = false, ignore = false, linkNext = false; 
	
	if(n.hammerOn || n.pullOff) { hopo = true; }
	if(n.tapLeft || n.tapRight) { tap = true; }
	
	std::string t = "";
	if(!trans) { t = "\t"; }
	t += "\t\t\t";
	
	if(chord) { t += "\t<chordN"; }
	else { t += "<n"; }
	dest << t << "ote time=\"" << n.getTime() << "\" sustain=\"" 
	<< n.getDuration() << "\" string=\"" << n.getString() << "\" fret=\"" 
	<< n.getFret() << "\" ignore=\"" << (int)ignore << "\" linkNext=\""
	<< (int)linkNext << "\" bend=\"" << (int)n.isBend() << "\" harmonic=\"" 
	<< (int)n.harmonic << "\" hopo=\"" << (int)hopo << "\" hammerOn=\"" 
	<< (int)n.hammerOn << "\" pullOff=\"" << (int)n.pullOff 
	<< "\" palmMute=\"" << (int)n.palmMute << "\" slideTo=\"" << n.slide 
	<< "\" tremolo=\"" << (int)n.tremolo
	// New stuff
	<< "\" accent=\"" << (int)n.accent << "\" leftHand=\"" << n.tapLeft 
	<< "\" rightHand=\"" << n.tapRight << "\" harmonicPinch=\"" 
	<< (int)n.pinchHarmonic << "\" mute=\"" << (int)n.fretHandMute 
	<< "\" pickDirection=\"" << n.pick << "\" slideUnpitchTo=\"" 
	<< n.slideUnpitch << "\" tap=\"" << (int)tap << "\" vibrato=\"" 
	<< (int)n.vibrato
	// Bass
	<< "\" pluck=\"" << n.pluck << "\" slap=\"" << n.slap 
	<< "\" bendValues=\"";
	if(n.isBend()) 
		{ dest << n.bendTime << "," << n.bendStep << "\" />\n"; }
	else { dest << -1 << "\" />\n"; }
}

void RSXMLWrite::writeChord(std::ostream& dest, const Chord& c, bool trans) 
	{
	/* <chord time="48.379" linkNext="0" accent="0" chordId="0" 
	fretHandMute="0" highDensity="0" ignore="0" palmMute="0" strum="down">
	*/
	std::string t = "";
	if(!trans) { t = "\t\t"; }
	t += "\t\t\t";
	dest << t << "<chord time=\"" << c.getTime() << "\" linkNext=\""
	<< c.getLinkNext() << "\" accent=\"" << c.getAccent() << "\" chordId=\"" 
	<< c.getID() << "\" fretHandMute=\"" << c.getFretHandMute() 
	<< "\" highDensity=\"" << c.getHighDensity() << "\" ignore=\"" 
	<< c.getIgnore() << "\" palmMute=\"" << c.getPalmMute() << "\" strum=\""
	<< c.getStrum() << "\">\n";
	// Writing the individual notes.
	auto& nSource(arr.getNotes());
	auto& nPointer(c.getNotesI());
	auto& notes(getXsFromIs(nSource, nPointer));
	for(auto& n : notes)
		{ writeNote( dest, n, trans, true ); }
	dest << t << "</chord>\n";
}

void RSXMLWrite::writeAnchor(std::ostream& dest, const Anchor& a, bool trans) 
	{
	/* <anchor time="11.579" fret="8" width="4.000"/> */
	std::string t = ""; 
	if(!trans) { t = "\t\t"; }
	t += "\t\t\t";
	dest << t << "<anchor time=\"" << a.time << "\" fret=\""
	<< a.fret << "\" width=\"" << a.width << "\" />\n";
}
	
void RSXMLWrite::writeHand(std::ostream& dest, const HandShape& h, bool trans) 
	{
	/* <handShape chordId="27" endTime="26.958" startTime="26.409"/> */
	std::string t = ""; 
	if(!trans) { t = "\t\t"; }
	t += "\t\t\t";
	dest << t << "<handShape startTime=\"" << h.time << "\" endTime=\"" 
	<< h.duration << "\" chordId=\"" << h.id << "\" />\n";
}	

