#include <SFML/Graphics.hpp> // SFML
#include <unistd.h>
#include <iostream>
#include <vector>

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}

int FFmpeg_VideoDecoderInit(AVCodecParameters *ctx);
int FFmpeg_H264DecoderInit(void);
int FFmpeg_VideoDecoderRelease(void);
int FFmpeg_H264Decode(unsigned char * inbuf, int inbufSize, unsigned char *outRGBBuf);

    
struct AVCodecContext *pAVCodecCtx_decoder = NULL;
struct AVCodec *pAVCodec_decoder;
struct AVPacket mAVPacket_decoder;
struct AVFrame *pAVFrame_decoder = NULL;
struct SwsContext* pImageConvertCtx_decoder = NULL;
struct AVFrame *pFrameYUV_decoder = NULL;


int FFmpeg_VideoDecoderInit(AVCodecParameters *codecParameters)
{
    if (!codecParameters) {
                    printf("Source codec context is NULL."); //CPrintf needs to be replaced by printf
        return -1;
    }
    FFmpeg_VideoDecoderRelease();
    avcodec_register_all();
    
    pAVCodec_decoder = avcodec_find_decoder(codecParameters->codec_id);
    if (!pAVCodec_decoder) {
        printf("Can not find codec:%d\n", codecParameters->codec_id);
        return -2;
    }
    
    pAVCodecCtx_decoder = avcodec_alloc_context3(pAVCodec_decoder);
    if (!pAVCodecCtx_decoder) {
        printf("Failed to alloc codec context.");
        FFmpeg_VideoDecoderRelease();
        return -3;
    }
    
    if (avcodec_parameters_to_context(pAVCodecCtx_decoder, codecParameters) < 0) {
        printf("Failed to copy avcodec parameters to codec context.");
        FFmpeg_VideoDecoderRelease();
        return -3;
    }
    
    
    if (avcodec_open2(pAVCodecCtx_decoder, pAVCodec_decoder, NULL) < 0){
        printf("Failed to open h264 decoder");
        FFmpeg_VideoDecoderRelease();
        return -4;
    }
    
    av_init_packet(&mAVPacket_decoder);
    
    pAVFrame_decoder = av_frame_alloc();
    pFrameYUV_decoder = av_frame_alloc();
    
    return 0;
}
    
int FFmpeg_H264DecoderInit()
{
    avcodec_register_all();
    //AVCodec *pAVCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
    AVCodec *pAVCodec = avcodec_find_decoder_by_name("h264");

    if (!pAVCodec){
        printf("can not find H264 codec\n");
        return -1;
    }
    
    AVCodecContext *pAVCodecCtx = avcodec_alloc_context3(pAVCodec);
    if (pAVCodecCtx == NULL) {
        printf("Could not alloc video context!\n");
        return -2;
    }
    
    AVCodecParameters *codecParameters = avcodec_parameters_alloc();
    if (avcodec_parameters_from_context(codecParameters, pAVCodecCtx) < 0) {
        printf("Failed to copy avcodec parameters from codec context.");
        avcodec_parameters_free(&codecParameters);
        avcodec_free_context(&pAVCodecCtx);
        return -3;
    }
    
    int ret = FFmpeg_VideoDecoderInit(codecParameters);
    avcodec_parameters_free(&codecParameters);
    avcodec_free_context(&pAVCodecCtx);
    
    return ret;
}
    
int FFmpeg_VideoDecoderRelease() {
    if (pAVCodecCtx_decoder != NULL) {
        avcodec_free_context(&pAVCodecCtx_decoder);
        pAVCodecCtx_decoder = NULL;
    }
    
    if (pAVFrame_decoder != NULL) {
        av_packet_unref(&mAVPacket_decoder);
        av_free(pAVFrame_decoder);
        pAVFrame_decoder = NULL;
    }
    
    if (pFrameYUV_decoder) {
        av_frame_unref(pFrameYUV_decoder);
        av_free(pFrameYUV_decoder);
        pFrameYUV_decoder = NULL;
    }
    
    if (pImageConvertCtx_decoder) {
        sws_freeContext(pImageConvertCtx_decoder);
    }
    
    av_packet_unref(&mAVPacket_decoder);
    
    return 0;
}
    
int FFmpeg_H264Decode(unsigned char *inbuf, int inbufSize, unsigned char *outRGBBuf)
{
    if (!pAVCodecCtx_decoder || !pAVFrame_decoder || !inbuf || inbufSize<=0)
    {
        std::cout << "metka" << std::endl;
        return -1;    
    }

    int framePara[5]= {0,0,0,0,0};
    av_frame_unref(pAVFrame_decoder);
    av_frame_unref(pFrameYUV_decoder);
    
    framePara[0] = framePara[1] = 0;
    mAVPacket_decoder.data = inbuf;
    mAVPacket_decoder.size = inbufSize;
    
    int ret = avcodec_send_packet(pAVCodecCtx_decoder, &mAVPacket_decoder);
    if (ret == 0)
    {
        std::cout << "УСПЕШНО ОТПРАВЛЕНЫ ДАННЫЕ ДЕКОДЕРУ" << std::endl;
        /*ret = avcodec_receive_frame(pAVCodecCtx_decoder, pAVFrame_decoder);
        if (ret == 0) 
        {
            framePara[0] = pAVFrame_decoder->width;
            framePara[1] = pAVFrame_decoder->height;

            if (outRGBBuf)
            {
                pFrameYUV_decoder->data[0] = outRGBBuf;
                pFrameYUV_decoder->data[1] = NULL;
                pFrameYUV_decoder->data[2] = NULL;
                pFrameYUV_decoder->data[3] = NULL;
                int linesize[4] = { pAVCodecCtx_decoder->width * 3, pAVCodecCtx_decoder->height * 3, 0, 0 };
                pImageConvertCtx_decoder = sws_getContext(pAVCodecCtx_decoder->width, pAVCodecCtx_decoder->height, AV_PIX_FMT_YUV420P, pAVCodecCtx_decoder->width, pAVCodecCtx_decoder->height, AV_PIX_FMT_RGB24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
                sws_scale(pImageConvertCtx_decoder, (const uint8_t* const *) pAVFrame_decoder->data, pAVFrame_decoder->linesize, 0, pAVCodecCtx_decoder->height, pFrameYUV_decoder->data, linesize);
                sws_freeContext(pImageConvertCtx_decoder);
                
                return 1;
            }
        }
        else if (ret == AVERROR(EAGAIN))
        {
            return 0;
        }
        else
        {    
            return -1;
        }*/
    }
    else
    {
        std::cout << "НЕУСПЕШНО ДАННЫЕ ДЕКОДЕРУ" << std::endl;
        std::cout << "ret= " << ret << std::endl;
    }
    
    return 0;
}







void DrawVideoFrame(unsigned char * h264_input_buff, long len)
{
    unsigned char * outbuff = new unsigned char (80000);
    
    int frameSize = FFmpeg_H264Decode(h264_input_buff, len, outbuff);
    std::cout << "FrameSize: " << frameSize << std::endl;
    
    delete outbuff;

}

extern unsigned char* video_data;


void * WindowVideoThread(void * args)
{
    sf::RenderWindow view_window;
    view_window.create(sf::VideoMode(580, 600), "VIDEO Window");

    sf::VertexArray ThermalMap(sf::Points, 327680);

    

    while(view_window.isOpen())
    {
        sf::Event event;

        while(view_window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                view_window.close();
            
            if(event.type == sf::Event::KeyPressed)
            {
                std::cout << "Нажата: " << event.key.code << std::endl;
               
                int z = FFmpeg_H264DecoderInit();
                std::cout << "FFmpeg Init: " << z << std::endl;

                view_window.clear(sf::Color::Black); // отрисовка в скрытый буфер
                view_window.draw(ThermalMap);
                view_window.display();
            }
        }
        //sleep(0.5);
    }
}


