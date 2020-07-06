#pragma once


#if 1
#define inline _inline //이걸 제일 위에 써줘야 한다. 
 
extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}
#else
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#endif

// MFCFFmpegIf 명령 대상
#if 0
typedef struct FilteringContext {

    AVFilterContext *buffersink_ctx;

    AVFilterContext *buffersrc_ctx;

    AVFilterGraph *filter_graph;

} FilteringContext;

#endif



typedef struct StreamContext {
    AVCodecContext *dec_ctx;
    AVCodecContext *enc_ctx;

} StreamContext;


class MFCFFmpegjpeg 
{
public:
	MFCFFmpegjpeg();
	virtual ~MFCFFmpegjpeg();

private:
	AVFormatContext *ifmt_ctx ;
	AVFormatContext *ofmt_ctx  ;
	//FilteringContext *filter_ctx;
	//StreamContext *stream_ctx;
	AVCodecContext *dec_ctx;
	AVCodecContext *enc_ctx;
	int open_input_file(const char *filename) ;
	int open_output_file(const char *filename) ;
	int decode_encode(void) ;
	int swscale(void) ;

public:
	int Jpeg2H264(const char * filename1, const char * filename2, BOOL last) ;
	
};
