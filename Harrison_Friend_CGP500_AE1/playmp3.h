

/*
	MP3 decoder

	Minimal PS4 Mp3 Decode
	- loads frame-by-frame blocks from the mp3 file
	- decode each block and plays


	Requires following PS4 system libraries:
	-lSceAjm_stub_weak
	-lSceAudioOut_stub_weak
	-lSceUserService_stub_weak


	How to use:

	#include "playmp3.h"

	void main()
	{
		// Renderer class
		DecodeAndPlayMp3( "/app0/allegro.mp3" );

	}// End main(..)

*/

namespace Solent
{
	// Decodes and plays .mp3 file
	// Blocking (i.e., doens't return until the file has completed playing)
	extern int DecodeAndPlayMp3(const char* mp3FileName);

}// End Solent namespace