#rsxml.meta[ "" ] = 0
rsxml.meta[ "version" ] = 0.1
rsxml.meta[ "title" ] = midi.name
#rsxml.meta[ "arrangement" ] = midi.nameTrack
#rsxml.meta[ "waveFilePath" ]
#rsxml.meta[ "part" ] 
rsxml.meta[ "offset" ] = midi.globalTrack.smpteOffset
#rsxml.meta[ "centOffset" ]  
rsxml.meta[ "songLength" ] = midi.length
#rsxml.meta[ "internalName" ] #
#rsxml.meta[ "songNameSort" ] #
rsxml.meta[ "startBeat" ] = 0
averageTempo = 0.0
for tempo in midi.globalTrack.tempo:  
	averageTempo += tempo.tempo
averageTempo /= len( midi.globalTrack.tempo )
rsxml.meta[ "averageTempo" ] = averageTempo # 
#rsxml.meta[ "tuning" ] = Tuning._DEFAULT[ "E Standard" ]	
#rsxml.meta[ "capo" ]
#rsxml.meta[ "artistName" ]
#rsxml.meta[ "artistNameSort" ] #
#rsxml.meta[ "albumName" ]
#rsxml.meta[ "albumNameSort" ] #
#rsxml.meta[ "albumYear"	] #
#rsxml.meta[ "albumArt" ] #
rsxml.meta[ "crowdSpeed" ] = 1
#rsxml.meta[ "arrangementProperties"	]
#rsxml.meta[ "lastConversionTime" ]

class noteTechniqueDict:
	_TECHNIQUES = OrderedDict( [ ( "linkNext", 0 ), ( "bend", 0 ), ( "hopo", 0 ), ( "hammerOn", 0 ), ( "pullOff", 0 ), 
			( "harmonic", 0 ), ( "harmonicPinch", 0 ), ( "ignore", 0 ), ( "leftHand", 1 ), ( "rightHand", 0 ), 
			( "mute", 0 ), ( "palmMute", 0 ), ( "pickDirection", 0 ), ( "pluck", -1 ), ( "slap", -1 ), 
			( "slideTo", -1 ), ( "slideUnpitchTo", -1 ), ( "sustain", float( 0 ) ), ( "tap", 0 ), ( "tremolo", 0 ), ( "vibrato", 0 ) ] )

	flags = RSXML.Note._TECHNIQUES

	if event.primary == Default._TECHNIQUECONTROLLER[ "bend" ]:
		# A flag for bends when Pitch Wheel doesn't want to play with channels.
		bends = YieldBends( note, rsxml.bendsOmni, False )
				
		maxBend = 0.0
		for i in bends:
			if i.step > maxBend: 
				maxBend = i.step

		if maxBend < 1.0:
			maxBend = 1.0

		flags[ "bend" ] = maxBend
	elif event.primary == Default._TECHNIQUECONTROLLER[ "hopo" ]:
		# Covers both hammer on and pull off.
		if event.secondary >= 64:
			flags[ "hopo" ] = 1
			flags[ "hammerOn" ] = 1
			flags[ "hammerOff" ] = 1
		else:
			flags[ "hopo" ] = 0
			flags[ "hammerOn" ] = 0
			flags[ "hammerOff" ] = 0
	elif event.primary == Default._TECHNIQUECONTROLLER[ "harmonic" ]:
		if event.secondary >= 64:
			flags[ "harmonic" ] = 1
		else:
			flags[ "harmonic" ] = 0
	elif event.primary == Default._TECHNIQUECONTROLLER[ "harmonicPinch" ]:
		if event.secondary >= 64:
			flags[ "harmonicPinch" ] = 1
		else:
			flags[ "harmonicPinch" ] = 0
	elif event.primary == Default._TECHNIQUECONTROLLER[ "mute" ]:
		if event.secondary >= 64:
			flags[ "mute" ] = 1
		else:
			flags[ "mute" ] = 0
	elif event.primary == Default._TECHNIQUECONTROLLER[ "palmMute" ]:
		if event.secondary >= 64:
			flags[ "palmMute" ] = 1
		else:
			flags[ "palmMute" ] = 0
	elif event.primary == Default._TECHNIQUECONTROLLER[ "slideTo" ]:
		if 0 < event.secondary <= Default._MAXFRETS:
			flags[ "slideTo" ] = event.secondary
		else:
			flags[ "slideTo" ] = -1
	elif event.primary == Default._TECHNIQUECONTROLLER[ "slideUnpitchTo" ]:
		if 0 < event.secondary <= Default._MAXFRETS:
			flags[ "slideUnpitchTo" ] = event.secondary
		else:
			flags[ "slideUnpitchTo" ] = -1
	elif event.primary == Default._TECHNIQUECONTROLLER[ "tap" ]:
		if event.secondary >= 64:
			flags[ "tap" ] = 1
		else:
			flags[ "tap" ] = 0
	elif event.primary == Default._TECHNIQUECONTROLLER[ "tremolo" ]:
		if event.secondary >= 64:
			flags[ "tremolo" ] = 1
		else:
			flags[ "tremolo" ] = 0
	elif event.primary == Default._TECHNIQUECONTROLLER[ "vibrato" ]:
		if event.secondary >= 64:
			flags[ "vibrato" ] = 1
		else:
			flags[ "vibrato" ] = 0
	# Bass
	elif event.primary == Default._TECHNIQUECONTROLLER[ "pluck" ]:
		pass
	elif event.primary == Default._TECHNIQUECONTROLLER[ "slap" ]:
		pass

class noteTechniqueClass:
	class Techniques:
		def __init__( self ):
			self.linkNext			= 0
			self.hopo				= 0
			self.hammerOn			= 0
			self.pullOff			= 0
			self.harmonic			= 0
			self.harmonicPinch		= 0
			self.ignore				= 0
			self.leftHand			= 1
			self.rightHand			= 0
			self.mute				= 0
			self.palmMute			= 0
			self.pickDirection		= 0
			self.pluck				= -1
			self.slap				= -1
			self.slideTo			= -1
			self.slideUnpitchTo		= -1
			self.tap				= 0
			self.tremolo			= 0
			self.vibrato			= 0

	flag = RSXML.Note.Techniques()

	if event.primary == Default._TECHNIQUECONTROLLER[ "bend" ]:
		# A flag for bends when Pitch Wheel doesn't want to play with channels.
		bends = YieldBends( note, rsxml.bendsOmni, False )
				
		maxBend = 0.0
		for i in bends:
			if i.step > maxBend: 
				maxBend = i.step

		if maxBend < 1.0:
			maxBend = 1.0

		note.bend = maxBend
	elif event.primary == Default._TECHNIQUECONTROLLER[ "hopo" ]:
		# Covers both hammer on and pull off.
		if event.secondary >= 64:
			flags.hopo = 1
			flags.hammerOn = 1
			flags.hammerOff = 1
		else:
			flags.hopo = 0
			flags.hammerOn = 0
			flags.hammerOff = 0
	elif event.primary == Default._TECHNIQUECONTROLLER[ "harmonic" ]:
		if event.secondary >= 64:
			flags.harmonic = 1
		else:
			flags.harmonic = 0
	elif event.primary == Default._TECHNIQUECONTROLLER[ "harmonicPinch" ]:
		if event.secondary >= 64:
			flags.harmonicPinch = 1
		else:
			flags.harmonicPinch = 0
	elif event.primary == Default._TECHNIQUECONTROLLER[ "mute" ]:
		if event.secondary >= 64:
			flags.mute = 1
		else:
			flags.mute = 0
	elif event.primary == Default._TECHNIQUECONTROLLER[ "palmMute" ]:
		if event.secondary >= 64:
			flags.palmMute = 1
		else:
			flags.palmMute = 0
	elif event.primary == Default._TECHNIQUECONTROLLER[ "slideTo" ]:
		if 0 < event.secondary <= Default._MAXFRETS:
			flags.slideTo = event.secondary
		else:
			flags.slideTo = -1
	elif event.primary == Default._TECHNIQUECONTROLLER[ "slideUnpitchTo" ]:
		if 0 < event.secondary <= Default._MAXFRETS:
			flags.slideUnpitchTo = event.secondary
		else:
			flags.slideUnpitchTo = -1
	elif event.primary == Default._TECHNIQUECONTROLLER[ "tap" ]:
		if event.secondary >= 64:
			flags.tap = 1
		else:
			flags.tap = 0
	elif event.primary == Default._TECHNIQUECONTROLLER[ "tremolo" ]:
		if event.secondary >= 64:
			flags.tremolo = 1
		else:
			flags.tremolo = 0
	elif event.primary == Default._TECHNIQUECONTROLLER[ "vibrato" ]:
		if event.secondary >= 64:
			flags.vibrato = 1
		else:
			flags.vibrato = 0
	# Bass
	elif event.primary == Default._TECHNIQUECONTROLLER[ "pluck" ]:
		pass
	elif event.primary == Default._TECHNIQUECONTROLLER[ "slap" ]:
		pass