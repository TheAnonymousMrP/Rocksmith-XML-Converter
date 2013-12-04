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