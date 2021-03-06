from decimal import *

def enum(**enums):
	# http://stackoverflow.com/a/1695250 
    return type('Enum', (), enums)

class Default:
	# Default numbers
	_DIVISION			= 480
	_TEMPO				= 120
	_TIMESIGNUM			= 4
	_TIMESIGDENOM		= 2
	_TIMESIGCLOCKS		= 24
	_TIMESIGQUARTER		= 8
	_TIMESIG			= [ 4, 2, 24, 8 ]

	_ONEMINUTE			= 60
	_ONEMINUTEMILLI		= 60000
	_ONEMINUTEMICRO		= 60000000
	_ONESECONDMILLI		= 1000

	_PITCHBENDRANGE		= 16384 # pow( 2, 14 )

	# Hardcoded MIDI format values.
	_FORMATBYTE			= 9
	_TRACKSBYTE			= 11
	_DIVISIONBYTES		= range( 12, 14 )
	_CHUNKIDLENGTH		= 4
	_HEADERLENGTH		= 14
	_TRACKHEADERLENGTH	= 8
	_MAXDELTASIZE		= 4
		
class Event( object ):
	class eType:
		_NOTE_OFF			= 0x80
		_NOTE_ON			= 0x90
		_AFTERTOUCH			= 0xA0
		_CONTROLLER			= 0xB0
		_PROGRAM			= 0xC0
		_PRESSURE			= 0xD0
		_PITCH_WHEEL		= 0xE0
		_SYSEX				= 0xF0
		_SYSEX_END			= 0xF7
		_META				= 0xFF

	class eMeta:
		_SEQUENCE			= 0x00
		_TEXT				= 0x01
		_COPYRIGHT			= 0x02
		_NAME_SEQUENCE		= 0x03
		_NAME_INSTRUMENT	= 0x04
		_LYRIC				= 0x05
		_MARKER				= 0x06
		_CUE				= 0x07
		_NAME_PROGRAM		= 0x08
		_NAME_DEVICE		= 0x09
		_CHANNEL_PREFIX		= 0x20
		_PORT				= 0x21
		_END_OF_TRACK		= 0x2F
		_TEMPO				= 0x51
		_SMPTE_OFFSET		= 0x54
		_TIME_SIGNATURE		= 0x58
		_KEY_SIGNATURE		= 0x59
		_SEQUENCER_SPECIFIC	= 0x7F

	def __init__( self, time = None, status = 0 ):
		self.time			= time
		self.status			= status

class ChannelEvent( Event ):
	def __init__( self, time = None, status = 0, primary = 0, secondary = 0 ):
		Event.__init__( self, time, status )

		self.channel = status & 0x0F
		self.primary = primary
		self.secondary = secondary

class PitchBend( Event ):
	def __init__( self, time = None, status = 0, bend = int( Default._PITCHBENDRANGE / 2 ) ):
		Event.__init__( self, time, status )

		self.channel = status & 0x0F
		self.bend = bend

class MetaEvent( Event ):
	def __init__( self, time = None, metaType = 0, message = "" ):
		Event.__init__( self, time, Event.eType._META )

		self.type = metaType
		self.message = message

class Tempo( Event ):
	def __init__( self, time = None, tempo = Default._TEMPO ):
		Event.__init__( self, time, Event.eType._META )

		self.tempo = tempo

class TimeSig( Event ):
	def __init__(self, time = None, numerator = 4, denominator = 2, clocks = 24, quarter = 8 ):
		Event.__init__( self, time, Event.eType._META )

		self.numerator		= numerator
		self.denominator	= denominator
		self.clocks			= clocks
		self.quarter		= quarter

class KeySig( Event ):
	def __init__(self, time = None, sharpsOrFlats = 0, isMinor = 0 ):
		Event.__init__( self, time, Event.eType._META )

		self.intonation = sharpsOrFlats
		self.isMinor	= isMinor
	
class Global( object ):
	def __init__( self ):
		self.smpteOffset	= None
		self.metaEvent		= []
		self.tempo			= []
		self.timeSig		= []
		self.keySig			= []

class Track( object ):
	def __init__( self, nameTrack = "", nameInstrument = "" ):
		if nameTrack is None:		nameTrack = ""
		if nameInstrument is None:	nameInstrument = ""

		self.nameTrack				= nameTrack
		self.nameInstrument			= nameInstrument
		self.sequenceNumber			= 0

		self.noteOn					= []
		self.noteOff				= []
		self.aftertouch				= []
		self.controller				= []
		self.programChange			= []
		self.channelPressure		= []
		self.pitchBend				= []
		self.sysex					= []
		self.systemCommon			= []
		self.systemRealTime			= []
		self.metaEvent				= []

class File( object ):
	def __init__( self, format = 0, division = 480 ):
		self.name			= ""
		self.format			= format
		self.division		= division
		self.length			= None
		self.globalTrack	= Global()
		self.tracks			= []

class BinaryFile( object ):
	def __init__( self, format = 0, division = 480 ):
		self.format			= format
		self.division		= division
		self.tracks			= []