from collections import OrderedDict

class Tuning( object ):
	_DEFAULT = { 
		 "E Standard": ( 0, 0, 0, 0, 0, 0 ), 
		 "Drop D": ( -2, 0, 0, 0, 0, 0 ), 
		 "Eb Standard": ( -1, -1, -1, -1, -1, -1 ), 
		 "Eb Standard Drop Db": ( -3, -1, -1, -1, -1, -1 ), 
		 "D Standard": ( -2, -2, -2, -2, -2, -2 ),
		 "Drop C": ( -4, -2, -2, -2, -2, -2 )
		 }

	_ESTANDARD	= ( ( 0, 0, 0, 0, 0, 0 ), "EStandard", "E Standard" )
	_DROPD		= ( ( -2, 0, 0, 0, 0, 0 ), "DropD", "Drop D" )
	_DSTANDARD	= ( ( -2, -2, -2, -2, -2, -2 ), "DStandard", "D Standard" )
	_DROPC		= ( ( -4, -2, -2, -2, -2, -2 ), "DropC", "Drop C" )

	def __init__( self, tuning = _DEFAULT[ "E Standard" ] ):
		self.tuning = tuning
		self.displayName = None
		self.name = None

	def __str__( self ):
		tuning = "<tuning "
		for index, value in enumerate( self.tuning ):
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
	def __init__( self, time = None, phraseID = 0, variation = 0 ):
		self.time		= time
		self.phraseID	= phraseID
		self.variation	= variation

	def __str__( self ):
		
		if self.time is None:
			self.time = -1.0
		return "<phraseIteration time=\"" + str( self.time ) + "\" phraseId=\"" + str( self.phraseID ) + "\" variation=\"" + str( self.variation ) + "\" />"

# class NewLinkedDiff( object ):

# class LinkedDiff( object ):

# class PhraseProperties( object ):

class ChordTemplate( object ):
	_DEFAULT = ( -1, -1, -1, -1, -1, -1 )

	def __init__( self, chordName = None, displayName = None, fingers = _DEFAULT, frets = _DEFAULT ):
		self.chordName		= chordName
		self.displayName	= displayName
		self.fingers		= fingers
		self.frets			= frets

		# Helper
		self.id				= None

	def __str__( self ):
		if self.chordName is None:
			self.chordName = ""
		if self.displayName is None:
			self.displayName = ""

		line = "<chordTemplate chordName=\"" + self.chordName + "\" displayName=\"" + self.displayName + "\" "
		for counter, finger in enumerate( self.fingers ):
			line += "finger" + str( counter ) + "=\"" + str( finger ) + "\" "
		for counter, fret in enumerate( self.frets ):
			line += "fret" + str( counter ) + "=\"" + str( fret ) + "\" "
		line += "/>"
		return line

	def ConvertFretsToFingers( frets = None ):
		'''Not yet functional.'''
		if frets is None:
			return None
		fingers = list( ChordTemplate._DEFAULT )
		for i in fingers:
			# Transform frets[ i ].
			pass
		return tuple( fingers )

# class FretHandMuteTemplate( object ):

class Ebeat( object ):
	def __init__( self, time = None, measure = -1 ):
		self.time = time
		self.measure = measure

	def __str__( self ):
		
		if self.time is None:
			self.time = -1.0
		return "<ebeat time=\"" + str( self.time ) + "\" measure=\"" + str( self.measure ) + "\" />"

class Section( object ):
	def __init__( self, name = "", number = 0, startTime = None ):
		self.name		= name
		self.number		= number
		self.startTime	= startTime

	def __str__( self ):
		
		if self.startTime is None:
			self.startTime = -1.0
		return "<section name=\"" + self.name + "\" number=\"" + str( self.number ) + "\" startTime=\"" + str( self.startTime ) + "\" />"

class Event( object ):
	def __init__( self, time = None, code = "" ):
		self.time = time
		self.code = str( code )

	def __str__( self ):
		
		if self.time is None:
			self.time = -1.0
		return "<ebeat time=\"" + str( self.time ) + "\" code=\"" + self.code + "\" />"

class BendValue( object ):
	def __init__( self, time = None, step = 0.0, channel = 0 ):
		self.time	= time
		self.step	= step

		# Helper
		self.channel	= channel
		self.difficulty = None

	def __str__( self ):
		if self.time is None:
			self.time = -1.0
		return "<bendValue time=\"" + str( self.time ) + "\" step=\"" + str( self.step ) + "\" />"

class Note( object ):
	_TECHNIQUES = OrderedDict( [ ( "linkNext", 0 ), ( "hopo", 0 ), ( "hammerOn", 0 ), ( "pullOff", 0 ), 
			( "harmonic", 0 ), ( "harmonicPinch", 0 ), ( "ignore", 0 ), ( "leftHand", -1 ), ( "rightHand", -1 ), 
			( "mute", 0 ), ( "palmMute", 0 ), ( "pickDirection", 0 ), ( "pluck", -1 ), ( "slap", -1 ), 
			( "slideTo", -1 ), ( "slideUnpitchTo", -1 ), ( "tap", 0 ), ( "tremolo", 0 ), ( "vibrato", 0 ) ] )

	def __init__( self, time = None, string = 0, fret = 0, isChord = False ):
		self.isChord	= isChord
		self.time		= time
		self.string		= string
		self.fret		= fret
		self.bend		= 0
		self.bendValues	= None
		self.sustain	= 0
		self.techniques	= Note._TECHNIQUES.copy()
		
		# Helper
		self.minDifficulty	= 0

	def __str__( self ):
		if self.time is None:
			self.time = -1.0
		if self.bendValues is None:
			self.bendValues = ()

		output = ""
		indent = "\t"
		if self.isChord is True:
			output = "<chordNote "
		else:
			output = "<note "
		output += "time=\"" + str( self.time ) + "\" string=\"" + str( self.string ) + "\" fret=\"" + str( self.fret )
		output += "\" sustain=\"" + str ( self.sustain ) + "\" "
		for type, value in self.techniques.items():
			output += type + "=\"" + str( value ) + "\" "
		output += "bend=\"" + str( self.bend ) + "\" "
		if len( self.bendValues ) == 0:
			output += "/>"
		else:
			output += ">\n"
			output += "<bendValues count=\"" + str( len( self.bendValues ) ) + "\">"
			for bend in self.bendValues:
				output += "\t" + str( bend ) + "\n"
			output += "</bendValues>"
			output += "</note>"
		return output

	def hasTuple( self ):
		return not isinstance( self.bendValues, tuple )

	def SetTechnique( self, technique = "ignore", value = 0 ):
		self.techniques[ technique ] = value

class Chord( object ):
	class Techniques:
		def __init__( self ):
			self.linkNext		= 0
			self.accent			= 0
			self.fretHandMute	= 0
			self.highDensity	= 0
			self.ignore			= 0
			self.palmMute		= 0
			self.hopo			= 0
			self.strum			= "down"

	_TECHNIQUES = OrderedDict( [ ( "linkNext", 0 ), ( "accent", 0 ), ( "fretHandMute", 0 ), 
				( "highDensity", 0 ), ( "ignore", 0 ), ( "palmMute", 0 ), ( "hopo", 0 ), ( "strum", "down" ) ] )

	def __init__( self, time = None ):
		self.time		= time
		self.chordID	= None
		self.techniques	= Chord._TECHNIQUES
		self.notes		= ()

		# Helper
		self.minDifficulty	= 0
		self.isRepeat		= False

	def __str__( self ):
		if self.time is None:
			self.time = -1.0
		output = "<chord " + "time=\"" + str( self.time ) + "\" "
		for type, value in self.techniques.items():
			output += type + "=\"" + str( value ) + "\" "
		if self.isRepeat or len( self.notes ) is 0:
			output += "/>"
		else:
			output += ">\n"
			for note in self.notes:
				output += "\t" + str( note ) + "\n"
			output += "</chord>"
		return output

	def hasTuples( self ):
		return isinstance( self.notes, tuple )

	def GetFrets( self ):
		frets = list( ChordTemplate._DEFAULT )
		for note in self.notes:
			frets[ note.string ] = note.fret

		return tuple( frets )

	def GetDuration( self ):
		maxDuration = 0
		for note in self.notes:
			if note.sustain > maxDuration:
				maxDuration = note.sustain
		return maxDuration

class Anchor( object ):
	def __init__( self, time = None, fret = 0, width = None ):
		self.time	= time
		self.fret	= fret
		self.width	= width

	def __str__( self ):
		if self.time is None:
			self.time = -1.0
		if self.width is None:
			self.width = 4.0

		return "<anchor time=\"" + str( self.time ) + "\" fret=\"" + str( self.fret ) + "\" width=\"" + str( self.width ) + "\" />"

class HandShape( object ):
	def __init__( self, time = None, endTime = None, chordID = 0 ):
		self.time		= time
		self.endTime	= endTime
		self.chordID	= chordID

	def __str__( self ):
		if self.time is None:
			self.time = -1.0
		if self.endTime is None:
			self.endTime = -1.0
		
		buffer = "<handShape startTime=\"" + str( self.time ) + "\" endTime=\"" + str( self.endTime ) + "\" chordID=\"" + str( self.fret ) + "\" />"
		return buffer

class Level( object ):
	def __init__( self, isTranscription = False, difficulty = 0 ):
		self.isTranscription	= isTranscription
		self.difficulty			= difficulty # Should be able to just use index of containing array, but can't hurt.
		self.notes				= []
		self.chords				= []
		self.anchors			= []
		self.handShapes			= []

		self.phraseChange		= []

	def __str__( self ):
		''' Describes contents of Level. For full output, see RSXMLWriter.WriteLevel() '''
		content = "Level Difficulty: " + self.difficulty + " Contents: "
		content += "\n\t Notes: " + str( len( self.notes ) )
		content += "\n\t Chords: " + str( len( self.chords ) )
		content += "\n\t Anchors: " + str( len( self.anchors ) )
		content += "\n\t HandShapes: " + str( len( self.handShapes ) )
		return content

	def hasTuples( self ):
		return ( isinstance( self.notes, tuple ) and isinstance( self.chords, tuple ) and isinstance( self.anchors, tuple ) and isinstance( self.handShapes, tuple ) )
	
class Track( object ):
	class Helper( object ):
		def __init__( self ):
			self.bends				= []
			for i in range( 0, 16 ):
				self.bends.append( [] )
			self.bendsOmni			= []
			self.arpeggios			= {}
			self.chordNames			= {}
			self.difficulties		= []
			self.key				= None
			self.quantise			= 2

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
			( "lastConversionDateTime"	, "" ),
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
		self.levels				= []

		self.helper				= Track.Helper()