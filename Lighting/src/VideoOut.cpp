#include "VideoOut.h"

VideoOut::VideoOut(const char *filename, const char *codec_name, unsigned int width, unsigned int height) {
	this->width = width;
	this->height = height;
	this->k = 0;
	//this->encode = { 0, 0, 1, 0xb7 };

	int ret;
	
	/* find the mpeg1video encoder */
	codec = avcodec_find_encoder_by_name(codec_name);

	if (!codec) {
		fprintf(stderr, "Codec '%s' not found\n", codec_name);
		exit(1);
	}
	c = avcodec_alloc_context3(codec);
	if (!c) {
		fprintf(stderr, "Could not allocate video codec context\n");
		exit(1);
	}
	pkt = av_packet_alloc();
	if (!pkt)
		exit(1);

	/* put sample parameters */
	c->bit_rate = 400000;
	/* resolution must be a multiple of two */
	c->width = width;
	c->height = height;
	/* frames per second */
	//c->time_base = (AVRational) { 1, 25 };
	//c->framerate = (AVRational) { 25, 1 };
	c->time_base.num = 1;
	c->time_base.den = 30;
	//c->time_base.den = 25;

	//c->framerate.num = 25;
	c->framerate.num = 30;
	c->framerate.den = 1;

	/* emit one intra frame every ten frames
	 * check frame pict_type before passing frame
	 * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
	 * then gop_size is ignored and the output of encoder
	 * will always be I frame irrespective to gop_size
	 */
	c->gop_size = 10;
	c->max_b_frames = 1;
	c->pix_fmt = AV_PIX_FMT_YUV420P;

	if (codec->id == AV_CODEC_ID_H264)
		av_opt_set(c->priv_data, "preset", "slow", 0);

	/* open it */
	ret = avcodec_open2(c, codec, NULL);
	if (ret < 0) {
		char buf[256];
		av_strerror(ret, buf, sizeof(buf));
		printf("Could not open codec: :%s,ret:%d\n", buf, ret);
		exit(1);
	}

	f = fopen(filename, "wb");
	if (!f) {
		fprintf(stderr, "Could not open %s\n", filename);
		exit(1);
	}

	frame = av_frame_alloc();
	if (!frame) {
		fprintf(stderr, "Could not allocate video frame\n");
		exit(1);
	}
	frame->format = c->pix_fmt;
	frame->width = c->width;
	frame->height = c->height;

	ret = av_frame_get_buffer(frame, 32);
	if (ret < 0) {
		fprintf(stderr, "Could not allocate the video frame data\n");
		exit(1);
	}

	this->rgb = new unsigned char[4 * width*height];
}

VideoOut::~VideoOut() {
	/* flush the encoder */
	encode(c, NULL, pkt, f);
	/* add sequence end code to have a real MPEG file */
	fwrite(endcode, 1, sizeof(endcode), f);
	fclose(f);
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	avcodec_free_context(&c);
	av_frame_free(&frame);
	av_packet_free(&pkt);
}


void VideoOut::writeFrame(unsigned char *pixels) {
	fflush(stdout);
	int ret;
	size_t cur_gl, cur_rgb;
	/* make sure the frame data is writable */
	ret = av_frame_make_writable(frame);
	if (ret < 0)
		exit(1);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			cur_gl = 4 * (width * (height - i - 1) + j);
			cur_rgb = 4 * (width * i + j);
			for (int p = 0; p < 4; p++) {
				rgb[cur_rgb + p] = (uint8_t)pixels[cur_gl + p];
			}
		}
	}

	ffmpeg_encoder_set_frame_yuv_from_rgb(rgb);
	frame->pts = k;
	/* encode the image */
	encode(c, frame, pkt, f);
	k++;
}

void VideoOut::encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt,
	FILE *outfile)
{
	int ret;

	/* send the frame to the encoder */
	if (frame)
		printf("Send frame %3\n", (long long)frame->pts);

	ret = avcodec_send_frame(enc_ctx, frame);
	if (ret < 0) {
		fprintf(stderr, "Error sending a frame for encoding\n");
		exit(1);
	}

	while (ret >= 0) {
		ret = avcodec_receive_packet(enc_ctx, pkt);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			return;
		else if (ret < 0) {
			fprintf(stderr, "Error during encoding\n");
			exit(1);
		}

		//printf("Write packet %3 (size=%5d)\n", (long long)pkt->pts, (long long)pkt->size);
		fwrite(pkt->data, 1, pkt->size, outfile);
		av_packet_unref(pkt);
	}
}

void VideoOut::ffmpeg_encoder_set_frame_yuv_from_rgb(uint8_t *rgb) {
	//const int in_linesize[1] = { 4 * c->width };
	const int in_linesize[1] = { 4 * c->width };

	sws_context = sws_getCachedContext(sws_context,
		c->width, c->height, AV_PIX_FMT_RGB32,
		c->width, c->height, AV_PIX_FMT_YUV420P,
		SWS_FAST_BILINEAR, 0, 0, 0);

	sws_scale(sws_context, (const uint8_t * const *)&rgb, in_linesize, 0,
		c->height, frame->data, frame->linesize);

}