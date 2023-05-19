#include <iostream>
#include <string>
#include "include/InfraredTempSDK.h"

IRNETHANDLE SdkHandle;          // дескриптор для работы с SDK
int DeviceType = 0;             // тип устройства
struct ChannelInfo Chan_Info;   // структура с информацией о подключении
bool isLogin = false;           // флаг прохождения успешной авторизации в устройстве

DeviceList devicelist;





void device_login()
{
    std::cout << "Попытка залогиниться в устройство..." << std::endl;

    int step_result = 999; 
    char UserName[30] = "888888";
    char UserPass[30] = "888888";
    

    // шаг 1. Установка типа девайса (нах здесь логин и пароль?)
    sdk_set_type(DeviceType, UserName, UserPass); // ничего не возвращает. Зачем здесь юзер и пароль неясно
    


    // шаг 2. Инициализация SDK
    step_result = sdk_initialize();         // инициализация SDK. Вернет 0 при успешном результате
    std::cout << " \tИнициализация SDK: " << step_result << std::endl;

    

    // шаг 3. Создание дескриптора SDK
    SdkHandle = sdk_create();   // 
    if (SdkHandle == NULL)
    {
        std::cout << "\tНеудачное создание SDK :(" << std::endl;
    }
    else
    {
        std::cout << "\tСоздание SDK - успешно :)" << std::endl;
    }


    // шаг xxx.
    //sdk_search_device(SdkHandle, devicelist);

    
    // шаг 4. непосредственно логин в устройство
    Chan_Info.channel = 0;
    Chan_Info.wPortNum = 3000;
    strcpy_s(Chan_Info.szIP, "192.168.1.156");
    strcpy_s(Chan_Info.szServerName, "AT61F-CAM");
    strcpy_s(Chan_Info.szUserName, "888888");
    strcpy_s(Chan_Info.szPWD, "888888");
    
    isLogin = (sdk_loginDevice(SdkHandle, Chan_Info) == 0);
    std::cout << (isLogin?"\tУспешная авторизация (хотя она всегда успешная)":"Неуспешная авторизация :(") << std::endl;
    
}


void MessageCallBackReceive(IRNETHANDLE SdkHandle, WPARAM wParam, LPARAM iParam, void * context)
{
    std::cout << "wParam: " << iParam << std::endl;
}


void SerialCallBackMy(char *pRecvDataBuff, int BuffSize, void *context)
{
    printf("metka metka. Прилетело %d байт \n", BuffSize);
    if (BuffSize < 0)
    {
        std::cout << "SerialCallBackReceive: Неудача? Буфер равен: " << BuffSize ;
        return;
    }
    //CString showData;
    int serialDataSize = BuffSize;
    unsigned char serialData[512];
    for (int i = 0; i < serialDataSize; ++i)
    {
        printf("прилетело: %c\n", pRecvDataBuff[i]);
        //showData.AppendFormat(_T("%02X "), ((UCHAR*)pRecvDataBuff)[i]);
        //serialData[i] = (unsigned char)pRecvDataBuff[i];
    }
    //std::cout << " 数据大小=" + to_string(serialDataSize) << "\n 数据=" + showData << std::endl;
}


int counter = 0;
int snap_counter = 0;


void TemperatureCallBackMy(char *pBuffer, long BufferLen, void* pContext)
{
    //unsigned char _tempBuffer[1280 * 1024 * 2];
    //unsigned short _temp_data[1280 * 1024];

    counter++;

    //std::cout << "Итерация: " << counter << std::endl;
    //std::cout << "Пришли данные по температуре. Длина буфера: " << BufferLen << std::endl;

    std::vector<unsigned char> _tempBuffer(1280 * 1024 * 2);
    std::vector<unsigned short> _temp_data(1280 * 1024);



    //if (_deviceType == DEVICE_TYPE_B)
    //{
    //    memcpy(_tempBuffer, pBuffer, BufferLen);
    //    for (int ii = 0; ii < BufferLen / 4; ii++) //数据转换
    //    {
    //    _temp_data[ii * 2] = (unsigned short)((unsigned short)(_tempBuffer[ii * 2] << 8) + _tempBuffer[ii * 2 + 1 + _width * _height]);
    //    _temp_data[ii * 2 + 1] = (unsigned short)((unsigned short)(_tempBuffer[ii * 2 + 1] << 8) + _tempBuffer[ii * 2 + (_width * _height)]);
    //    }
    //}
    //else
    //{
        float celsius_point;
        memcpy(_temp_data.data(), pBuffer, BufferLen);
        //示例显示 100 个前数据
        for (int i = 0; i < BufferLen; i++)
        {
           celsius_point = ((_temp_data[i]+ 7000)/30) - 273.2;


            if(celsius_point > 40)
            {
                std::cout << celsius_point << std::endl;
                int result = sdk_CapSingle(SdkHandle, Chan_Info);
                //sdk_snapshot(SdkHandle, Chan_Info, 1, "ddd");
            
                
                break;
                
            }
        }

        // сделать скриншот
        
        //sleep(5);
        
    //}
        
    //}

}

void SnapCallBackMy(int m_ch, char *pBuffer, int size, void *context)
{
    snap_counter++;
    
    printf("metka work snap call: %d \n", snap_counter);
    if(pBuffer)
    {
        printf("metka work snap call ZAHOD V USLOVIE\n");
        char filePath[200] = "capture.jpg";
        //GetModuleFileName(NULL, filePath, sizeof(filePath));
        //SYSTEMTIME st;
        //GetLocalTime(&st);
        char fileName[50] = "test.jpg";
        //sprintf(fileName, "test.JPG");
        //strcat_s(filePath, fileName);
        FILE * pFile = fopen(filePath, "wb");
        if (!pFile) //
        {
            printf("metka 777");
            return;
        }
        if (!fwrite(pBuffer, size, 1, pFile)) //写文件失败
            std::cout << "Не удалось сохранить файл";
        else
            std::cout << "Сохранение файла="<< filePath;
        fclose(pFile);
        printf("metka work snap call VYHOD IZ USLOVIYA\n");

        //sleep(5);


    }

}


void VideoCallBackReceiveMy(char *pBuffer, long BufferLen, int width, int height, void* pContext)
{
    
}


char RecieveBuff[500] = { 0 };
int buflen;


int main()
{
    
    std::cout << "Хелло. Это драйвер для тепловизора AT61U (производства Infiray). Пусть он будет работать с нашими роботами." << std::endl;
    device_login();



    //sdk_osd_switch(SdkHandle, Chan_Info, 1);

    sdk_set_temp_unit(SdkHandle, Chan_Info, 0);
    
    SetMessageCallBack(SdkHandle, MessageCallBackReceive, NULL); // НИЧЕГО НЕ ПРОИСХОДИТ
    // установка функций обработки приходящих данных
    SetSerialCallBack(SdkHandle, Chan_Info, SerialCallBackMy, NULL); // установка функции обработки серийных данных
    SetTempCallBack(SdkHandle, TemperatureCallBackMy, NULL);         // установка функции обработки температурных данных
    SetSnapCallBack(SdkHandle, Chan_Info, SnapCallBackMy, NULL);
    SetDeviceVideoCallBack(SdkHandle, VideoCallBackReceiveMy, NULL);

    uint8_t sendCmd[] = {0xAA, 0x04, 0x01, 0x70, 0x00, 0x1F, 0xEB, 0xAA};
    int length = sizeof(sendCmd);


    int imgType = 3;
    sdk_set_capture_format(SdkHandle, Chan_Info, imgType);

    
    Alarm_Config alarm_config;
    
    int vvv = sdk_set_temp_alarm(SdkHandle, Chan_Info, 256, alarm_config);
    std::cout << "vvv=" << vvv << std::endl;


    int iUnit;
    sdk_set_temp_unit(SdkHandle, Chan_Info, 0);
    sdk_get_temp_unit(SdkHandle, Chan_Info, &iUnit);


    int zapis = sdk_start_record(SdkHandle, Chan_Info, "./file222");
    std::cout << "zapis res: " << zapis << std::endl;

    //exit(1);

    std::cout << "\n\n\n\n\n";
    
    Area_Temp area_temp = { 0 };
    
    /*Area_pos area1_pos;
    area1_pos.iMode = 2;
    area1_pos.iIndex = 3;
    area1_pos.m_cursor_enable = true;
    area1_pos.m_osd_enable = true;
    area1_pos.m_region_enable = true;
    area1_pos.iStartPosX = 50;
    area1_pos.iStartPosY = 50;
    area1_pos.iEndPosX = 150;
    area1_pos.iEndPosY = 150;
    sdk_set_area_pos(SdkHandle, Chan_Info, 5, area1_pos);
    */

    //sdk_reset_param(SdkHandle, Chan_Info);

    //sdk_remove_area_pos(SdkHandle, Chan_Info, 6, 2);

    envir_param envir_data;
    envir_param envir_data2;
    envir_data2.airTemp = 200000;
    envir_data2.emissivity = 20000;
    envir_data2.reflectTemp = 100000;
    envir_data2.humidity = 20000;
    envir_data2.distance = 50000;

    //sdk_set_envir_param(SdkHandle, Chan_Info, envir_data2);

    unsigned short port; 
    sdk_get_onvif_port(SdkHandle, Chan_Info, &port);

    std::cout << port;


    char filename[30] = "/home/hoop/file999";
    int iIndex = 4;
    int iWidth = 640;
    int iHeight = 512;
    int nnn = sdk_get_pseudo_color_pic(filename, iIndex, iWidth, iHeight);
    std::cout << nnn;

    //exit(1);
    std::cout << "\n\nqwerty: " << filename << "\n" << iIndex << "\n" << iWidth << "\n" << iHeight << std::endl;

    exit(1);

    
    while(true)
    {
        
        //sleep(10);
        //std::cout << "metka123 " << std::endl;

        //int z = sdk_serial_cmd_send(SdkHandle, reinterpret_cast<char*>(sendCmd), length);
        //std::cout << "Результат отправки: " << z << std::endl;


        


        //z = sdk_serial_cmd_receive(SdkHandle, RecieveBuff, &buflen);
        //std::cout << "Результат приема: " << z << std::endl;
        //sdk_start_url(SdkHandle, Chan_Info.szIP);
        //getchar(); // прокручивает сразу много циклов если много символов
        std::cin.get();


    int res = sdk_get_temp_data(SdkHandle, Chan_Info, 256, area_temp);
    std::cout << "Result: " << res << std::endl;

    std::cout << "TempMax: " << (float) area_temp.iTempMax/10 << std::endl;
    std::cout << "TempMin: " << (float) area_temp.iTempMin/10 << std::endl;
    std::cout << "TempCent: " << (float) area_temp.iTempCenter/10 << std::endl;
    std::cout << "TempAvg: " << (float) area_temp.iTempAvg/10 << std::endl;

    sdk_CapSingle(SdkHandle, Chan_Info);

        

    //pallette_num++;

        //sdk_set_device_ip(SdkHandle, Chan_Info, "192.168.1.156", 3000);

        //s
        //int zz = sdk_snapshot(SdkHandle, Chan_Info, 1, "./ddd");
        //std::cout << "Результат сэпшота: " << zz << std::endl;

    }
    // sdk_start_url(SdkHandle, Chan_Info.szIP); // что это? зачем?

    std::cin.get();
    
    sdk_release(SdkHandle); // походу удаление дескриптора
    return 0;
}





/*
    DeviceList deviceList = { 0 }; // структура для хранения списка устройств
    //step_result = sdk_search_device(SdkHandle, deviceList); // вернет 0 при успешном результате, -1 при неудачном
    //std::cout << "Результат поиска устройства: " << step_result << std::endl;

    std::cout << "Кол-во устройств: " << deviceList.iNumber << std::endl;

    if(deviceList.iNumber > 0)
    {
        for(int i=0; i < deviceList.iNumber; i++)
        {
            std::cout << "Найдено устройство с IP: " << deviceList.DevInfo[i].szIP << std::endl;
        }
    }
    else
    {
        std::cout << "Устройств не найдено. Конец." << std::endl;
        return 1;
    }



    std::cout << "\n\n\n\n" << std::endl;
   
    // шаг 5. Подключение к устройству
    
    
    

    //chninf.byChanNum = 0;
   
    //step_result =  // связывание структуры и дескриптора?
    //std::cout << "Result: " << step_result << std::endl;
   
    
    //SetSerialCallBack(pHandle, chninf, serialCallback, NULL);
    //sdk_release(mySdkHandle);
*/