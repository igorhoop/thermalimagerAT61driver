#include <SFML/Graphics.hpp>
#include <unistd.h>
#include <iostream>
#include <vector>

#include "../headers/3l_functions.h"

extern struct PROGRAM_CONFIG CONFIG;



extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/time.h>
}



// === Все что касается RTSP ===
AVPacket * pPack_RTSP = NULL;
AVFormatContext * pOutputContext_RTSP = NULL;
int RTSP_FLAG=0;    // флаг старта стрима


// для чего эти две переменные???
uint8_t * dst_data[4];
int dst_linesize[4];


int Flag_I_Frame = 0;

// мои объекты ffmpeg
AVPacket * pPacket;    // пакет используемый для отрисовки и записи
AVFrame *pFrame = NULL;


int64_t rtsp_start_time = 0; // Время старта стриминга
int64_t record_start_time = 0; // Время старта проигрывания на экране

AVPacket * pSparePacket;

struct SwsContext *pResizeContext;


AVFormatContext * output_format_context = NULL;   // контекст контейнера ВЫХОДНОГО ФАЙЛОВОГО

int RECORD_FLAG = 0;



AVCodecContext *pCodecContext = NULL;       // указатель на контекст кодека (основная структура кодека)



// ПОДГОТОВКА ДЕКОДЕРА ДЛЯ H264 И ВСЕГО ЧТО НЕОБХОДИМО ДЛЯ ЕГО ИСПОЛЬЗОВАНИЯ
int H264_DecoderInitialization()
{
    /* На выходе должны получить: 
        1) AVCodecContext кодека
        2) Открытие интерфейса взаимодействия с кодеком через созданный кодек-контекст
        3) Создание ресайз-контекста для масштабирования, но для чего масштабировать непомню

    */

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






int RTSP_Init() // функция инициализации (или сразу начала уже) трансляции по протоколу RTSP
{
    /*
    Что хотим на выходе:
        1) Выходной формат-контекст для RTSP с добавленным видеопотоком
        2) Заполненная AVCodecParameters для добавленного видеопотока
        3) Открытый ввод/вывод для взаимодействия с URL, связанным с этим формат-контекстом
    
    */

    int res = 0;


    avformat_alloc_output_context2(&pOutputContext_RTSP, NULL, "rtsp", CONFIG.AT61F_RTSP_URL.c_str()); // ВЫДЕЛЕНИЕ ПАМЯТИ
    
    // создаем новый поток в AVFormatContext
    avformat_new_stream(pOutputContext_RTSP, NULL);

    //pOutputContext_RTSP->streams[0]->time_base.den = 30000;
    std::cout << "TIMEBASE нового потока в RTSP: " << pOutputContext_RTSP->streams[0]->time_base.den << std::endl;


    // ==== для нового потока заполняем AVCodecParameters ====

    //avcodec_parameters_copy(pOutputContext_RTSP->streams[0]->codecpar, pInputContext->streams[1]->codecpar);
    //std::cout << "Метка 222: " << pOutputContext->streams[0]->codecpar->codec_id << std::endl;
    //av_dump_format(pOutputContext, 0, pOutputURL, 1); // Должен делать дамп только по определенному URL, но делает полностью. Шляпа какая то

    pOutputContext_RTSP->streams[0]->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    pOutputContext_RTSP->streams[0]->codecpar->codec_id = AV_CODEC_ID_H264;
    
    pOutputContext_RTSP->streams[0]->codecpar->codec_tag = 828601953;
    pOutputContext_RTSP->streams[0]->codecpar->width = 640;
    pOutputContext_RTSP->streams[0]->codecpar->height = 512;
    pOutputContext_RTSP->streams[0]->codecpar->format = -1;


    std::cout << "Сейчас будет коннект к RTSP" << std::endl;

    // ПОХОДУ ДЛЯ RTSP ОТКРЫВАТЬ IO НЕ НУЖНО

    /*if (!(pOutputContext_RTSP->oformat->flags & AVFMT_NOFILE))
    {
        //res = avio_open(&pOutputContext_RTSP->pb, pOutputURL, AVIO_FLAG_READ_WRITE);
        std::cout << "res=" << res << std::endl;
        if (res < 0)
        {
            std::cout << "Что-то пошло не так с RTSP, походу не получилось открыть IO для выходного URL" << std::endl;
            return 1;
        }

    }
    */

 
    return 0;
    

}

int RTSP_Start()
{
    int res = 0; 

    if(RTSP_FLAG==1)
    {
        std::cout << "Стрим уже идет" << std::endl;
    }
    else
    {
        RTSP_Init();
             
        // Пишем хедер в выход (заголовок всего 48 байт?)
        res = avformat_write_header(pOutputContext_RTSP, NULL);
        if(res < 0)
        {
            std::cout << "Инициализация RTSP не получилась" << std::endl;
            RTSP_FLAG=0;
        }
        else
        {
            RTSP_FLAG=1;
            rtsp_start_time=av_gettime();
        }
    }

    return 0;

}


int RTSP_Stop()
{
    if(RTSP_FLAG==0)
    {
        std::cout << "Стрим итак не идет, тормозить нечего" << std::endl;
    }
    else
    {
        av_write_trailer(pOutputContext_RTSP); // запись трейлера
        avformat_free_context(pOutputContext_RTSP);
 
        RTSP_FLAG=0;
    }
}


int RecordInit(const char * filename)
{
    std::cout << "Подготовка выходного формат-контекста (ffmpeg понятие) для записи в файл" << std::endl;
    int res; // вспомогательная

    const char * out_filename = filename;
  
    avformat_alloc_output_context2(&output_format_context, NULL, NULL, out_filename);

    AVStream *out_stream;
    out_stream = avformat_new_stream(output_format_context, NULL);


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
        if (res < 0) 
        {
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


    RECORD_FLAG = 1; // флаг что к записи готов
    record_start_time=av_gettime();

}



int StopRecord()
{
    if(RECORD_FLAG==1)
    {
        std::cout << "ОСТАНОВКА ЗАПИСИ" << std::endl;
        RECORD_FLAG=0;
        std::cout << "Записываем трейлер" << std::endl;

        av_write_trailer(output_format_context); // запись трейлера
        avio_closep(&output_format_context->pb); // завершение взаимодействия с файлом посредством формат-контекста

        avformat_free_context(output_format_context);
    }
    else
    {
      std::cout << "Останавливать нечего. Запись итак не шла" << std::endl;  
    }

    return 0;
}






// ЗАПИСЬ ПАКЕТА (КАДРА) В ФАЙЛ ИЛИ RTSP
int WriteFrame()
{



}


    
int DecodeH264(uint8_t *inbuf, int inbufSize)
{
    if(pCodecContext == NULL || pFrame == NULL || inbufSize <= 0)
    {
        std::cout << "Еще рано декодировать, т.к. отсутствуют нужные объекты (не было инициализации всяких штук от ffmpeg)" << std::endl;
        return 1;
    }

    
    pPacket->data = inbuf;
    pPacket->size = inbufSize;
  
    //int * pSUPER = new int [500000]; // СПЕЦИАЛЬНЫЙ БАГ

    av_packet_ref(pSparePacket, pPacket);
    
    int result = avcodec_send_packet(pCodecContext, pSparePacket);
    
    //std::cout << "что то из запасного пакета: " << pSparePacket->size << std::endl;
    //std::cout << "результат отправки пакета: " << result << std::endl;

    if(result == 0)
    {
        result = avcodec_receive_frame(pCodecContext, pFrame);

        if(pFrame->pict_type == AV_PICTURE_TYPE_I && RECORD_FLAG == 1)
        {
            //std::cout << "ПРИШЕЛ I-КАДР" << std::endl;
            Flag_I_Frame = 1;
        }

        if(pFrame->pict_type == AV_PICTURE_TYPE_I)
        {
            std::cout << "ПРИШЕЛ I-КАДР" << std::endl;
            Flag_I_Frame = 1;
        }

        if(pFrame->pict_type == AV_PICTURE_TYPE_P)
        {
            std::cout << "ПРИШЕЛ P-КАДР" << std::endl;
        }

        if(pFrame->pict_type == AV_PICTURE_TYPE_B)
        {
            std::cout << "ПРИШЕЛ B-КАДР" << std::endl;
            // ЭКСПЕРИМЕНТ ПОКАЗАЛ ЧТО B-КАДРОВ НЕТ ИЗ ТЕПЛОВИЗОРА
        }

        int height_output = sws_scale(pResizeContext, (const uint8_t * const*) pFrame->data, pFrame->linesize, 0, 512, dst_data, dst_linesize);

        //std::cout << "высота output: " << height_output << std::endl;

    }

    
     int64_t now_time = av_gettime();
    
    if(RECORD_FLAG == 1)
    {     
        // ПОХОДУ Я ЗАПИСЫВАЛ В ФАЙЛ ТОЛЬКО I-КАДРЫ 

        //WriteFrame();
        
        pSparePacket->stream_index = 0;

        std::cout << "TIMEBASE ПОТОКА ЗАПИСЫВАЕМОГО ФАЙЛА: " << output_format_context->streams[0]->time_base.den << std::endl;


        // добавлено сегодня
        pSparePacket->pts = (now_time - record_start_time) * 90000 / 1000000;
        pSparePacket->dts = pSparePacket->pts - 8000;
        pSparePacket->duration = 3000;

        int res = av_interleaved_write_frame(output_format_context, pSparePacket);
        if (res < 0)
        {   
            fprintf(stderr, "Error muxing packettzzz\n");
            exit(1);
        }   

        std::cout << "Время прошедшее со старта записи: " << (av_gettime() - record_start_time) / 1000000 << std::endl;
    }

   
   

    if(RTSP_FLAG == 1) // ЕСЛИ RTSP-СТРИМИНГ ВКЛЮЧЕН
    {
        std::cout << "RTSP_FLAG=" << RTSP_FLAG << std::endl;

        pOutputContext_RTSP->streams[0]->time_base.den = 30000;
        std::cout << "TIMEBASE нового потока: " << pOutputContext_RTSP->streams[0]->time_base.den << std::endl;
        
        // считаем время со старта RTSP-стриминга
        int64_t stream_time_seconds = (av_gettime() - rtsp_start_time) / 1000000;

        pPacket->pts = (now_time - rtsp_start_time) * 30000 / 1000000;
        pPacket->dts = pPacket->pts - 8000;
        pPacket->duration = 3000;


        std::cout << "pts пакета = " << pPacket->pts << std::endl;
        std::cout << "dts пакета = " << pPacket->dts << std::endl;
        std::cout << "длительность пакета = " << pPacket->duration << std::endl;

        if(Flag_I_Frame == 1)
        {
            int res = av_write_frame(pOutputContext_RTSP, pPacket);
            if (res < 0)
            {
                fprintf(stderr, "Error при отправки на RTSP сервер\n");
                exit(1);
            }
        }      

        std::cout << "Время старта RTSP-стриминга: " << rtsp_start_time << std::endl;
        std::cout << "Время прошедшее со старта RTSP-стриминга: " << (av_gettime() - rtsp_start_time) / 1000000 << std::endl;  
    }


    //av_packet_unref(pPacket);
    //av_packet_unref(pSparePacket);
    //av_packet_free(&pPacket);
    //av_packet_free(&pSparePacket);
   
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

    H264_DecoderInitialization();
        
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
                    std::string newfilename = GetCurrentTimestamp(1);
                    newfilename.append(".mp4");
                    
                    RecordInit(newfilename.c_str());
                }


                if(event.key.code == 10)
                {
                    std::cout << "ОСТАНОВКА ЗАПИСИ" << std::endl;

                    StopRecord();   
                }

                if(event.key.code == 16)
                {
                    std::cout << "Старт RTSP-стрима" << std::endl;

                    RTSP_Start();
                }

                if(event.key.code == 22)
                {
                    std::cout << "Старт RTSP-стрима" << std::endl;

                    RTSP_Stop();
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







// ПОТОЧНАЯ ФУНКЦИЯ, ДЛЯ РЕАЛИЗАЦИИ СТРИМИНГА
void * VideoThread(void * args)
{
    H264_DecoderInitialization();
    while(1)
    {
        sleep(10);
    }
}

