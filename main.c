#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#define PI 3.14F

typedef struct {
	char ChunkID[4];
	uint32_t ChunkSize;
	char Format[4];
	char Subchunk1ID[4];
	uint32_t Subchunk1Size;
	uint16_t AudioFormat;
	uint16_t NumChannels;
	uint32_t SampleRate;
	uint32_t ByteRate;
	uint16_t BlockAlign;
	uint16_t BitsPerSample;
	char Subchunk2ID[4];
	uint32_t Subchunk2Size;
} Header;

uint32_t reverseEndian( uint32_t data ) {
	uint32_t new =
		((data & 0xFF000000) >> 24) +
		((data & 0x00FF0000) >> 8) +
		((data & 0x0000FF00) << 8) +
		((data & 0x000000FF) << 24);

	return new;
}

int main(int argc, char ** argv){

	FILE * file;

	uint32_t length; // @ 44100 Hz
	uint32_t data;

	double step;
	double freq = 440.0;

	Header header = {
	.ChunkID = {'R','I','F','F'},
	.Format = {'W','A','V','E'},
	.Subchunk1ID = {'f','m','t',' '},
	.Subchunk1Size = 0x00000010, // Little Endian 16
	.AudioFormat = 0x0001, // Little Endian 1 (PCM)
	.NumChannels = 0x0001, //Little Endian 1 (Mono)
	.SampleRate = 0x0000AC44, // Little Endian 44100 Hz
	.ByteRate = 0x0002B110, // SampleRate * NumChannels * BitsPerSample / 8
	.BlockAlign = 0x0004,
	.BitsPerSample = 0x0020, // Little Endian 32
	.Subchunk2ID = {'d','a','t','a'}	
	};


	if(argc != 2 ){
		fprintf(stdout, "Invalid Usage:\n./a.out <length>\n");
		return -1;
	}

	errno = 0;
	length = strtoul(argv[1], NULL, 0);
	if(errno){
		fprintf(stderr, "strtoul() errno %d %s\n", errno, strerror(errno));
		return -1;
	}

	step = (double) length / freq;

	header.Subchunk2Size = length * 4;
	header.ChunkSize = 36 + header.Subchunk2Size;

	file = fopen("output.wav","wb");
	if(errno){
		fprintf(stderr, "fopen() errno %d %s\n", errno, strerror(errno));
	}
	
	fwrite(&header, sizeof(header), 1, file);

	for(int i = 0; i < length; i++){
		data = sin(2.0 * PI * (float) i * step) * 100.F;
		data = reverseEndian(data);
		fwrite(&data, sizeof(data), 1, file);
	}
	
	fclose(file);

	return 0;

}
