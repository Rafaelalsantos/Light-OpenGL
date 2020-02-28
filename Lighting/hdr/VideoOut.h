#pragma once
#include <iostream>


extern "C"
{
//#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}



class VideoOut
{
public:
	AVCodecContext *c = nullptr;
	AVFrame *frame;
	struct SwsContext *sws_context = nullptr;
	const AVCodec *codec;
	FILE *f;
	AVPacket *pkt;
	//uint8_t endcode[];
	unsigned char endcode[4] = { 0, 0, 1, 0xb7 };
	unsigned char *rgb;
	 

	unsigned int width, height, k;


	void writeFrame(unsigned char *pixels);

	VideoOut(const char *filename, const char *codec_name, unsigned int width, unsigned int height);
	~VideoOut();
private:
	void encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt, FILE *outfile);
	void ffmpeg_encoder_set_frame_yuv_from_rgb(uint8_t *rgb);
};

