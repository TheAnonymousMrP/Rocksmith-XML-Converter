from sys import argv
from MIDIReader import MIDIReader
from RSXMLArranger import RSXMLArranger
from RSXMLWriter import RSXMLWriter

def main():
	script, fileName = argv

	midiFile = MIDIReader( fileName )
	RSXMLArranger( midiFile )

main()