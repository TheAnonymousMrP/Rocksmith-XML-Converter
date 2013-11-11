from sys import argv
from MIDIReader import MIDIReader

def main():
	script, fileName = argv

	tracks = MIDIReader( fileName )

main()