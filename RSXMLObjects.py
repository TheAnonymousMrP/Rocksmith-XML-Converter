class Tuning:
	_ESTANDARD	= [ 0, 0, 0, 0, 0, 0, "E Standard" ]
	_DROPD		= [ -2, 0, 0, 0, 0, 0, "Drop D" ]
	_DSTANDARD	= [ -2, -2, -2, -2, -2, -2, "D Standard" ]
	_DROPC		= [ -4, -2, -2, -2, -2, -2, "Drop C" ]

class Phrase( object ):
	def __init__( self, name = str, maxDifficulty = int, disparity = 0, ignore = 0, solo = 0 ):
		self.name			= name
		self.maxDifficulty	= maxDifficulty
		self.disparity		= disparity
		self.ignore			= ignore
		self.solo			= solo

	def __str__( self ):
		return "<phrase name=\"" + self.name + "\" maxDifficulty=\"" + str( self.maxDifficulty ) + "\" disparity=\"" + str( self.disparity ) + "\" ignore=\"" + str( ignore ) + "\" solo=\"" + str( solo ) + "\" />"

class PhraseIteration( object ):
	def __init__( self, time = float, phraseID = int, variation = int ):
		self.time		= time
		self.phraseID	= phraseID
		self.variation	= variation

	def __str__( self ):
		return "<phraseIteration time=\"" + str( self.time ) + "\" phraseId=\"" + str( self.phraseID ) + "\" variation=\"" + str( variation ) + "\" />"

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
	def __init__( self, name = str, number = 0, startTime = float( 0 ) ):
		self.name		= name
		self.number		= number
		self.startTime	= startTime

	def __str__( self ):
		return "<section name=\"" + self.name + "\" number=\"" + str( self.number ) + "\" startTime=\"" + str( self.startTime ) + "\" />"

class Event( object ):
	def __init__( self, time = float( 0 ), code = str ):
		self.time = time
		self.code = measure

	def __str__( self ):
		return "<ebeat time=\"" + str( self.time ) + "\" code=\"" + self.code + "\" />"

class Level( object ):
	def __init__( self, type = False, difficulty = int ):
		self.type		= type
		self.difficulty = difficulty
		self.notes		= []
		self.chords		= []
		self.anchors	= []
		self.handShapes = []

	def __str__( self ):
		content = ""
		# Notes
		if self.notes.count() == 0:
			content += "\t<notes count=\"0\" />\n"
		else:
			content += "\t<notes count=\"" + self.notes.count() + "\">\n"
			for i in self.notes:
				content += "\t\t" + str( i ) + "\n"
			content += "\t</notes>\n"
		# Chords
		if self.chords.count() == 0:
			content += "\t<chords count=\"0\" />\n"
		else:
			content += "\t<chords count=\"" + self.chords.count() + "\">\n"
			for i in self.chords:
				content += "\t\t" + str( i ) + "\n"
			content += "\t</chords>\n"
		# Anchors
		if self.anchors.count() == 0:
			content += "\t<anchors count=\"0\" />\n"
		else:
			content += "\t<anchors count=\"" + self.anchors.count() + "\">\n"
			for i in self.anchors:
				content += "\t\t" + str( i ) + "\n"
			content += "\t</anchors>\n"
		# HandShapes
		if self.handShapes.count() == 0:
			content += "\t<handShapes count=\"0\" />\n"
		else:
			content += "\t<handShapes count=\"" + self.handShapes.count() + "\">\n"
			for i in self.handShapes:
				content += "\t\t" + str( i ) + "\n"
			content += "\t</handShapes>\n"

		output = ""
		if type is False:
			output = "<level difficulty=\"" + str( self.difficulty ) + "\" >\n"
			output += content
			output += "</level>"
		else:
			output = "<transcriptionTrack difficulty=\"-1\" >\n"
			output += content
			output += "</transcriptionTrack>"
		return output

class Note( object ):
	_TECHNIQUES = [ ( "linkNext", 0, ), ( "bend", 0 ), ( "hopo", 0 ), ( "hammerOn", 0 ), ( "pullOff", 0 ), 
				( "harmonic", 0 ), ( "harmonicPinch", 0 ), ( "ignore", 0 ), ( "leftHand", 1 ), ( "rightHand", 0 ), 
				( "mute", 0 ), ( "palmMute", 0 ), ( "pickDirection", 0 ), ( "pluck", -1 ), ( "slap", -1 ), 
				( "slideTo", -1 ), ( "slideUnpitchTo", -1 ), ( "sustain", float( 0 ) ), ( "tap", 0 ), ( "tremolo", 0 ), ( "vibrato", 0 ) ]

	def __init__( self, time = float( 0 ), string = 0, fret = 0, techniques = _TECHNIQUES, isChord = False ):
		self.isChord	= isChord
		self.time		= time
		self.string		= string
		self.fret		= fret
		self.techniques	= techniques

	def __str__( self ):
		output = ""
		if self.isChord is True:
			output = "\t<chordNote "
		else:
			output = "<note "
		output += "time=\"" + str( self.time ) + "\" string=\"" + str( self.string ) + "\" fret=\"" + str( self.fret ) + "\" "
		for i in self.techniques:
			type, value = i
			output += type + "=\"" + str( value ) + "\" "
		output += "/>"
		return output
	
class Track( object ):
	def __init__( self ):
		# head
		self.title					= str
		self.waveFilePath			= str #
		# self.arrType				= str
		self.part					= 1
		self.offset					= float( 0 )
		self.centOffset				= float( 0 )
		self.songLength				= float( 0 )
		self.internalName			= str #
		self.startBeat				= 0			
		self.averageTempo			= 120 #
		self.tuning					= Tuning._ESTANDARD	
		self.capo					= 0					
		self.artistName				= str
		self.artistNameSort			= str #
		self.albumName				= str
		self.albumNameSort			= str #
		self.albumYear				= int #
		self.albumArt				= str #
		self.crowdSpeed				= 1
		self.arrangementProperties	= []
		self.lastConversionTime		= str
		
		# Structure
		self.phrases			= []
		self.phraseIterations	= []
		self.newLinkedDiffs		= []
		self.linkedDiffs		= []
		self.phraseProperties	= []
		self.chordTemplates		= []
		self.fretHandTemplates	= []
		self.ebeats				= []
		self.sections			= []
		self.events				= []
		self.transcriptionTrack	= [ [], [], [], [] ]
		self.levels				= []