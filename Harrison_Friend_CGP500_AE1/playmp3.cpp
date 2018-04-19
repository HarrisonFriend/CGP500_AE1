

/*
	see .h for details
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <ajm/mp3_decoder.h>
#include <audioout.h>

#include <stdint.h>
#include <ajm.h>

#include <libdbg.h>			  // SCE_BREAK(..)

// Custom assert - if the boolean test fails the
// code will trigger the debugger to halt
#define DBG_ASSERT(f) { if (!(f)) { SCE_BREAK(); } }


#ifdef __ORBIS__
size_t sceLibcHeapSize = 1000*1024*1024;
unsigned int sceLibcHeapExtendedAlloc = 1;
#endif




namespace Solent
{


// MP3 information we are interested in.
typedef struct MP3Info 
{
	int iSamplePerFrame;
	int iFrameSize;
	int iNumChannel;
	int iSampleRate;
	int iLoopStart;
	int iLoopEnd;
	int iTotalSamples;
} MP3_Info;


// AJM context.
static SceAjmContextId	uiContext		= -1;
static int32_t			sceAudiohandle	= 0;

// Get bitstream information into a simple data structure.
static void GetStreamInfo(MP3Info *pInfo, FILE *pFileInput)
{
	// See https://en.wikipedia.org/wiki/MP3
	// TODO: parse these from the first frame in the bitstream.
	pInfo->iSamplePerFrame = 1152;
	pInfo->iNumChannel     = 2;
	pInfo->iSampleRate     = 48000;
	pInfo->iFrameSize      = 1441; //SCE_AJM_DEC_MP3_MAX_FRAME_SIZE
}

// Decode a file - frame by frame (reads -> decodes -> plays)
static 
void StreamDecodeMp3(SceAjmInstanceId uiInstance, 
					 MP3Info*		 pInfo, 
					 FILE*			 pFileInput)
{
	char			cBatch[SCE_AJM_JOB_DECODE_SIZE];
	static uint8_t	cInputBuffer[SCE_AJM_DEC_MP3_MAX_FRAME_SIZE];
	static int16_t	cOutputBuffer[ 4608 ] ;
	SceAjmBatchId	uiBatch;
	SceAjmBatchInfo sBatch;

	int seekOffset = 0;

	// Stream the data into for playing
	const int tmpOutputBufferSize	= 1024*1024*5;
	char * tmpOutputBuffer			= new char[tmpOutputBufferSize];
	int    tmpOffset				= 0;


	while (!feof(pFileInput)) 
	{
		sceAjmBatchInitialize(cBatch, sizeof (cBatch), &sBatch);

		fseek( pFileInput, seekOffset,  SEEK_SET );

		int iRet = fread(cInputBuffer, 1, pInfo->iFrameSize, pFileInput);
		if (iRet != (size_t)pInfo->iFrameSize) 
		{
			if (feof(pFileInput)) 
			{
				break;
			}
			fprintf(stderr, "Error reading input: %s\n", strerror(errno));
			DBG_ASSERT(false);
			exit(1);
		}

		DBG_ASSERT( cInputBuffer[0] == 0xff );


		#if 1
		SceAjmDecodeSingleResult dec;
		int outpcm = sizeof (cOutputBuffer);
		DBG_ASSERT( sceAjmBatchJobDecodeSingle(	 &sBatch, 
												 uiInstance, 
												 cInputBuffer, 
												 pInfo->iFrameSize,
												 cOutputBuffer, 
												 outpcm, 
												 &dec) >= 0);

		DBG_ASSERT( sceAjmBatchStart(uiContext, &sBatch, SCE_AJM_PRIORITY_GAME_DEFAULT, NULL, &uiBatch) >= 0 );
		
		DBG_ASSERT( sceAjmBatchWait(uiContext, uiBatch, SCE_AJM_WAIT_INFINITE, NULL) >= 0 );

		if (dec.sResult.iResult != 0) 
		{
			printf("AjmJob: iResult = 0x%08X, iInternalResult = 0x%08X\n", dec.sResult.iResult, dec.sResult.iInternalResult);
		}

		if (dec.sResult.iResult < 0 )
		{
			DBG_ASSERT(false);
		}
		seekOffset += dec.sStream.iSizeConsumed;
		#endif

		// Copy the decode data into a buffer
		memcpy( &tmpOutputBuffer[tmpOffset], cOutputBuffer, dec.sStream.iSizeProduced );
		tmpOffset += dec.sStream.iSizeProduced;
		DBG_ASSERT( tmpOffset < tmpOutputBufferSize );

		// Once we have over 1024 bytes of data in our buffer
		// play the data and subtract the 1024 bytes
		while ( tmpOffset > 1024 )
		{
			//int ret = 
			sceAudioOutOutput(sceAudiohandle, &tmpOutputBuffer[0]);
			//DBG_ASSERT(ret==1024);

			memcpy(&tmpOutputBuffer[0], &tmpOutputBuffer[1024], tmpOutputBufferSize-2014);
			tmpOffset -= 1024;
		}// End tmpOffset

	}// End while (!feof(pFileInput)) 

	delete[] tmpOutputBuffer;
	tmpOutputBuffer = NULL;
}// End DecodeMp3(..)





static 
void StartPlayingMP3(const char* mp3FileName)
{

	#define AUDIO_MONAURAL (1)
	#define AUDIO_STEREO   (2)
	#define AUDIO_SURROUND (8)

	int32_t volume[AUDIO_STEREO] = {
		SCE_AUDIO_VOLUME_0dB / 4,
		SCE_AUDIO_VOLUME_0dB / 4,
	};

	DBG_ASSERT( sceAudioOutInit()>= 0 );

	// Output 256 samples at once
	sceAudiohandle = 
	sceAudioOutOpen(  SCE_USER_SERVICE_USER_ID_SYSTEM, // userId
					  SCE_AUDIO_OUT_PORT_TYPE_MAIN,
					  0,
					  256, 
					  48000,
					  SCE_AUDIO_OUT_PARAM_FORMAT_S16_STEREO);
	if (sceAudiohandle < 0)
	{
		DBG_ASSERT(false); // Failed to set 
	}

	sceAudioOutSetVolume(sceAudiohandle, 
						(SCE_AUDIO_VOLUME_FLAG_L_CH | SCE_AUDIO_VOLUME_FLAG_R_CH), volume);

	FILE*			 pFileInput	= 0;
	SceAjmInstanceId uiInstance = -1;
	MP3Info			 sInfo;

	// Open input and output files. 
	pFileInput = fopen(mp3FileName, "rb");
	DBG_ASSERT( pFileInput ); // "Can't find file"


	// Create AJM MP3 instance.
	DBG_ASSERT( sceAjmInstanceCreate(uiContext, 
								SCE_AJM_CODEC_MP3_DEC,
								SCE_AJM_INSTANCE_FLAG_MAX_CHANNEL(2) |
								SCE_AJM_INSTANCE_FLAG_FORMAT(SCE_AJM_FORMAT_ENCODING_S16) |
								SCE_AJM_INSTANCE_FLAG_DEC_MP3_IGNORE_OFL,
								&uiInstance) == SCE_OK );

	GetStreamInfo(&sInfo, pFileInput);

	StreamDecodeMp3(	uiInstance, 
						&sInfo, 
						pFileInput);


	sceAjmInstanceDestroy(uiContext, uiInstance);
	fclose(pFileInput);
}// End StartPlayingMP3(..)




// Decode an MP3 file with loops.
int DecodeAndPlayMp3(const char* mp3FileName)
{
	// Initialize AJM.
	DBG_ASSERT( sceAjmInitialize(0, &uiContext) == SCE_OK );
	
	// Register AJM
	DBG_ASSERT( sceAjmModuleRegister(uiContext, SCE_AJM_CODEC_MP3_DEC, 0) == SCE_OK );

	
	StartPlayingMP3(mp3FileName);

	// Cleanup AJM.
	DBG_ASSERT( sceAjmModuleUnregister(uiContext, SCE_AJM_CODEC_MP3_DEC) == SCE_OK );

	DBG_ASSERT( sceAjmFinalize(uiContext) == SCE_OK );

	return 0;
}// End DecodeAndPlayMp3(..)




}// End Solent namespace