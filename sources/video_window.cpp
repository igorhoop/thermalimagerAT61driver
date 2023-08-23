#include <SFML/Graphics.hpp>
#include <unistd.h>
#include <iostream>
#include <vector>

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>
}



uint8_t *dst_data[4];
int dst_linesize[4];


// мои объекты ffmpeg
AVPacket * pPacket;
AVFrame *pFrame = NULL;

AVPacket * pSparePacket;

struct SwsContext *pResizeContext;
AVFrame *pFrame2 = NULL; // сюда будем класть данные после scale



AVCodecContext *pCodecContext = NULL;       // указатель на контекст кодека (основная структура кодека)
//AVCodecParameters *pCodecParameters = NULL; // указатель на объект, описывающий свойства закодированного потока


// ПОДГОТОВКА ДЕКОДЕРА ДЛЯ H264, ВСЕ ЧТО ОТНОСИТСЯ К FFMPEG
int VideoDecoderInit()
{
    avcodec_register_all(); // Регистрируем все существующие кодеки. Этой функции нет в документации, но без нее кодек не ищется

    AVCodec *pAVCodec = NULL; // указатель на объект-кодек. Он нужен только чтобы заполнить соответствующий член структуры контекста кодека.
                              // Поэтому объявлен локально в функции, а не глобально
    // определяемся с кодеком (в данном случае ищем декодер на h264). Получаем объект-кодек
    pAVCodec = avcodec_find_decoder_by_name("h264");
    if(pAVCodec == NULL)
    {
        std::cout << "Кодек не найден" << std::endl;
        exit(1);
    }    

    // заполняем КОНТЕКСТ КОДЕКА основываясь на объекте-кодеке
    pCodecContext = avcodec_alloc_context3(pAVCodec); // Это глобальный объект, который потребуется для декодирования.
    if(pCodecContext == NULL)
    {
        std::cout << "Не получилось выделить место под контекст кодека" << std::endl;
        exit(1);
    }

    std::cout << "ЧИТАЕМ ИНФУ ИЗ КОНТЕКСТА КОДЕКА: " << pCodecContext->codec->name << std::endl;
    std::cout << "размер: " << sizeof(AVCodec) << std::endl;
    

    // ВАЖНЫЙ ШАГ. Открываем интерфейс для взаимодействия с кодеком (взаимодействие будет через экземпляр контекста кодека)
    avcodec_open2(pCodecContext, pAVCodec, NULL);
        

    // зачем то нужно произвести инициализацию пакета. Этой функции тоже нет в документации. СЕГФОЛТ, сначала нужно alloc
    //av_init_packet(&pPacket); // она же устарела

    pPacket = av_packet_alloc();
    pSparePacket = av_packet_alloc();

    pFrame = av_frame_alloc();


    //std::cout << "из пакета: " << pPacket->size << std::endl;

    //exit(1);


    int src_w = 640, src_h = 512; // исходные размеры кадра
    int dst_w = 640, dst_h = 512; // целевые размеры кадра

    enum AVPixelFormat src_pix_fmt = AV_PIX_FMT_YUV420P; // тот самый формат который я понял
    enum AVPixelFormat dst_pix_fmt = AV_PIX_FMT_RGB24;   // с этим форматом пока неясно
    
    pResizeContext = sws_getContext(src_w, src_h, src_pix_fmt, dst_w, dst_h, dst_pix_fmt, SWS_BILINEAR, NULL, NULL, NULL);



    if ((av_image_alloc(dst_data, dst_linesize, dst_w, dst_h, dst_pix_fmt, 1)) < 0)
    {
            fprintf(stderr, "Could not allocate destination image\n");
            exit(1);
    }


    return 0;
}


AVFormatContext *output_format_context = NULL;   // контекст контейнера ВЫХОДНОГО

int SAVEFILE_FLAG = 0;

int RecordInit()
{
    std::cout << "Для инициализации FFMPEG" << std::endl;
    int res; // вспомогательная

    const char * sample_name = "molodym_sample.mp4"; // сэмпл чтобы взять инфу о кодеке
    const char * out_filename = "teplo_video.mp4";      // имя выходного файла
    
    AVFormatContext *pInputFormatContext;            // контекст контейнера ВХОДНОГО
    pInputFormatContext = avformat_alloc_context();  // выделить память под контекст контейнера

    // STEP 1    

    res = avformat_open_input(&pInputFormatContext, sample_name, NULL, NULL);
     if(res!=0)
    {
        std::cout << "Файл сэмпла не найден" << std::endl;
        return 0;
    }
    std::cout << "ТОЛЬКО ОТКРЫЛИ ВХОДНОЙ СЭМПЛ ФАЙЛ, ПОСМОТРИМ ЧТО СЕЙЧАС ИМЕЕМ В КОНТЕКСТЕ:" << std::endl;
    av_dump_format(pInputFormatContext, 0, sample_name, 0);

    //exit(1);

    
    avformat_alloc_output_context2(&output_format_context, NULL, NULL, out_filename);

    // ищем видеопоток, чтобы узнать его параметры кодека
    int i = 0;
    for(i = 0; i < pInputFormatContext->nb_streams; i++)
    {
        AVStream *in_stream = pInputFormatContext->streams[i];
        AVCodecParameters * in_codecpar = in_stream->codecpar;
        if(in_codecpar->codec_type != AVMEDIA_TYPE_VIDEO)
        {
            continue;
        }

        if(in_codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            break;
        }

    }

    std::cout << "Индекс видеопотока: " << i << std::endl;

 
    AVStream *out_stream;
    out_stream = avformat_new_stream(output_format_context, NULL);

   
    std::cout << "Копируем данные видеокодека из сэмпл файла в наш новый поток в новом формат-контесте." << std::endl;
    
    /*
    res = avcodec_parameters_copy(out_stream->codecpar, pInputFormatContext->streams[i]->codecpar);
    if (res < 0)
    {
        fprintf(stderr, "Failed to copy codec parameters\n");
        exit(1);
    }
    */
    

    std::cout << "Проверяем заполненность нового формат-контекста" << std::endl;

     

    // небольшие корректировочки
    out_stream->codecpar->width = 640;
    out_stream->codecpar->height = 512;


    std::cout << "codec_id: " << out_stream->codecpar->codec_id << std::endl;
    //std::cout << "codec_type: " << out_stream->codecpar->codec_type << std::endl;
    std::cout << "codec_tag: " << out_stream->codecpar->codec_tag << std::endl;
    std::cout << "height: " << out_stream->codecpar->height << std::endl;
    std::cout << "width: " << out_stream->codecpar->width << std::endl;
    std::cout << "format: " << out_stream->codecpar->format << std::endl;

    out_stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    out_stream->codecpar->codec_id = AV_CODEC_ID_H264;
    //std::cout << "codec_type: " << out_stream->codecpar->codec_type << std::endl;
    out_stream->codecpar->codec_tag = 828601953;
    out_stream->codecpar->width = 640;
    out_stream->codecpar->height = 512;
    out_stream->codecpar->format = -1;



    //av_dump_format(output_format_context, 0, out_filename, 1);


    if (!(output_format_context->oformat->flags & AVFMT_NOFILE))
        {
            res = avio_open(&output_format_context->pb, out_filename, AVIO_FLAG_WRITE);
            if (res < 0) {
            fprintf(stderr, "Could not open output file '%s'", out_filename);
            exit(1);
            }
        }



    AVDictionary* opts = NULL;

    

    res = avformat_write_header(output_format_context, &opts);
    if (res < 0)
    {
        fprintf(stderr, "Error occurred when opening output file\n");
        exit(1);
    }


    SAVEFILE_FLAG = 1;
    //exit(1);

}



int global_pts = 1;
int global_dts = 0;
int record_counter = 0;

extern long GlobalRecieveByteValue;
    
int DecodeH264(uint8_t *inbuf, int inbufSize)
{
    if(pCodecContext == NULL || pFrame == NULL || inbufSize <= 0)
    {
        std::cout << "Не получилось декодировать, т.к. отсутствуют нужные объекты" << std::endl;
        return 1;
    }

    
    pPacket->data = inbuf;
    pPacket->size = inbufSize;

    if(pPacket->buf == NULL)
        std::cout << "NUUUUUUUUUUUUUUUUL" << std::endl;

    av_packet_ref(pSparePacket, pPacket);
    
    


    if(SAVEFILE_FLAG == 1)
    {     
        global_pts += 4300;
        global_dts += 4300;

        pPacket->pts = global_pts;
        pPacket->dts = global_dts;
        pPacket->duration = 1000;
        //pPacket->pos = -1;
        pPacket->stream_index = 0;

        int res = av_interleaved_write_frame(output_format_context, pPacket);
        if (res < 0)
        {
            fprintf(stderr, "Error muxing packettzzz\n");
            exit(1);
        }   
           
    }

    av_packet_unref(pPacket);
  
    
    


    int result = avcodec_send_packet(pCodecContext, pSparePacket);
    
    std::cout << "что то из запасного пакета: " << pSparePacket->size << std::endl;
    std::cout << "результат отправки пакета: " << result << std::endl;

    if(result == 0)
    {
        result = avcodec_receive_frame(pCodecContext, pFrame);
        std::cout << "расшифровываем кадр: " << std::endl;
        std::cout << "его ширина: " << pFrame->linesize[0] << std::endl;

        int height_output = sws_scale(pResizeContext, (const uint8_t * const*) pFrame->data, pFrame->linesize, 0, 512, dst_data, dst_linesize);

        std::cout << "высота output: " << height_output << std::endl;
    }

    
    
        
    return 0;
}




// ФУНКЦИЯ ОТРИСОВКИ БУФЕРОВ С КАДРОМ 512x640
int DrawVideoFrame(sf::VertexArray &VideoFrame, uint8_t *buff_component_1, uint8_t *buff_component_2, uint8_t *buff_component_3, int lensize)
{
    sf::Color CurrentColor(0,0,0);

    if(lensize != 640)
    {
        
        printf("метка. Колво: %d\n", lensize); 
        return 1;  
    }
    
    
    //printf("Колво: %d\n", lensize);

    int x=0, y=0;
    for(int i = 0, j = 0; i < 2073600 && j < 327680; i+=3, j++)
    {
        CurrentColor.r = buff_component_1[i];
        CurrentColor.g = buff_component_1[i+1];
        CurrentColor.b = buff_component_1[i+2];

        VideoFrame[j].color = CurrentColor;
        VideoFrame[j].position = sf::Vector2f(x,y);
        x++;
        if(x > 639)
        {
            x=0;
            y++;
        }
    }
}
 




// ПОТОЧНАЯ ФУНКЦИЯ, РИСУЮЩАЯ ОКНО И ОТОБРАЖАЮЩАЯ ВИДЕО
void * WindowVideoThread(void * args)
{
    sf::RenderWindow Stream_Window;     // объект окна
    int Win_Width = 640;                // ширина окна
    int Win_Height = 512;               // высота окна

    sf::VertexArray ThermalFrame(sf::Points, Win_Width * Win_Height); // массив вертексов
    
    Stream_Window.create(sf::VideoMode(Win_Width, Win_Height), "3Logic Thermal Stream");  // создание окна требуемого размера и с указанным заголовком
    Stream_Window.setPosition(sf::Vector2i(1250,30));                                     // положение окна


    


    VideoDecoderInit();
        
    while(Stream_Window.isOpen())
    {
        sf::Event event;

        while(Stream_Window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                Stream_Window.close();
            
            if(event.type == sf::Event::KeyPressed)
            {
                // событие нажатие на клавишу
                std::cout << "Нажата: " << event.key.code << std::endl;
                
            if(event.key.code == 18)
            {
                std::cout << "ВКЛЮЧАЕМ ЗАПИСЬ" << std::endl;
                std::cout << "Кол-во байт перед записью: " << GlobalRecieveByteValue << std::endl;
                RecordInit();
            }



            if(event.key.code == 10)
            {
                std::cout << "ОСТАНОВКА ЗАПИСИ" << std::endl;
                SAVEFILE_FLAG=0;
                std::cout << "Записываем трейлер" << std::endl;
                std::cout << "Кол-во байт после записи: " << GlobalRecieveByteValue << std::endl;
                av_write_trailer(output_format_context);
                avio_closep(&output_format_context->pb);
                
            }
                //av_packet_free(pPacket);
                //av_packet_unref(&pPacket);

                //av_frame_free(&pFrame);
            }
        }


        if(pFrame != NULL)
        {
            DrawVideoFrame(ThermalFrame, dst_data[0], pFrame->data[1], pFrame->data[2], dst_linesize[0]/3);


        }

        Stream_Window.clear(sf::Color::Black); // отрисовка в скрытый буфер
        Stream_Window.draw(ThermalFrame);
        Stream_Window.display();
    }
}







