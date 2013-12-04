from MIDIObjects import Default, Track, Global

def GOTHERE( location = None ):
	input( "Got here. " + str( location ) )

def DebugGlobal( track = Global() ):
	print( "Global Track: " )
	print( "SMPTE Offset: " + str( track.smpteOffset ) )

	print( "Meta Events: " )
	for m in track.metaEvent:
		print( "\tTime: " + "{0:.2f}".format( m.time ) )
		print( "\t\tType: " + str( m.type ) + "\tMessage: " + str( m.message ) )

	print( "Tempos: " )
	for t in track.tempo:
		print( "\tTime: " + "{0:.2f}".format( t.time ) + "\tTempo: " + str( t.tempo ) )
		
	print( "Time Signatures: " )
	for ts in track.timeSig:
		print( "\tTime: " + "{0:.2f}".format( ts.time ) )

	print( "Key Signatures: " )
	for ks in track.keySig:
		print( "\tTime: " + "{0:.2f}".format( ks.time ) )

def DebugTrack( track = Track() ):
	print( "Track: " )
	print( "Meta Events: " )
	for i in track.metaEvent:
		print( "\tTime: " + str( i.time ) )
		print( "\t\tType: " + str( i.type ) + "\tMessage: " + str( i.message ) )

	print( "Notes On: " )
	for i in track.noteOn:
		print( "\tTime: " + str( i.time ) )

	print( "Pitch Bend: " )
	for i in track.pitchBend:
		print( "\tTime: " + str( i.time ) )
		print( "\t\tBend: " + str( i.bend ) )

class Antiquated:
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

	def ProcessTrack( memblock, readPosition, division = Default._DIVISION ):
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
