from sys import argv
from MIDIReader import MIDIReader
from RSXMLArranger import RSXMLArranger, RSXMLArrangerFlags
from RSXMLWriter import RSXMLWriter

def main():
	script, fileName, trackName = argv

	arrFlags = RSXMLArrangerFlags()
	arrFlags.hasChanneledBends = False
	arrFlags.hasManualAnchors = False
	arrFlags.quantise = 2

	midiFile = MIDIReader( fileName )
	midiFile.name = trackName

	arrangements = RSXMLArranger( midiFile, arrFlags )

	RSXMLWriter( arrangements )

main()