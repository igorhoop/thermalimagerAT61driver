#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <unistd.h>

#include <mutex>
std::mutex mutex;




std::vector<unsigned short> _temp_data(640 * 512); // для температурной матрицы
int DecodeH264(uint8_t *inbuf, int inbufSize);

int flag_live_buffer;

int _width = 384;       // ???
int _height = 288;      // ???

//short * videodata = new short(1000)

// ФУНКЦИЯ-ОБРАБОТЧИК ПРИХОДА ВИДЕОКАДРА
void VideoCallBackMy(char *pBuffer, long BufferLen, int width, int height, void* pContext)
{
    //video_data = new unsigned char[BufferLen];
    //memcpy(&video_data[0], pBuffer, BufferLen);

    uint8_t * videoData;
    int videoDataSize;
    
    videoDataSize = 0;

    videoData = new uint8_t [500000]; // для обращения в нужный порядок байт
    flag_live_buffer = 1;
    
    //std::lock_guard<std::mutex> lock(mutex);

    memcpy(videoData, pBuffer, BufferLen);
    videoDataSize = BufferLen;

    std::cout << "Пришло байт(видеокадр): " << BufferLen << std::endl;

    //std::cout << "Вызываем функцию декодирования" << std::endl;
    DecodeH264(videoData, videoDataSize);


    //sleep(1);

    
    
    //DrawVideoFrame(video_data, BufferLen);
    

    //
    //unsigned char ** outbuff2 = new unsigned char (400000);
    



    //unsigned short videoData[50000];   //1280*1024*1.5 данные изображения
    //_width = width;
    //_height = height;
    //memcpy(videoData, pBuffer, BufferLen);

    //std::cout << "VideoCallBackReceive: _width=" << _width << std::endl;


    //Пример отображения 100 предварительных данных
    //for (int i = 0; i < 50; i++)
    //    printf("%04X, ", videoData[i]);

    //std::cout << std::endl;
    
    //delete video_data;
    //flag_live_buffer = 0;

    
    delete [] videoData;

    //std::cout << "Все, данные удалились" << std::endl;
    
}



// ФУНКЦИЯ-ОБРАБОТЧИК ПРИХОДА ТЕМПЕРАТУРНОЙ МАТРИЦЫ
void TempCallBackMy(char *pBuffer, long BufferLen, void* pContext)
{
    int min = 5000, max = 0;

    //std::cout << "Пришли данные по температуре. Длина буфера: " << BufferLen << std::endl;
    

    float celsius_point;
    memcpy(_temp_data.data(), pBuffer, BufferLen);

    //printf("Рандомный элемент: %d \n", _temp_data[1200]);
    
    for (int i = 0; i < 327636; i++) // больше чем в 327636 лежит какая-то параша
    {
        if(_temp_data[i]==0)
            std::cout << "Номер нулевых: " << i << std::endl;

        if(_temp_data[i]>3000)
            std::cout << "Номер слишком больших: " << i << ". Значение: " << _temp_data[i] << std::endl;

        if(_temp_data[i]<500)
            std::cout << "Номер слишком маленьких: " << i << ". Значение: " << _temp_data[i] << std::endl;

        if(_temp_data[i] < min)
            min = _temp_data[i];
        
        if(_temp_data[i] > max)
            max = _temp_data[i];

        

        //celsius_point = ((_temp_data[i]+ 7000)/30) - 273.2;
        //std::cout << celsius_point << std::endl;
    }
    //std::cout << "Global MIN: " << min << std::endl;
    //std::cout << "Global MAX: " << max << std::endl;
}


// ФУНКЦИЯ-ОБРАБОТЧИК ПРИХОДА СЕРИЙНЫХ ДАННЫХ
void SerialCallBackMy(char *pRecvDataBuff, int BuffSize, void *context)
{
    /*
    printf("Приход данных. Прилетело %d байт \n", BuffSize);
    if (BuffSize < 0)
    {
        std::cout << "SerialCallBackReceive: Неудача? Буфер равен: " << BuffSize ;
        return;
    }
    int serialDataSize = BuffSize;
    unsigned char serialData[512];
    for (int i = 0; i < serialDataSize; ++i)
    {
        printf("%c", pRecvDataBuff[i]);
    }
    */
}


// ФУНКЦИЯ-ОБРАБОТЧИК ПРИХОДА CНИМКОВ
void SnapCallBackMy(int m_ch, char *pBuffer, int size, void *context)
{
    // мы используем sdk_snapshot(), в которой имя файла задается в аргументах
    
    /*
    if(pBuffer)
    {
        //char filePath[200] = "capture.jpg";
        //GetModuleFileName(NULL, filePath, sizeof(filePath));
        //SYSTEMTIME st;
        //GetLocalTime(&st);
        //char fileName[50] = "test.jpg";
        //sprintf(fileName, "test.JPG");
        //strcat_s(filePath, fileName);

        std::string fullCapName = curCapName;
        fullCapName.append(".jpg");

        FILE * pFile = fopen(fullCapName.c_str(), "wb");
        if (!pFile)
        {
            return;
        }
        if (!fwrite(pBuffer, size, 1, pFile)) // ошибка записи файла
            std::cout << "Не удалось сохранить файл" << std::endl;
        else
            std::cout << "Сохранен снимок с именем: " << fullCapName.c_str() << std::endl;
        fclose(pFile);


    }

    */
}


