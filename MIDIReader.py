from MIDIObjects import *
from MIDIDebug import *

def ConvertBytesToFloat( bytes = bytearray() ):
	if bytes is None:
		return 0.0

	buffer = 0.0
	for index, value in enumerate( bytes ):
		buffer += value * pow( 256, ( len( bytes ) - 1 - index ) )
	return buffer;

def ConvertBytesToVLQ( bytes = bytearray() ):
	if bytes is None:
		return 0

	buffer = bytes[ -1 ]
	#debug print( "VLQ Last: " + str( buffer ) )
	for index, value in enumerate( bytes[:-1] ):
		buffer += ( value - 0x80 ) * pow( 128, ( len( bytes ) - 1 - index ) )
		# debug print( "Byte: " + str( value ) + " " + str( buffer ) )
	return buffer;

def ConvertDeltaToTime( delta = 0, division = Default._DIVISION, tempo = Default._TEMPO):
	length = delta / division
	return ( Default._ONEMINUTE / tempo ) * length

def ConvertParametersToPitchBend( param1 = int, param2 = int ):
	# http://www.blitter.com/~russtopia/MIDI/~jglatt/tech/midispec/wheel.htm
	# unsigned short CombineBytes(unsigned char First, unsigned char Second) {
	#	unsigned short _14bit;
	#	_14bit = (unsigned short)Second;
	#	_14bit <<= 7;
	#	_14bit |= (unsigned short)First;
	#	return(_14bit);
	# }
	bend = param2
	bend <<= 7
	bend |= param1
	return bend

def ConvertSMPTEToTime( bytes = bytearray(3), division = Default._DIVISION ):
	time = 0.0;
	time = time + bytes[1]
	time = time + ( bytes[2] / division )
	time = time + ( ( bytes[3] / 100 ) / division )
	return time

def GetMIDI( filePath = None ):
	if filePath is None:	
		print("No MIDI file was selected to be read.")
		return None
	
	memblock = bytearray()
	
	try:
		# http://stackoverflow.com/questions/1035340/reading-binary-file-in-python
		with open( filePath, "rb" ) as f:
			byte = f.read( 1 )
			while byte:
				# Do stuff with byte.
				memblock.extend( byte )
				byte = f.read( 1 )		
	except Exception as e:
		raise Exception( "Failed to open file.\n" + str( e ) )

	return memblock

# Returns delta (int).
def ProcessDelta( memblock, readPosition ):
	bytes = bytearray()
	while len( bytes ) < Default._MAXDELTASIZE:
		if readPosition >= len( memblock ) - 1:
			raise EOFError( "Reached end of file at unexpected point during delta processing." )

		bytes.append( memblock[ readPosition ] )
		readPosition += 1
		# This should happen on the last delta byte.
		if memblock[ readPosition - 1 ] < Event.eType._NOTE_OFF:
			return ConvertBytesToVLQ( bytes ), readPosition

	raise Exception( "Delta length exceeded max." )

def ProcessGlobal( memblock, readPosition, division = Default._DIVISION ):
	globalTrack = TrackGlobal()

	lastEvent = 0x00

	currentTime = 0.0
	currentTempo = Default._TEMPO

	finished = False	
	while finished == False:
		delta = 0
		try:
			delta, readPosition = ProcessDelta( memblock, readPosition )
			currentTime = currentTime + ConvertDeltaToTime( delta, division, currentTempo )
		except EOFError as e:
			print( e )
			print( "AAAAAAAAAAAAAAAAAAAH" )
		except Exception as e:
			raise e

		status = memblock[ readPosition ]
		if status < Event.eType._META:
			status = lastEvent
			readPosition -= 1
		else:
			readPosition += 1
			metaType = memblock[ readPosition ]
			readPosition += 1
			range = readPosition + memblock[ readPosition ] + 1
			readPosition += 1

			contents = bytearray()
			for byte in memblock[ readPosition : range ]:
				contents.append( byte )	
			readPosition = range - 1
			
			""" Non-global meta-events. """
			#if metaType == Event.eMeta._SEQUENCE: break
			#elif metaType == Event.eMeta._TEXT: break
			#elif metaType == Event.eMeta._COPYRIGHT: break
			#elif metaType == Event.eMeta._NAME_SEQUENCE: break
			#elif metaType == Event.eMeta._NAME_INSTRUMENT: break
			#elif metaType == Event.eMeta._LYRIC: break
			#elif metaType == Event.eMeta._NAME_PROGRAM: break
			#elif metaType == Event.eMeta._NAME_DEVICE: break
			#elif metaType == Event.eMeta._CHANNEL_PREFIX: break
			#elif metaType == Event.eMeta._PORT: break
			""" Global meta-events. """
			if metaType == Event.eMeta._MARKER: 
				globalTrack.metaEvent.append( MetaEvent( currentTime, Event.eMeta._MARKER, str( contents, "ascii" ) ) )
			elif metaType == Event.eMeta._CUE: pass					# Not relevant at present.
			elif metaType == Event.eMeta._END_OF_TRACK: 
				finished = True
			elif metaType == Event.eMeta._TEMPO: 
				currentTempo = ( Default._ONEMINUTEMICRO / ConvertBytesToFloat( contents ) )
				globalTrack.tempo.append( Tempo( currentTime, currentTempo ) )
			elif metaType == Event.eMeta._SMPTE_OFFSET: 
				offset = ConvertSMPTEToTime( contents, division )
				currentTime += offset
				globalTrack.smpteOffset = offset
			elif metaType == Event.eMeta._TIME_SIGNATURE: 
				if len( contents ) != 4:
					print( "Invalid length in content block for Time Signature." )
				else:
					globalTrack.timeSig.append( TimeSig( currentTime, contents[0], contents[1], contents[2], contents[3] ) )
			elif metaType == Event.eMeta._KEY_SIGNATURE: pass		# Key Signatures aren't relevant.
			elif metaType == Event.eMeta._SEQUENCER_SPECIFIC: pass	# No relevence.

		readPosition += 1
		lastEvent = status

	return globalTrack, readPosition

def ProcessTrack( memblock, readPosition, division = Default._DIVISION, globalTrack = TrackGlobal() ):
	track = Track()
	
	currentTime = 0.0
	currentTempo = Default._TEMPO
	globalTempo = iter( globalTrack.tempo )
	try:
		nextTempo = next( globalTempo )
	except StopIteration as e:
		nextTempo = Tempo( 0, -1 )
	
	lastEvent = 0x00

	finished = False	
	while finished == False:
		delta = 0
		try:
			delta, readPosition = ProcessDelta( memblock, readPosition )
			if nextTempo.tempo != -1 and currentTime >= nextTempo.time:
				currentTempo = nextTempo.tempo
				try:
					nextTempo = next( globalTempo )
				except StopIteration as e:
					nextTempo = Tempo( 0, -1 )
			currentTime = currentTime + ConvertDeltaToTime( delta, division, currentTempo )
		except EOFError as e:
			print( e )
			print( "AAAAAAAAAAAAAAAAAAAH" )
		except Exception as e:
			raise e

		status = memblock[ readPosition ]
		readPosition += 1
		if status < Event.eType._NOTE_OFF:
			status = lastEvent
			readPosition -= 1
		if status >= Event.eType._NOTE_OFF and status < Event.eType._SYSEX:
			eventType = status & 0xF0
			channel = status & 0x0F
			primary = memblock[ readPosition ]

			# 3 bytes
			if eventType >= Event.eType._NOTE_OFF and eventType < Event.eType._PROGRAM or eventType == Event.eType._PITCH_WHEEL:
				readPosition += 1
				secondary = memblock[ readPosition ]
				if eventType == Event.eType._NOTE_OFF:		track.noteOff.append( ChannelEvent( currentTime, status, primary, secondary ) )
				elif eventType == Event.eType._NOTE_ON:		track.noteOn.append( ChannelEvent( currentTime, status, primary, secondary ) )
				elif eventType == Event.eType._AFTERTOUCH:	track.aftertouch.append( ChannelEvent( currentTime, status, primary, secondary ) )
				elif eventType == Event.eType._CONTROLLER:	track.controller.append( ChannelEvent( currentTime, status, primary, secondary ) )
				elif eventType == Event.eType._PITCH_WHEEL:	
					bend = ConvertParametersToPitchBend( primary, secondary )
					track.pitchBend.append( PitchBend( currentTime, status, bend ) )
			# 2 bytes
			elif eventType == Event.eType._PROGRAM:			track.programChange.append( ChannelEvent( currentTime, status, primary ) )
			elif eventType == Event.eType._PRESSURE:		track.channelPressure.append( ChannelEvent( currentTime, status, primary ) )
		elif status >= Event.eType._SYSEX and status < Event.eType._META:
			break
		elif status == Event.eType._META:
			metaType = memblock[ readPosition ]
			readPosition += 1
			range = readPosition + memblock[ readPosition ] + 1
			readPosition += 1

			contents = bytearray()
			for byte in memblock[ readPosition : range ]:
				contents.append( byte )	
			readPosition = range - 1


			""" Non-global meta-events. """
			# Broad if statement for text-based Meta Events:
			if metaType == Event.eMeta._SEQUENCE or metaType == Event.eMeta._TEXT or metaType == Event.eMeta._COPYRIGHT or metaType == Event.eMeta._LYRIC:
				track.metaEvent.append( MetaEvent( currentTime, metaType, str( contents, "ascii" ) ) )
			#if metaType == Event.eMeta._SEQUENCE: break			# Handled above.
			#elif metaType == Event.eMeta._TEXT: break				# Handled above.
			#elif metaType == Event.eMeta._COPYRIGHT: break			# Handled above.
			elif metaType == Event.eMeta._NAME_SEQUENCE:
				track.nameTrack = str( contents, "ascii" )
			elif metaType == Event.eMeta._NAME_INSTRUMENT:
				track.nameInstrument = str( contents, "ascii" )
			#elif metaType == Event.eMeta._LYRIC: break				# Handled above.
			#elif metaType == Event.eMeta._NAME_PROGRAM: break
			#elif metaType == Event.eMeta._NAME_DEVICE: break
			#elif metaType == Event.eMeta._CHANNEL_PREFIX: break
			#elif metaType == Event.eMeta._PORT: break
			elif metaType == Event.eMeta._END_OF_TRACK: 
				finished = True
			""" Global meta-events. """
			#if metaType == Event.eMeta._MARKER: break
			#elif metaType == Event.eMeta._CUE: pass
			#elif metaType == Event.eMeta._TEMPO: break
			#elif metaType == Event.eMeta._SMPTE_OFFSET: break
			#elif metaType == Event.eMeta._TIME_SIGNATURE: break
			#elif metaType == Event.eMeta._KEY_SIGNATURE: pass		# Key Signatures aren't relevant.
			#elif metaType == Event.eMeta._SEQUENCER_SPECIFIC: pass	# No relevence.

		readPosition += 1
		lastEvent = status

	return track, readPosition

def ProcessSimple( memblock, readPosition, division = Default._DIVISION ):
	track = TrackSimple()

	lastEvent = 0x00

	currentTime = 0.0
	currentTempo = Default._TEMPO

	finished = False	
	while finished == False:
		delta = 0
		try:
			delta, readPosition = ProcessDelta( memblock, readPosition )
			currentTime = currentTime + ConvertDeltaToTime( delta, division, currentTempo )
		except EOFError as e:
			print( e )
			print( "AAAAAAAAAAAAAAAAAAAH" )
		except Exception as e:
			raise e

		status = memblock[ readPosition ]
		print( "Position: " + str( readPosition ) + "\t\t" + hex( status ))
		readPosition += 1
		if status < Event.eType._NOTE_OFF:
			status = lastEvent
			readPosition -= 1
		if status >= Event.eType._NOTE_OFF and status < Event.eType._SYSEX:
			eventType = status & 0x0F
			channel = status & 0xF0
			primary = memblock[ readPosition ]

			# 3 bytes
			if eventType > Event.eType._NOTE_OFF and eventType < Event.eType._PROGRAM or eventType == Event.eType._PITCH_WHEEL:
				readPosition += 1
				secondary = memblock[ readPosition ]
				if eventType == Event.eType._NOTE_OFF:		track.noteOff.append( ChannelEvent( currentTime, status, primary, secondary ) )
				elif eventType == Event.eType._NOTE_ON:		track.noteOn.append( ChannelEvent( currentTime, status, primary, secondary ) )
				elif eventType == Event.eType._AFTERTOUCH:	track.aftertouch.append( ChannelEvent( currentTime, status, primary, secondary ) )
				elif eventType == Event.eType._CONTROLLER:	track.controller.append( ChannelEvent( currentTime, status, primary, secondary ) )
				elif eventType == Event.eType._PITCH_WHEEL:	
					bend = ConvertParametersToPitchBend( primary, secondary )
					track.pitchBend.append( PitchBend( currentTime, status, bend ) )

				break
			# 2 bytes
			elif eventType == Event.eType._PROGRAM:			track.programChange.append( ChannelEvent( currentTime, status, primary ) )
			elif eventType == Event.eType._PRESSURE:		track.channelPressure.append( ChannelEvent( currentTime, status, primary ) )
		elif status >= Event.eType._SYSEX and status < Event.eType._META:
			break
		elif status == Event.eType._META:
			# x bytes
			metaType = memblock[ readPosition ]
			readPosition += 1
			range = readPosition + memblock[ readPosition ] + 1
			readPosition += 1

			contents = bytearray()
			for byte in memblock[ readPosition : range ]:
				contents.append( byte )	
			readPosition = range - 1

			# Broad if statement for text-based Meta Events:
			# if metaType == Event.eMeta._SEQUENCE or metaType == Event.eMeta._TEXT or metaType == Event.eMeta._COPYRIGHT
			# or metaType == Event.eMeta._LYRIC or metaType == Event.eMeta._MARKER or metaType == Event.eMeta._CUE:
			#	track.metaEvent.append( MetaEvent( currentTime, metaType, str( contents, "ascii" ) ) )

			if metaType != Event.eMeta._END_OF_TRACK:
				track.metaEvent.append( MetaEvent( currentTime, metaType, str( contents, "ascii" ) ) )
			#if metaType == Event.eMeta._SEQUENCE: break			
			#elif metaType == Event.eMeta._TEXT: break				
			#elif metaType == Event.eMeta._COPYRIGHT: break			
			#elif metaType == Event.eMeta._NAME_SEQUENCE: break
			#elif metaType == Event.eMeta._NAME_INSTRUMENT: break
			#elif metaType == Event.eMeta._LYRIC: break				
			#elif metaType == Event.eMeta._MARKER: break
			#elif metaType == Event.eMeta._CUE: pass
			#elif metaType == Event.eMeta._NAME_PROGRAM: break
			#elif metaType == Event.eMeta._NAME_DEVICE: break
			#elif metaType == Event.eMeta._CHANNEL_PREFIX: break
			#elif metaType == Event.eMeta._PORT: break
			elif metaType == Event.eMeta._END_OF_TRACK: 
				finished = True
			#elif metaType == Event.eMeta._TEMPO: break
			#elif metaType == Event.eMeta._SMPTE_OFFSET: break
			#elif metaType == Event.eMeta._TIME_SIGNATURE: break
			#elif metaType == Event.eMeta._KEY_SIGNATURE: break
			#elif metaType == Event.eMeta._SEQUENCER_SPECIFIC: break

		readPosition += 1
		lastEvent = status

	return track

def ProcessDebug( memblock, readPosition, division = Default._DIVISION, globalTrack = TrackGlobal() ):	
	currentTime = 0.0
	currentTempo = Default._TEMPO
	globalTempo = iter( globalTrack.tempo )
	try:
		nextTempo = next( globalTempo )
	except StopIteration as e:
		nextTempo = Tempo( 0, -1 )
	
	lastEvent = 0x00

	finished = False	
	while finished == False:
		start = readPosition
		delta = 0
		try:
			delta, readPosition = ProcessDelta( memblock, readPosition )
			if nextTempo.tempo != -1 and currentTime >= nextTempo.time:
				input( str( nextTempo.time ) + " " + str( nextTempo.tempo ) )
				currentTempo = nextTempo.tempo
				try:
					nextTempo = next( globalTempo )
					input( str( nextTempo.time ) + " " + str( nextTempo.tempo ) )
				except StopIteration as e:
					nextTempo = Tempo( 0, -1 )
			currentTime = currentTime + ConvertDeltaToTime( delta, division, currentTempo )
		except EOFError as e:
			print( e )
			print( "AAAAAAAAAAAAAAAAAAAH" )
		except Exception as e:
			raise e

		status = memblock[ readPosition ]
		readPosition += 1

		print( "Start: " + str( start ) + "\t" + hex( status ))
		if status < Event.eType._NOTE_OFF:
			status = lastEvent
			print( "\tNo Status Byte." )
			readPosition -= 1
		if status >= Event.eType._NOTE_OFF and status < Event.eType._SYSEX:
			eventType = status & 0xF0
			channel = status & 0x0F
			primary = memblock[ readPosition ]

			# 3 bytes
			if eventType >= Event.eType._NOTE_OFF and eventType < Event.eType._PROGRAM or eventType == Event.eType._PITCH_WHEEL:
				readPosition += 1
				secondary = memblock[ readPosition ]

				print( "\tEvent Type: " + hex( eventType ) + "\tChannel: " + hex( channel ) + " Pitch: " + hex( primary ) + " Velocity: " + hex( secondary ) )
			# 2 bytes
			else:
				print( "\tEvent Type: " + hex( eventType ) + "\tChannel: " + hex( channel ) + " Primary: " + hex( primary ) )

		elif status >= Event.eType._SYSEX and status < Event.eType._META:
			print( "\tSysEx." )

		else:
			metaType = memblock[ readPosition ]
			readPosition += 1
			contentStart = readPosition + 1
			range = readPosition + memblock[ readPosition ] + 1
			readPosition += 1

			contents = bytearray()
			for byte in memblock[ readPosition : range ]:
				contents.append( byte )	
			readPosition = range - 1

			if metaType == Event.eMeta._END_OF_TRACK: 
				input( "> " )
				finished = True
			#elif metaType == Event.eMeta._TEMPO: break
			#elif metaType == Event.eMeta._SMPTE_OFFSET: break
			#elif metaType == Event.eMeta._TIME_SIGNATURE: break
			#elif metaType == Event.eMeta._KEY_SIGNATURE: break
			#elif metaType == Event.eMeta._SEQUENCER_SPECIFIC: break

			print( "\tMeta Type: " + hex( metaType) + "\tContent Start: " + str( contentStart) + " Range: " + str( range ) + "\n\tContents: " + str( contents ) )
	
		debugStart = 999999999
		if start >= debugStart:			
			input( "> " )

		readPosition += 1
		lastEvent = status

	return readPosition

# Generates MIDIObjects.Track's from MIDI file.
def Process( filePath = None, numArrs = None ):
	memblock = GetMIDI( filePath )
	if memblock is None:
		raise Exception( "Failed to create memory block of file contents." )

	division = ( memblock[12] * 256 ) + memblock[13]
	readPosition = Default._HEADERLENGTH
	tracks = []

	if memblock[ Default._FILETYPEBYTE ] == 0:
		track = ProcessSimple( memblock, readPosition + Default._TRACKHEADERLENGTH, division )
		tracks.append( track )
	else:
		#ProcessDebug( memblock, readPosition + Default._TRACKHEADERLENGTH, division, TrackGlobal() )
		globalTrack, readPosition = ProcessGlobal( memblock, readPosition + Default._TRACKHEADERLENGTH, division )
		#DebugGlobalTrack( globalTrack )
		while readPosition < len( memblock ):
			#readPosition = ProcessDebug( memblock, readPosition + Default._TRACKHEADERLENGTH, division, globalTrack ) 
			newTrack, readPosition = ProcessTrack( memblock, readPosition + Default._TRACKHEADERLENGTH, division, globalTrack )
			DebugTrack( newTrack )
			tracks.append( newTrack )

	return tracks

def MIDIReader( fileName = str ):
	try:					
		return Process( fileName )
	except Exception as e:	
		print( "Error: " + str( e ) )