#include "pch.h"
#include "CMFCApplication1.h"
#include "MFCFFmpegjpeg.h"

#if 1
//#define inline _inline //이걸 제일 위에 써줘야 한다. 
 

extern "C" {

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/buffersrc.h"
#include "libavfilter/buffersink.h"
#include "libavutil/avutil.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"



int av_image_alloc (uint8_t *pointers[4], int linesizes[4], int w, int h, enum AVPixelFormat pix_fmt, int align) ;

#pragma comment( lib, "avcodec.lib")
#pragma comment( lib, "avformat.lib")
#pragma comment( lib, "avutil.lib")
#pragma comment( lib, "avfilter.lib")
#pragma comment( lib, "avdevice.lib")
#pragma comment( lib, "swresample.lib")
#pragma comment( lib, "swscale.lib")

}


#else
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/buffersrc.h"
#include "libavfilter/buffersink.h"
#include "libavutil/avutil.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"

#pragma comment( lib, "avcodec.lib")
#pragma comment( lib, "avformat.lib")
#pragma comment( lib, "avutil.lib")
#pragma comment( lib, "avfilter.lib")
#pragma comment( lib, "avdevice.lib")
#pragma comment( lib, "swresample.lib")
#pragma comment( lib, "swscale.lib")

extern "C" {
    int av_image_alloc (uint8_t *pointers[4], int linesizes[4], int w, int h, enum AVPixelFormat pix_fmt, int align) ;
}

#endif



// MFCFFmpegIf

MFCFFmpegjpeg::MFCFFmpegjpeg()
{
}

MFCFFmpegjpeg::~MFCFFmpegjpeg()
{
}

// MFCFFmpegIf 멤버 함수

int MFCFFmpegjpeg::Jpeg2H264(const char * filename1, const char * filename2, BOOL last)
{
    int ret;
    unsigned int i;


   
    av_register_all();


    ret = open_input_file(filename1) ;

    if(ret > 0) 
        return ret ;

    if (ofmt_ctx==NULL){
        ret = open_output_file(filename2)  ;
        if(ret > 0) {
            
            avcodec_free_context(&dec_ctx);
            avformat_close_input(&ifmt_ctx);
            return ret ;
        }
    }

    ret = decode_encode() ;
    if ((ret==0)&&(last==true))
        av_write_trailer(ofmt_ctx);
    else
        last = true ;

    avcodec_free_context(&dec_ctx);
    avformat_close_input(&ifmt_ctx);

    if (last==true){
        
        avcodec_free_context(&enc_ctx);
        if (ofmt_ctx && !(ofmt_ctx->oformat->flags & AVFMT_NOFILE))
            avio_closep(&ofmt_ctx->pb);
        avformat_free_context(ofmt_ctx);
    }

    return ret ;
}

int MFCFFmpegjpeg::open_input_file(const char *filename)
{   
    int i, ret ;
    
    ret = avformat_open_input(&ifmt_ctx, filename,NULL,NULL) ;
    if (ret < 0) 
        return 1 ;
    
    ret = avformat_find_stream_info(ifmt_ctx,NULL) ;
    if (ret < 0) 
        return 2 ;

    int VSI = av_find_best_stream(ifmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0); //비디오 스트림 정보를 가져온다.
    
    if (ifmt_ctx->nb_streams !=1)
        return 3 ;
  
    AVCodecParameters *params = ifmt_ctx->streams[0]->codecpar;

    if (params->codec_type != AVMEDIA_TYPE_VIDEO)
        return 4 ;

    AVCodec *codec = avcodec_find_decoder(params->codec_id) ;
     if (codec==NULL)
        return 5 ;
    dec_ctx = avcodec_alloc_context3(codec);
    if (dec_ctx==NULL)
        return 6 ;
    ret = avcodec_parameters_to_context(dec_ctx,params) ;
     if (ret < 0) 
        return 7 ;
    ret = avcodec_open2(dec_ctx,codec, NULL);
    if (ret < 0) 
        return 8 ;


    av_dump_format(ifmt_ctx, 0, filename, 0);
    return 0 ;

}

int MFCFFmpegjpeg::open_output_file(const char *filename)
{
    AVStream *out_stream;
    AVStream *in_stream;
    AVCodec *encoder;
    int ret;
    unsigned int i;
    ofmt_ctx = NULL;

    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, filename);
    if (!ofmt_ctx) 
        return 11;

    //encoder = avcodec_find_encoder(AV_CODEC_ID_H264);
    //encoder = avcodec_find_encoder(AV_CODEC_ID_MPEG4);
    encoder = avcodec_find_encoder(AV_CODEC_ID_MPEG2VIDEO);
    if (!encoder)
        return 12 ;


    /* The streams created by avformat_new_stream have their own internal codec contexts, 
       the one you created with avcodec_alloc_context3 is unnecessary 
       and has no effect on the workings of avformat_write_header.
    */
    out_stream = avformat_new_stream(ofmt_ctx, encoder);
    if (!out_stream) 
        return 13 ;

   

    enc_ctx = avcodec_alloc_context3(encoder);
    if (!enc_ctx) 
        return 14 ;

    if ((enc_ctx->codec_id != AV_CODEC_ID_MPEG2VIDEO)||
        (enc_ctx->codec != encoder)||
        (enc_ctx->codec_type != AVMEDIA_TYPE_VIDEO))
        return 15 ;

    AVDictionary    *opts ;
    av_dict_set(&opts, "b", "2.5M", 0);
    CString msgbox ;
           
           #if 0
            /* In this example, we transcode to same properties (picture size,
             * sample rate etc.). These properties can be changed for output
             * streams easily using filters */
            if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
                enc_ctx->height = dec_ctx->height;
                enc_ctx->width = dec_ctx->width;
                enc_ctx->sample_aspect_ratio = dec_ctx->sample_aspect_ratio;

        
                /* take first format from list of supported formats */
                if (encoder->pix_fmts)
                    enc_ctx->pix_fmt = encoder->pix_fmts[0];
                else
                    enc_ctx->pix_fmt = dec_ctx->pix_fmt;
                /* video time_base can be set to whatever is handy and supported by encoder */
                enc_ctx->time_base = av_inv_q(dec_ctx->framerate); // return 1, 0
                

                if (encoder->pix_fmts)
                    msgbox.Format(_T("pr=%d,pf=%d,tn=%d,td=%d,gs=%d,mb=%d \n")
                                  ,enc_ctx->profile,enc_ctx->pix_fmt
                                  ,enc_ctx->time_base.num,enc_ctx->time_base.den
                                  ,enc_ctx->gop_size,enc_ctx->max_b_frames) ;
                else
                    msgbox.Format(_T("pr=%d,PF=%d,tn=%d,td=%d,gs=%d,mb=%d \n")
                                  ,enc_ctx->profile,enc_ctx->pix_fmt
                                  ,enc_ctx->time_base.num,enc_ctx->time_base.den
                                  ,enc_ctx->gop_size,enc_ctx->max_b_frames) ;

                AfxMessageBox(msgbox) ;
            } 
            #else
            enc_ctx->height = dec_ctx->height;
            enc_ctx->width = dec_ctx->width;
            enc_ctx->sample_aspect_ratio = dec_ctx->sample_aspect_ratio;
            enc_ctx->framerate.num = 1;
            enc_ctx->framerate.den = 24 ;
            enc_ctx->time_base = av_inv_q(enc_ctx->framerate); // 24, 1
            enc_ctx->ticks_per_frame = 1 ;
            enc_ctx->gop_size = 12 ;         // has tw0 sequence 
            enc_ctx->max_b_frames = 1 ;
            enc_ctx->bit_rate = 200000 ;
            enc_ctx->profile = FF_PROFILE_MPEG2_MAIN;
            enc_ctx->pix_fmt = AV_PIX_FMT_YUV420P ;
            #endif

           
            
        
            if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
                enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

        
            /* Third parameter can be used to pass settings to encoder */
            ret = avcodec_open2(enc_ctx, encoder, NULL) ;
            if (ret < 0) // -22 means invalid data arguments , -1 means 
                return 100-ret; // to get 122
            /* KHR comment at avcodec_open2()
                . KILLED      : MPEG4 time_base.den !=0
                . Return -22  : MPEG4 
                . Return -1   : MPEG2
            */
            
    
            ret = avcodec_parameters_from_context(out_stream->codecpar, enc_ctx);
            if (ret < 0) 
                return 16 ;

            out_stream->time_base = enc_ctx->time_base;


        
    
    av_dump_format(ofmt_ctx, 0, filename, 1);
    if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, filename, AVIO_FLAG_WRITE);
        if (ret < 0) 
            return 17 ;
    }
        
    /* init muxer, write output file header */
    ret = avformat_write_header(ofmt_ctx, NULL);
    if (ret < 0) 
        return 18 ;
        
    return 0;
    
}


int MFCFFmpegjpeg::decode_encode(void)
{
    
    AVPacket packet ;
    AVFrame *frame = NULL;
    enum AVMediaType type;
    enum AVPixelFormat  format ;
    unsigned int stream_index;
    unsigned int i;
    unsigned int width, height ;
    int ret , got_frame = 0 ;

    packet.data = NULL ;
    packet.size = 0 ;
      
    /*
    av_read_frame() 
        0 if OK, < 0 on error or end of file 
    avcodec_send_packet()
        0 on success, otherwise negative error code: 
        AVERROR(EAGAIN): input is not accepted in the current state - user must read output with avcodec_receive_frame() (once all output is read, the packet should be resent, and the call will not fail with EAGAIN). 
        AVERROR_EOF: the decoder has been flushed, and no new packets can be sent to it (also returned if more than 1 flush packet is sent) 
        AVERROR(EINVAL): codec not opened, it is an encoder, or requires flush 
        AVERROR(ENOMEM): failed to add packet to internal queue, or similar other errors: legitimate decoding errors 
    
    av_codec_receive_frame()
       0: success, a frame was returned 
       AVERROR(EAGAIN): output is not available in this state - user must try to send new input 
       AVERROR_EOF: the decoder has been fully flushed, and there will be no more output frames 
       AVERROR(EINVAL): codec not opened, or it is an encoder other negative values: legitimate decoding errors 


    av_codec_send_frame
        0 on success, otherwise negative error code: 
        AVERROR(EAGAIN): input is not accepted in the current state - user must read output with avcodec_receive_packet() (once all output is read, the packet should be resent, and the call will not fail with EAGAIN). 
        AVERROR_EOF: the encoder has been flushed, and no new frames can be sent to it 
        AVERROR(EINVAL): codec not opened, refcounted_frames not set, it is a decoder, or requires flush 
        AVERROR(ENOMEM): failed to add packet to internal queue, or similar other errors: legitimate decoding errors 

    av_codec_receive_packet()
        0 on success, otherwise negative error code: 
        AVERROR(EAGAIN): output is not available in the current state - user must try to send input 
        AVERROR_EOF: the encoder has been fully flushed, and there will be no more output packets 
        AVERROR(EINVAL): codec not opened, or it is an encoder other errors: legitimate decoding errors 

    */

    av_init_packet(&packet) ;
    while (av_read_frame(ifmt_ctx, &packet) == 0) 
    {
        stream_index = packet.stream_index;
        type = ifmt_ctx->streams[packet.stream_index]->codecpar->codec_type;
        format = (AVPixelFormat) ifmt_ctx->streams[packet.stream_index]->codecpar->format ;
        width = ifmt_ctx->streams[packet.stream_index]->codecpar->width ;
        height = ifmt_ctx->streams[packet.stream_index]->codecpar->height ;

        if (type == AVMEDIA_TYPE_VIDEO) {

            #if 0
             av_packet_rescale_ts(&packet,
                ifmt_ctx->streams[stream_index]->time_base,
                ifmt_ctx->streams[stream_index]->codec->time_base);
            #endif

            ret = avcodec_send_packet(dec_ctx, &packet);  
            if (ret==0)
                ;
            else if ((ret == AVERROR(EAGAIN))|| (ret== AVERROR_EOF))
                continue ;
            else
                return 1 ;

            frame = av_frame_alloc();
            if(frame==NULL)
                return 2 ;

            av_image_alloc(frame->data, frame->linesize, width, height, format, 1) ;
           

            while(1) {
                ret = avcodec_receive_frame(dec_ctx, frame);
                if (ret==0){
                    got_frame++ ;
                    continue ;
                }
                else if ((ret == AVERROR(EAGAIN))|| (ret== AVERROR_EOF))
                    break;
                else{
                    av_frame_unref(frame);
                    return 3 ;
                }
            }

            while (got_frame){

                ret = avcodec_send_frame(enc_ctx, frame);
                if (ret==0) 
                    ;
                else if ((ret == AVERROR(EAGAIN))|| (ret== AVERROR_EOF))
                    break ;
                else{
                    av_frame_unref(frame);
                    return 4 ;
                }
                while(1) {
                      
                    ret = avcodec_receive_packet(enc_ctx, &packet);
                    if (ret==0){
                         /* prepare packet for muxing */
                
                        av_packet_rescale_ts(&packet,enc_ctx->time_base,
                                                ofmt_ctx->streams[stream_index]->time_base);

                        av_interleaved_write_frame(ofmt_ctx, &packet);
                        continue ;
                    }
                    else if ((ret == AVERROR(EAGAIN))|| (ret== AVERROR_EOF))
                        break;
                    else{
                        av_frame_unref(frame);
                        return 5 ;
                    }
                }
            }
            av_frame_unref(frame);
           
        }
 
    }
 
    av_packet_unref(&packet);
    av_frame_unref(frame);
    return 0 ;
      

}

int MFCFFmpegjpeg::swscale(void)
{
    #if 0
    SwsContext swcCtx ;

   
    swsCtx = sws_getContext(
                            vFrame.width, vFrame.height, AVPixelFormat(vFrame.format),
                            vFrame.width, vFrame.height, AV_PIX_FMT_RGB24,
                            SWS_BICUBIC, NULL, NULL, NULL) ;
 
    // 변환을 수행한다.
    sws_scale(swsCtx, vFrame.data, vFrame.linesize, 0, vFrame.height,
                            RGBFrame.data, RGBFrame.linesize);
    #endif

    return 0 ;

}



