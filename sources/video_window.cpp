#include <SFML/Graphics.hpp> // SFML
#include <unistd.h>
#include <iostream>
#include <vector>

#include <mutex>

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}




struct AVCodecContext *pAVCodecCtx_decoder = NULL;
struct AVPacket mAVPacket_decoder;
struct AVFrame *pAVFrame_decoder = NULL;
struct SwsContext* pImageConvertCtx_decoder = NULL;
struct AVFrame *pFrameYUV_decoder = NULL;


// мои объекты ffmpeg
AVCodecContext *pCodecContext = NULL;
AVPacket pPacket;
AVFrame *pFrame = NULL;

AVCodec *pAVCodec = NULL;
AVCodecParameters *pCodecParameters = NULL;


// инициализация всего что нужно для декодирования
int DecoderInit()
{
    avcodec_register_all(); // этой функции нет в документации, но без нее кодек не ищется

    // сначала определяемся с кодеком (в данном случае ищем декодер на h264)
    pAVCodec = avcodec_find_decoder_by_name("h264");
    if(!pAVCodec)
    {
        std::cout << "кодек не найден" << std::endl;
        exit(1);
    }


    // потом создаем контекст кодека и заполняем его на основе структуры AVCodec
    pCodecContext = avcodec_alloc_context3(pAVCodec); // это основная глобальная структура для работы кодека. выделяем место и заполняем поля
    if(!pCodecContext)
    {
        std::cout << "не получилось выделить место под контекст кодека" << std::endl;
        exit(1);
    }


    // зачем то создаем структуру ACCodecParameters и заполняем ее на базе контекста кодека
    pCodecParameters = avcodec_parameters_alloc();
    int result = avcodec_parameters_from_context(pCodecParameters, pCodecContext);
    if(result < 0)
    {
        std::cout << "не получилось заполнить структуру AVCodecParameters на базе контекста кодека" << std::endl;
        exit(1);
    }

    // зачем то копируем параметры кодека в контекст кодека
    result = avcodec_parameters_to_context(pCodecContext, pCodecParameters);
    if(result < 0)
    {
        std::cout << "не получилось скопировать параметры кодека в контекст кодека" << std::endl;
        exit(1);
    }


    // открыть кодек
    avcodec_open2(pCodecContext, pAVCodec, NULL);

    // зачем то нужно произвести инициализацию пакета. Этой функции тоже нет в документации. СЕГФОЛТ, сначала нужно alloc
    av_init_packet(&pPacket); 

    pFrame = av_frame_alloc();


    std::cout << "УСПЕШНАЯ ИНИЦИАЛИЗАЦИЯ" << std::endl;
    return 0;
}





int DrawVideoFrame(sf::VertexArray &VideoFrame, uint8_t *buff_component_1, uint8_t *buff_component_2, uint8_t *buff_component_3, int lensize)
{
    sf::Color CurrentColor(0,0,0);

    if(lensize != 640)
    {
        
        printf("метка. Колво: %d\n", lensize); 
        return 1;  
    }
    
    
    printf("Колво: %d\n", lensize);
    //printf("Данные: %d\n", buf[50034]);

    int x=0, y=0;
    for(int i = 0; i < 327680; i++)
    {
        CurrentColor.r = buff_component_1[i];
        VideoFrame[i].color = CurrentColor;
        
        VideoFrame[i].position = sf::Vector2f(x,y);
        x++;
        if(x > 639)
        {
            x=0;
            y++;
        }
    }
}


uint8_t YUVOUT_Y[328000];
uint8_t YUVOUT_U[328000];
uint8_t YUVOUT_V[328000];


    
int DecodeH264(uint8_t *inbuf, int inbufSize)
{
    //std::cout << "Начало функции декодирования" << std::endl;
    if(pCodecContext == NULL || pFrame == NULL || inbufSize <= 0)
    {
        std::cout << "Не получилось декодировать, т.к. отсутствуют нужные объекты" << std::endl;
        return 1;
    }
    
    pPacket.data = inbuf;
    pPacket.size = inbufSize;

    int result = avcodec_send_packet(pCodecContext, &pPacket);
    std::cout << "результат отправки пакета: " << result << std::endl;

    if(result == 0)
    {
        result = avcodec_receive_frame(pCodecContext, pFrame);
        std::cout << "расшифровываем кадр: " << std::endl;
        std::cout << "его ширина: " << pFrame->linesize[0] << std::endl;
        //uint8_t * Y_buffer = pFrame->data[0];

        //memcpy(YUVOUT_Y, pFrame->data[0], pFrame->linesize[0]);

        


    }
        
    return 0;
}

extern uint8_t * videoData;;
extern int flag_live_buffer;
extern int videoDataSize;



// ПОТОК ОКНА
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
                std::cout << "флаг существования буфера: " << flag_live_buffer << std::endl;
                
                DecoderInit();
                

                //av_packet_free(pPacket);
                //av_packet_unref(&pPacket);

                //av_frame_free(&pFrame);

                
            }
        }


        if(pFrame != NULL)
        {
            //struct SwsContent *resize;
            //resize = sws_getContext(width1, height1, AV_PIX_FMT_YUV420P, width2, height2, PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
            DrawVideoFrame(ThermalMap, pFrame->data[0], pFrame->data[1], pFrame->data[2], pFrame->linesize[0]);
        }



        view_window.clear(sf::Color::Black); // отрисовка в скрытый буфер
        view_window.draw(ThermalMap);
        view_window.display();
    }
}







