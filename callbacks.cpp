#include <iostream>
#include <string>
#include <vector>
#include <cstring>

extern char curCapName[50];

// ФУНКЦИЯ-ОБРАБОТЧИК ПРИХОДА ТЕМПЕРАТУРНОЙ МАТРИЦЫ
void TempCallBack(char *pBuffer, long BufferLen, void* pContext)
{
    std::vector<unsigned short> _temp_data(1280 * 1024);

    std::cout << "Пришли данные по температуре. Длина буфера: " << BufferLen << std::endl;
 

    float celsius_point;
    memcpy(_temp_data.data(), pBuffer, BufferLen);
    //обработка первых 100 элементов буфера
    for (int i = 0; i < BufferLen; i++)
    {
        celsius_point = ((_temp_data[i]+ 7000)/30) - 273.2;
        std::cout << celsius_point << std::endl;
    }
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


