from collections import OrderedDict

class Tuning( object ):
	_DEFAULT = { 
		 "E Standard": [ 0, 0, 0, 0, 0, 0 ], 
		 "Drop D": [ -2, 0, 0, 0, 0, 0 ], 
		 "Eb Standard": [ -1, -1, -1, -1, -1, -1 ], 
		 "Eb Standard Drop Db": [ -3, -1, -1, -1, -1, -1 ], 
		 "D Standard": [ -2, -2, -2, -2, -2, -2 ],
		 "Drop C": [ -4, -2, -2, -2, -2, -2 ]
		 }

	_ESTANDARD	= [ [ 0, 0, 0, 0, 0, 0 ], "EStandard", "E Standard" ]
	_DROPD		= [ [ -2, 0, 0, 0, 0, 0 ], "DropD", "Drop D" ]
	_DSTANDARD	= [ [ -2, -2, -2, -2, -2, -2 ], "DStandard", "D Standard" ]
	_DROPC		= [ [ -4, -2, -2, -2, -2, -2 ], "DropC", "Drop C" ]

	def __init__( self, tuning = _DEFAULT[ "E Standard" ] ):
		self.tuning = tuning

	def __str__( self ):
		tuning = "<tuning "
		for index, value in enumerate( self.tuning[0] ):
			tuning += "string" + str( index ) + "=\"" + str( value ) + "\" "
		tuning += "/>"
		return tuning

class Phrase( object ):
	def __init__( self, name = "", maxDifficulty = 0, disparity = 0, ignore = 0, solo = 0 ):
		self.name			= name
		self.maxDifficulty	= maxDifficulty
		self.disparity		= disparity
		self.ignore			= ignore
		self.solo			= solo

	def __str__( self ):
		phrase = "<phrase name=\"" + self.name + "\" maxDifficulty=\"" + str( self.maxDifficulty ) + "\" "
		phrase += "disparity=\"" + str( self.disparity ) + "\" ignore=\"" + str( self.ignore ) + "\" solo=\"" + str( self.solo ) + "\" />"
		return phrase

class PhraseIteration( object ):
	def __init__( self, time = float, phraseID = 0, variation = 0 ):
		self.time		= time
		self.phraseID	= phraseID
		self.variation	= variation

	def __str__( self ):
		return "<phraseIteration time=\"" + str( self.time ) + "\" phraseId=\"" + str( self.phraseID ) + "\" variation=\"" + str( self.variation ) + "\" />"

# class NewLinkedDiff( object ):

# class LinkedDiff( object ):

# class PhraseProperties( object ):

class ChordTemplate( object ):
	_DEFAULT = [ -1, -1, -1, -1, -1, -1 ]

	def __init__( self, chordName = str, displayName = str, fingers = _DEFAULT, frets = _DEFAULT ):
		self.chordName		= chordName
		self.displayName	= maxDifficulty
		self.fingers		= fingers
		self.frets			= frets

	def __str__( self ):
		line = "<chordTemplate chordName=\"" + self.name + "\" displayName=\"" + self.displayName + "\" "
		for counter, finger in enumerate( self.fingers ):
			line += "finger" + str( counter ) + "=\"" + str( finger ) + "\" "
		for counter, fret in enumerate( self.frets ):
			line += "fret" + str( counter ) + "=\"" + str( fret ) + "\" "
		line += "/>"
		return line

# class FretHandMuteTemplate( object ):

class Ebeat( object ):
	def __init__( self, time = float( 0 ), measure = -1 ):
		self.time = time
		self.measure = measure

	def __str__( self ):
		return "<ebeat time=\"" + str( self.time ) + "\" measure=\"" + str( self.measure ) + "\" />"

class Section( object ):
	def __init__( self, name = str, number = 0, startTime = 0.0 ):
		self.name		= name
		self.number		= number
		self.startTime	= startTime

	def __str__( self ):
		return "<section name=\"" + self.name + "\" number=\"" + str( self.number ) + "\" startTime=\"" + str( self.startTime ) + "\" />"

class Event( object ):
	def __init__( self, time = float( 0 ), code = "" ):
		self.time = time
		self.code = str( code )

	def __str__( self ):
		return "<ebeat time=\"" + str( self.time ) + "\" code=\"" + self.code + "\" />"

class Level( object ):
	def __init__( self, isTranscription = False, difficulty = 0 ):
		self.type		= isTranscription
		self.difficulty = difficulty
		self.notes		= []
		self.chords		= []
		self.anchors	= []
		self.handShapes = []

	def __str__( self ):
		indent = "\t\t"
		if self.type == False:
			indent += "\t"
		content = ""
		# Notes
		if len( self.notes ) == 0:
			content += indent + "<notes count=\"0\" />\n"
		else:
			content += indent + "<notes count=\"" + len( self.notes ) + "\">\n"
			for i in self.notes:
				content += indent + "\t" + str( i ) + "\n"
			content += indent + "</notes>\n"
		# Chords
		if len( self.chords ) == 0:
			content += indent + "<chords count=\"0\" />\n"
		else:
			content += indent + "<chords count=\"" + len( self.chords ) + "\">\n"
			for i in self.chords:
				content += indent + "\t\t" + str( i ) + "\n"
			content += indent + "</chords>\n"
		# Anchors
		if len( self.anchors ) == 0:
			content += indent + "<anchors count=\"0\" />\n"
		else:
			content += indent + "<anchors count=\"" + len( self.anchors ) + "\">\n"
			for i in self.anchors:
				content += indent + "\t\t" + str( i ) + "\n"
			content += indent + "</anchors>\n"
		# HandShapes
		if len( self.handShapes ) == 0:
			content += indent + "<handShapes count=\"0\" />\n"
		else:
			content += indent + "<handShapes count=\"" + len( self.handShapes ) + "\">\n"
			for i in self.handShapes:
				content += indent + "\t\t" + str( i ) + "\n"
			content += indent + "\t</handShapes>\n"

		output = ""
		if self.type == False:
			output = "\t\t<level difficulty=\"" + str( self.difficulty ) + "\">\n"
			output += content
			output += "\t\t</level>"
		else:
			output = "\t<transcriptionTrack difficulty=\"-1\">\n"
			output += content
			output += "\t</transcriptionTrack>"
		return output

class BendValue( object ):
	def __init__( self, time = None, step = 0.0, string = 0 ):
		self.time	= time
		self.step	= step

		# Helper
		self.string	= string

	def __str__( self ):
		indent = "\t"
		return indent + "<bendValue time=\"" + str( self.time ) + "\" step=\"" + str( self.step ) + "\" />"

class Note( object ):
	_TECHNIQUES = OrderedDict( [ ( "linkNext", 0 ), ( "bend", 0 ), ( "hopo", 0 ), ( "hammerOn", 0 ), ( "pullOff", 0 ), 
				( "harmonic", 0 ), ( "harmonicPinch", 0 ), ( "ignore", 0 ), ( "leftHand", 1 ), ( "rightHand", 0 ), 
				( "mute", 0 ), ( "palmMute", 0 ), ( "pickDirection", 0 ), ( "pluck", -1 ), ( "slap", -1 ), 
				( "slideTo", -1 ), ( "slideUnpitchTo", -1 ), ( "sustain", float( 0 ) ), ( "tap", 0 ), ( "tremolo", 0 ), ( "vibrato", 0 ) ] )

	def __init__( self, time = float( 0 ), string = 0, fret = 0, isChord = False ):
		self.isChord	= isChord
		self.time		= time
		self.string		= string
		self.fret		= fret
		self.techniques	= Note._TECHNIQUES
		self.bends		= []

		# Helper
		self.difficulty = 0

	def __str__( self ):
		output = ""
		indent = "\t"
		if self.isChord is True:
			indent += "\t"
			output = indent + "<chordNote "
		else:
			output = indent + "<note "
		output += "time=\"" + str( self.time ) + "\" string=\"" + str( self.string ) + "\" fret=\"" + str( self.fret ) + "\" "
		for type, value in self.techniques.items():
			output += type + "=\"" + str( value ) + "\" "
		if len( self.bends ) == 0:
			output += "/>"
		else:
			output += ">\n"
			output += indent + "\t<bendValues count=\"" + str( len( self.bends ) ) + "\">"
			for i in self.bends:
				output += indent + "\t" + str( i, tabs ) + "\n"
			output += indent + "\t</bendValues>"
			output += indent + "</note>"
		return output

	def SetTechnique( self, technique = "ignore", value = 0 ):
		self.techniques[ technique ] = value

class Chord( object ):
	_TECHNIQUES = OrderedDict( [ ( "linkNext", 0 ), ( "accent", 0 ), ( "fretHandMute", 0 ), 
				( "highDensity", 0 ), ( "ignore", 0 ), ( "palmMute", 0 ), ( "hopo", 0 ), ( "strum", "down" ) ] )

	def __init__( self, time = float( 0 ) ):
		self.time		= time
		self.techniques	= Chord._TECHNIQUES
		self.notes		= []

	def __str__( self ):
		indent = "\t"
		output = indent + "<chord " + "time=\"" + str( self.time ) + "\" "
		for type, value in self.techniques.items():
			output += type + "=\"" + str( value ) + "\" "
		if len( self.notes ) is 0:
			output += "/>"
		else:
			output += ">\n"
			for i in self.notes:
				output += str( i ) + "\n"
			output += indent + "</chord>"
		return output

	def SetTechnique( self, technique = "ignore", value = 0 ):
		self.techniques[ technique ] = value

class Anchor( object ):
	def __init__( self, time = 0.0, fret = 0, width = 4.0 ):
		self.time	= time
		self.fret	= fret
		self.width	= width

	def __str__( self ):
		indent = "\t"
		return indent + "<anchor time=\"" + str( self.time ) + "\" fret=\"" + str( self.fret ) + "\" width=\"" + str( self.width ) + "\" />"

class HandShape( object ):
	def __init__( self, startTime = float( 0 ), endTime = float( 0 ), chordID = 0 ):
		self.startTime	= startTime
		self.endTime	= endTime
		self.chordID	= chordID

	def __str__( self ):
		indent = "\t"
		return indent + "<handShape startTime=\"" + str( self.startTime ) + "\" endTime=\"" + str( self.endTime ) + "\" chordID=\"" + str( self.fret ) + "\" />"
	
class Track( object ):
	_ARR_PROPERTIES = OrderedDict( [ ( "represent", 0 ), ( "bonusArr", 0 ), ( "standardTuning", 1 ), ( "nonStandardChords", 0 ), ( "barreChords", 0 ), ( "powerChord", 0 ),
		( "dropDPower", 0 ), ( "openChords", 0 ), ( "fingerPicking", 0 ), ( "pickDirection", 0 ), ( "doubleStops", 0 ), ( "palmMutes", 0 ), ( "harmonics", 0 ) ] )
	# pinchHarmonics="0" hopo="0" tremolo="0" slides="0" unpitchedSlides="0" bends="0" tapping="0" vibrato="0" fretHandMutes="0" slapPop="0" 
	# twoFingerPicking="0" fifthsAndOctaves="0" syncopation="0" bassPick="0" sustain="0" pathLead="0" pathRhythm="1" pathBass="0" />
	
	def __init__( self ):
		# Meta
		self.meta = OrderedDict( [
			( "version"					, "" ),
			( "title"					, "" ),
			( "arrangement"				, "" ),
			( "waveFilePath"			, "" ), #
			( "part"					, 1 ),
			( "offset"					, float( 0 ) ),
			( "centOffset"				, float( 0 ) ),
			( "songLength"				, float( 0 ) ),
			( "internalName"			, "" ), #
			( "songNameSort"			, "" ), #
			( "startBeat"				, 0	),
			( "averageTempo"			, 120 ), #
			( "tuning"					, Tuning._DEFAULT[ "E Standard" ]	),
			( "capo"					, 0 ),
			( "artistName"				, "" ),
			( "artistNameSort"			, "" ), #
			( "albumName"				, "" ),
			( "albumNameSort"			, "" ), #
			( "albumYear"				, 0 ), #
			( "albumArt"				, "" ), #
			( "crowdSpeed"				, 1 ),
			( "arrangementProperties"	, Track._ARR_PROPERTIES ),
			( "lastConversionTime"		, "" ),
			] )
		
		# Structure
		self.structure = OrderedDict( [ 
			( "phrases"					, [ Phrase() ] ),
			( "phraseIterations"		, [] ),
			( "newLinkedDiffs"			, [] ),
			( "linkedDiffs"				, [] ),
			( "phraseProperties"		, [] ),
			( "chordTemplates"			, [] ),
			( "fretHandTemplates"		, [] ),
			( "ebeats"					, [] ),
			( "sections"				, [] ),
			( "events"					, [] ),
			] )

		# Content
		self.transcriptionTrack	= Level( True )
		self.levels				= [ Level( False ) ]

		# Helper
		self.quantize			= 1
		self.difficulties		= []
		self.bends				= []

class Helper( object ):
	def __init__( self ):
		pass