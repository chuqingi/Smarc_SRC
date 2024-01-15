/****************************************************************************
Developer: Jiaxin Li
E-mail: 1319376761@qq.com
Github: https://github.com/chuqingi/Smarc_SRC
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "smarc.h"
#include "math.h"


#define BUF_SIZE 160
#define BANDWIDTH "0.95"
#define RP "0.1"
#define RS "140"
#define TOL "0.000001"


/*The WAVE file format*/
typedef struct tagWAVHEADER {
	uint8_t   ChunkID[4];     
	uint32_t  ChunkSize;      
	uint8_t   Format[4];      
	uint8_t   FmtChunkID[4];  
	uint32_t  FmtChunkSize;   
	uint16_t  AudioFormat;    
	uint16_t  NumChannels;    
	uint32_t  SampleRate;     
	uint32_t  ByteRate;       
	uint16_t  BlockAlign;     
	uint16_t  BitsPerSample;
	uint8_t   DataChunkID[4];
	uint32_t  DataChunkSize;
} WAVHEADER;


/**
 * Resample each input file channels separately, output file has same number of channels.
 */
void resample_separately(struct PFilter* pfilt, FILE* fin, FILE* fout, int nbChannels) {
	// init state and buffer per channels
	struct PState* pstate[nbChannels];
	for (int c = 0; c < nbChannels; c++)
		pstate[c] = smarc_init_pstate(pfilt);
	const int IN_BUF_SIZE = BUF_SIZE;
	const int OUT_BUF_SIZE = (int)smarc_get_output_buffer_size(pfilt, IN_BUF_SIZE);
	double* inbuf = malloc(IN_BUF_SIZE * sizeof(double));
	double* outbuf = malloc(OUT_BUF_SIZE * sizeof(double));
	int16_t* readBuf = malloc(IN_BUF_SIZE * nbChannels * sizeof(int16_t));
	int16_t* writeBuf = malloc(OUT_BUF_SIZE * nbChannels * sizeof(int16_t));
	int read = 0;
	int written = 0;
	// resample audio
	while (1) {
		read = fread(readBuf, sizeof(int16_t), IN_BUF_SIZE * nbChannels, fin);
		if (read == 0) {
			// reached end of file, have to flush last values
			break;
		}
		// resample each channels
		for (int c = 0; c < nbChannels; c++) {
			for (int i = 0; i < IN_BUF_SIZE; i++)
				inbuf[i] = (double)(readBuf[i * nbChannels + c] / 32767.0);
			written = smarc_resample(pfilt, pstate[c], inbuf, IN_BUF_SIZE, outbuf, OUT_BUF_SIZE);
			for (int i = 0; i < written; i++)
				writeBuf[i * nbChannels + c] = (int16_t)(outbuf[i] * 32767);
		}
		fwrite(writeBuf, sizeof(int16_t), written * nbChannels, fout);
	}
	// flushing last values
	while (1) {
		for (int c = 0; c < nbChannels; c++) {
			written = smarc_resample_flush(pfilt, pstate[c], outbuf, OUT_BUF_SIZE);
			for (int i = 0; i < written; i++)
				writeBuf[i * nbChannels + c] = (int16_t)(outbuf[i] * 32767);
		}
		fwrite(writeBuf, sizeof(int16_t), written * nbChannels, fout);
		if (written < OUT_BUF_SIZE)
			break;
	}
	// release memory
	for (int c = 0; c < nbChannels; c++)
		smarc_destroy_pstate(pstate[c]);
	free(inbuf);
	free(outbuf);
	free(readBuf);
	free(writeBuf);
}


int main(int argc, char** argv) {
  	WAVHEADER* WAVE = malloc(sizeof(WAVHEADER));
	FILE* fh = fopen(argv[1], "rb");
	if (!fh) {
		printf("ERROR: cannot open file \n");
		exit(1);
	}
	fread(WAVE, sizeof(WAVHEADER), 1, fh);
	int fsin = WAVE->SampleRate;
	int fsout = atoi(argv[3]);  // target samplerate
	if (fsin==fsout) {
		printf("ERROR: invalid samplerate, input %d Hz = ouput %d Hz\n",fsin,fsout);
		exit(1);
	}
	// init filter
	double bandwidth = atof(BANDWIDTH);
	if (bandwidth<=0 || bandwidth>=1) {
		printf("ERROR: invalid bandwidth parameter %f, should be in 0 < .. < 1\n",bandwidth);
		exit(1);
	}
	double rpvalue = atof(RP);
	if (rpvalue<=0) {
		printf("ERROR: invalid rp parameter %f, should be > 0\n",rpvalue);
		exit(1);
	}
	double rsvalue = atof(RS);
	if (rsvalue<=0) {
		printf("ERROR: invalid rs parameter %f, should be > 0\b",rsvalue);
		exit(1);
	}
	double tolerance = atof(TOL);
	const char* userratios = "";
	struct PFilter* pfilt = smarc_init_pfilter(fsin, fsout, bandwidth, rpvalue, rsvalue, tolerance, userratios, 1);
	if (pfilt == NULL) {
		exit(1);
	}
	// init channels
	int inc = WAVE->NumChannels;
	int outc = inc;
	// init length
	int inlen = WAVE->DataChunkSize / sizeof(int16_t);
	int outlen = (int)smarc_get_output_buffer_size(pfilt, inlen);
	// open output file
	FILE* fhout = fopen(argv[2], "wb");
	WAVE->SampleRate = fsout;
	WAVE->DataChunkSize = outlen * sizeof(int16_t);
  	fwrite(WAVE, sizeof(WAVHEADER), 1, fhout);
	// resample
	resample_separately(pfilt, fh, fhout, inc);
	// close files
	fclose(fh);
	fclose(fhout);
	free(WAVE);
	// release filter
	smarc_destroy_pfilter(pfilt);
	return 0;
}