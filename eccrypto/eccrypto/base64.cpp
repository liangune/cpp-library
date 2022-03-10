/*************************************************************************
	> Date:   2020-12-08
	> Author: liangjun
	> Desc:   base64编解码
*************************************************************************/

/*
 ** Translation Table as described in RFC1113
 */
#include "base64.h"
#include <string.h>

static const char encodeCharacterTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char base64_pad = '=';

static const signed char decodeCharacterTable[256] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

int base64_encode(uint8_t *input, size_t input_length, uint8_t *output)
{
	uint8_t buff1[3];
	uint8_t buff2[4];
	uint8_t i=0, j;
	size_t input_cnt=0;
	size_t output_cnt=0;

	while(input_cnt<input_length)
	{
		buff1[i++] = input[input_cnt++];
		if (i==3)
		{
			output[output_cnt++] = encodeCharacterTable[(buff1[0] & 0xfc) >> 2];
			output[output_cnt++] = encodeCharacterTable[((buff1[0] & 0x03) << 4) + ((buff1[1] & 0xf0) >> 4)];
			output[output_cnt++] = encodeCharacterTable[((buff1[1] & 0x0f) << 2) + ((buff1[2] & 0xc0) >> 6)];
			output[output_cnt++] = encodeCharacterTable[buff1[2] & 0x3f];
			i=0;
		}
	}
	if (i)
	{
		for(j=i;j<3;j++)
		{
			buff1[j] = '\0';
		}
		buff2[0] = (buff1[0] & 0xfc) >> 2;
		buff2[1] = ((buff1[0] & 0x03) << 4) + ((buff1[1] & 0xf0) >> 4);
		buff2[2] = ((buff1[1] & 0x0f) << 2) + ((buff1[2] & 0xc0) >> 6);
		buff2[3] = buff1[2] & 0x3f;
		for (j=0;j<(i+1);j++)
		{
			output[output_cnt++] = encodeCharacterTable[buff2[j]];
		}
		while(i++<3)
		{
			output[output_cnt++] = '=';
		}
	}
	if( output )
		output[output_cnt] = 0;
	return output_cnt;
}

int base64_decode(uint8_t *input, size_t input_length, uint8_t *output)
{
	uint8_t buff1[4];
	uint8_t buff2[4];
	uint8_t i=0, j;
	size_t input_cnt=0;
	size_t output_cnt=0;

	while(input_cnt<input_length)
	{
		buff2[i] = input[input_cnt++];
		if (buff2[i] == '=')
		{
			break;
		}
		if (++i==4)
		{
			for (i=0;i!=4;i++)
			{
				buff2[i] = decodeCharacterTable[buff2[i]];
			}
			output[output_cnt++] = (char)((buff2[0] << 2) + ((buff2[1] & 0x30) >> 4));
			output[output_cnt++] = (char)(((buff2[1] & 0xf) << 4) + ((buff2[2] & 0x3c) >> 2));
			output[output_cnt++] = (char)(((buff2[2] & 0x3) << 6) + buff2[3]);
			i=0;
		}
	}
	if (i)
	{
		for (j=i;j<4;j++)
		{
			buff2[j] = '\0';
		}
		for (j=0;j<4;j++)
		{
			buff2[j] = decodeCharacterTable[buff2[j]];
		}
		buff1[0] = (buff2[0] << 2) + ((buff2[1] & 0x30) >> 4);
		buff1[1] = ((buff2[1] & 0xf) << 4) + ((buff2[2] & 0x3c) >> 2);
		buff1[2] = ((buff2[2] & 0x3) << 6) + buff2[3];
		for (j=0;j<(i-1); j++)
		{
			output[output_cnt++] = (char)buff1[j];
		}
	}
	if( output )
		output[output_cnt] = 0;
	return output_cnt;
}

int B64::base64Encode(uint8_t *input, size_t inLength, std::string &sOutput)
{
    char *pszOutput = (char *)malloc(inLength*2+1);
    // memset(pszOutput, 0, inLength*2+1);
    int outLen = base64_encode(input, inLength, (uint8_t *)pszOutput);
    pszOutput[outLen] = 0;

    std::string strOutput(pszOutput, outLen);
    sOutput = strOutput;
    if(pszOutput)
        free(pszOutput);

    return outLen;
}

int B64::base64Decode(uint8_t *input, size_t inLength, std::string &sOutput)
{
    char * pszOutput = (char *)malloc(inLength+1);
    // memset(pszOutput, 0, inLength+1);
    int outLen = base64_decode(input, inLength, (uint8_t *)pszOutput);
    pszOutput[outLen] = 0;

    std::string strOutput(pszOutput, outLen);
    sOutput = strOutput;
    if(pszOutput)
        free(pszOutput);

    return outLen;
}   