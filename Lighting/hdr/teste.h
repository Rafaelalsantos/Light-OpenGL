extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>
#include <libavdevice/avdevice.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
#include <sys/time.h>
}

#include <glad/glad.h>
#include <KHR/khrplatform.h>
// app data structure
typedef struct {
	AVFormatContext *fmt_ctx;
	int stream_idx;
	AVStream *video_stream;
	AVCodecContext *codec_ctx;
	AVCodec *decoder;
	AVPacket *packet;
	AVFrame *av_frame;
	AVFrame *gl_frame;
	struct SwsContext *conv_ctx;
	GLuint vao;
	GLuint vert_buf;
	GLuint elem_buf;
	GLuint frame_tex;
	GLuint program;
	GLuint attribs[2];
	GLuint uniforms[2];
} AppData;

// initialize the app data structure
void initializeAppData(AppData *data) {
	data->fmt_ctx = NULL;
	data->stream_idx = -1;
	data->video_stream = NULL;
	data->codec_ctx = NULL;
	data->decoder = NULL;
	data->av_frame = NULL;
	data->gl_frame = NULL;
	data->conv_ctx = NULL;
}

// clean up the app data structure
void clearAppData(AppData *data) {
	if (data->av_frame) av_free(data->av_frame);
	if (data->gl_frame) av_free(data->gl_frame);
	if (data->packet) av_free(data->packet);
	if (data->codec_ctx) avcodec_close(data->codec_ctx);
	if (data->fmt_ctx) avformat_free_context(data->fmt_ctx);
	glDeleteVertexArrays(1, &data->vao);
	glDeleteBuffers(1, &data->vert_buf);
	glDeleteBuffers(1, &data->elem_buf);
	glDeleteTextures(1, &data->frame_tex);
	initializeAppData(data);
}