#include <iostream>
#include <string>
#include "include/InfraredTempSDK.h"

#include "headers/myfunctions.h"
#include <array>
#include <cstdint>
#include <cstring>


IRNETHANDLE pSdk;               // дескриптор для работы с SDK
struct ChannelInfo Chan_Info;   // структура с информацией о подключении

bool isLogin = false;           // флаг прохождения успешной авторизации в устройстве

DeviceList devicelist;


#pragma pack(push, 1)
struct SENDPARAM {
 uint8_t error;
 int32_t average_t;
 int32_t min_t;
 int32_t max_t;
 uint8_t signal;
};

struct GETPARAM {
 uint8_t cmd; 
 int32_t around_temp;
 int32_t distance;
 int32_t min_t;
 int32_t max_t;
};
#pragma pack(pop)


int main()
{
    float camTemp;
    int subresult;
    char camSN[30];
    char camPN[50];

    // Устанавливаемые параметры для тепловизора
    int32_t SettedAroundT = 0;
    int32_t SettedDist = 0;

    

    int TmmFlag = false; // флаг установки порога минимальной и максимальной температуры

    int32_t SettedTmax = 0;
    int32_t SettedTmin = 0;

    unsigned short temp_data[640*512] = { 0 };
    unsigned char image_data[1000*1000] = { 0 };

    

    std::cout << "Это драйвер для тепловизора AT61F (производства Infiray)." << std::endl;
    initial();
    // Установка функций-обработчиков. Это надо после инициализации
    //int res = SetTempCallBack(pSdk, TempCallBackMy, NULL);
    SetSerialCallBack(pSdk, Chan_Info, SerialCallBackMy, NULL);
    //SetSnapCallBack(pSdk, Chan_Info, SnapCallBackMy, NULL);
    sdk_set_capture_format(pSdk, Chan_Info, 4);

    sdk_set_color_plate(pSdk, Chan_Info, 2); // цветовая гамма

    char * nadpis ="3Logic Group Robotic Systems";


    Custom_String osdContent;
    osdContent.iFormat = 2;
    osdContent.iFormatTime = 5;
    osdContent.iShow= 1;
    osdContent.iIndex = 3;
    std::memcpy(osdContent.m_szString, nadpis, sizeof(osdContent.m_szString));
    osdContent.iWidth = 100;
    osdContent.iDeviceWidth = 640;
    osdContent.iDeviceHeight = 512;
    osdContent.iX = 5;
    osdContent.iY = 50;
    osdContent.iStringX = 5;
    osdContent.iStringY = 490;
    sdk_set_osd_display(pSdk, Chan_Info, osdContent);
    
    std::cout << "Первоначальное подключение к тепловизору..." << std::endl;
    subresult = sdk_get_camera_temp(pSdk, &camTemp);

    
  
    if(subresult == 0)
    {
        std::cout << "\tСигнал от камеры есть" << std::endl;
        sdk_get_SN_PN(pSdk, Chan_Info, camSN, camPN);
        std::cout << "\ts/n=" << camSN << std::endl;
        std::cout << "\tp/n=" << camPN << std::endl;
        sdk_get_camera_temp(pSdk, &camTemp);
        std::cout << "\tтемпература камеры: " << camTemp << " Цельсия" << std::endl;
    }
    else
    {
        std::cout << "\tСигнала от камеры нет. Завершаемся" << std::endl;
        return 0;
    }

    std::cout << "Ожидание входящих команд..." << std::endl;

    char com = 'a';
    Area_Temp area_temp = { 0 };

    // сетевые установки
    int bytes_send; // количество отправленных клиенту байт
    int bytes_recv; // количество полученных от клиента байт
    int listener_socket; // дескриптор слушающего сокета
    int exchange_socket; // десприктор сокета для обмена данными с клиентом
    sockaddr_in ServerAddr; // адресная структура, которую нужно заполнить и связать с сокетом. Хранит IP-адрес
    sockaddr_in ClientAddr; // адресная структура клиента для инфы о нем, которая будет заполняться при приходе от него сообщений
    int ClientAddrSize = sizeof(ClientAddr); // адрес этой переменной, в которой лежит размер структуры, передадим функции accept()
    sockaddr * addr;
    socklen_t * addr_size;
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = INADDR_ANY; // раннее было inet_addr(my_ip), таким образом привяжемся к конкретному сетевому интерфейсу. Либо использовать INADDR_ANY, для входящих от всех интерфейсов
    ServerAddr.sin_port = htons(30001); // занимаемый порт на моем компьютере
    listener_socket = socket(AF_INET,SOCK_STREAM, 0); // Семейство Internet, потоковые сокеты, TCP    
    if (bind(listener_socket, (sockaddr *) &ServerAddr, sizeof(ServerAddr)) < 0)
    {
        std::cerr << "Ошибка bind\nПерезапустить через минуту?\n"; // МИНУТА!
        return 0;;
    }
    // для TCP нужно возиться с соединениями (использовать listen() и accept())
    // для передачи и приема используем 
   
    if(listen(listener_socket, 5) < 0) // создание очереди запросов на соединение. Указывается размер. Блокирует программу (??? походу нет) и ждет подключений
    {
        std::cerr << "Ошибка Listen";
    }
    char recv_buf[1024]; // приемный буфер
    char * luxuary_buf = new char[1000];
    int result;
    bool flag=true;


    while(true)
    {
        std::string response;

        std::array<uint8_t, 14> response_temp_data = {0};

        SENDPARAM OutputStructData = {0};
        GETPARAM InputStructData = {0};

        char reserve_byte_1 = 0;
        char reserve_byte_2 = 0;
        char reserve_byte_3 = 0;

        std::cout << "Ожидание входящих запросов..." << std::endl;

        
        printf("\n\n\n ==== ЖДЕМ КОМАНДУ ОТ РОБОТА ==== \n____________\n\n");
        
        exchange_socket = accept(listener_socket, (sockaddr *) &ClientAddr, (socklen_t *) &ClientAddrSize); // а вот здесь уже блокируется программа. Извлекает первый запрос из очереди либо если очередь пустая ждет и блокирует программу до первого соединения
        if(exchange_socket >= 0)
        {
            printf("Принят запрос на соединение:\n");
        }
        else
        {
            printf("Что то не так с сокетом обмена:\n");
            std::cout << errno;
            break;
        }
        
        std::string his_ip = "";
        his_ip = inet_ntoa(ClientAddr.sin_addr);
        printf("IP-адрес подключившегося: %s \n", his_ip.c_str()) ;
        printf("Его порт: %d \n", ClientAddr.sin_port);

        // получение данных из сокета
        bytes_recv = recv(exchange_socket, luxuary_buf, 1024, 0);
        std::cout << "Пришло байт: " << bytes_recv << std::endl;

        for(int i=0; i < bytes_recv; i++)
        {
            //std::cout << luxuary_buf[i];
            printf("0x%02X, ", luxuary_buf[i]);
        }

        // берем первый байт чтобы узнать тип запроса
        char command = luxuary_buf[0];
        std::cout << "Тип запроса: " << command+48 << std::endl; // прибавил 48 чтоб отображал число в ASCII
        std::string RestRequest(luxuary_buf+1, bytes_recv-1);
        if(command == 0x01)
        {
            std::cout << "Это команда на формирование снимка. Имя для снимка: " << RestRequest << std::endl;
        }

        IRG_Param irg_param;
        
        // НАСТРОЙКИ ПУТЕЙ К СНИМКАМ
        std::string CaptureName = "";                       // имя снимка
        std::string CapturePath = "./photos/";              // путь к снимку
        int resultmkdir = mkdir(CapturePath.c_str(), 0777); // создаем каталог если он отсутствует
        std::string todayDirName = GetCurrentTimestamp(0);  // готовимся к созданию каталога с именем-датой (сегодняшней)
        CapturePath.append(todayDirName);                   // 
        resultmkdir = mkdir(CapturePath.c_str(), 0777);     // создаем каталог с именем-датой если он отсутствует
        CapturePath.append("/");                            // добавляем заход в каталог
        


        int32_t CalcTavg = 0;
        int32_t CalcTmax = 0;
        int32_t CalcTmin = 0;
        





        char answer_error = 0x00;
        char signal = 0x00;
        int32_t answer_size = 0;
        int32_t Frame_Tavg;
        int32_t Frame_Tmax;
        int32_t Frame_Tmin;
        
        
 
        switch(command)
        {
            
            case 1: // СДЕЛАТЬ СНИМОК
                // сначала формируем имя снимка
                CaptureName = RestRequest;
                if((CaptureName.length() < 3) || (CaptureName.length() > 39))
                {
                    std::cout << "Неподходящая длина имени снимка" << std::endl;
                    OutputStructData.error = 0x02;
                }
                else
                {
                    CaptureName.append("_");
                    CaptureName.append(GetCurrentTimestamp(2));
                    std::cout << "Принята команда на формирование снимка. Имя для него берем из запроса: " << CaptureName << std::endl;
                    CapturePath.append(CaptureName);
                    std::cout << "Путь сохранения снимка: " << CapturePath << std::endl;

                    // делаем jpeg и irg файл
                    sdk_snapshot(pSdk, Chan_Info, 1, (char *) CapturePath.c_str());

                    // далее извлекаем из irg файла данные
                    CapturePath.append(".irg");
                    sdk_get_irg_data( (char *) CapturePath.c_str(), 4, temp_data, image_data);

                    CalcTmax = temp_data[0];
                    CalcTmin = temp_data[0];
                    CalcTavg = temp_data[0];
                    
                    for(int i=0; i < 327680; i++)
                    {
                        CalcTavg += temp_data[i];

                        if(CalcTmax < temp_data[i])

                        {
                            CalcTmax = temp_data[i];
                        }

                        if(CalcTmin > temp_data[i])
                        {
                            CalcTmin = temp_data[i];
                        } 
                    }

                    CalcTavg /= 327680;
                    CalcTavg = (CalcTavg)/10-273.2;
                    CalcTmax = (CalcTmax)/10-273.2;
                    CalcTmin = (CalcTmin)/10-273.2;

                    std::cout << "\tCalcTavg=" << CalcTavg<< std::endl;
                    std::cout << "\tCalcTmax=" << CalcTmax << std::endl;
                    std::cout << "\tCalcTmin=" << CalcTmin << std::endl;

                    std::cout << "\tСнимок сделан: " << std::endl;
                

                    OutputStructData.average_t = CalcTavg;
                    OutputStructData.min_t = CalcTmin;
                    OutputStructData.max_t = CalcTmax;
                    

                    if(TmmFlag == 1)
                    {
                        if((CalcTmax >= SettedTmax) && (CalcTmin >= SettedTmin))
                            OutputStructData.signal = 0x01;
                    }
                    else
                    {
                        OutputStructData.error = 0x01;
                    }

                }


                remove(CapturePath.c_str());
                answer_size=14;
                break;   


            case 2: // ЗАПРОС ПАРАМЕТРОВ
                sdk_get_temp_data(pSdk, Chan_Info, 256, area_temp);

                
                Frame_Tavg = area_temp.iTempAvg/10;
                Frame_Tmax = area_temp.iTempMax/10;
                Frame_Tmin = area_temp.iTempMin/10;

    
                OutputStructData.average_t = Frame_Tavg;
                OutputStructData.min_t = Frame_Tmin;
                OutputStructData.max_t = Frame_Tmax;
                
                std::cout << "TempMax: " << Frame_Tmax << std::endl;
                std::cout << "TempMin: " <<  Frame_Tmin << std::endl;
                std::cout << "TempAvg: " << Frame_Tavg << std::endl;


                if(TmmFlag == 1)
                {
                    if((Frame_Tmax >= SettedTmax) && (Frame_Tmin >= SettedTmin))
                        OutputStructData.signal = 0x01;
                }
                else
                {
                    OutputStructData.error = 0x01;
                }

                answer_size = 14;
                break;


            case 3: // УСТАНОВКА ПАРАМЕТРОВ

                std::cout << "Пришла команда на установку параметров" << std::endl;


                if((bytes_recv) == sizeof(GETPARAM))
                {
                    std::cout << "metka" << std::endl;
                    memcpy(&InputStructData, luxuary_buf, sizeof(InputStructData));
                }
                
                std::cout << "command: " << InputStructData.cmd << std::endl;
                std::cout << "aroundTemp: " << InputStructData.around_temp << std::endl;
                std::cout << "distance: " << InputStructData.distance << std::endl;
                std::cout << "min_t: " << InputStructData.min_t << std::endl;
                std::cout << "max_t: " << InputStructData.max_t << std::endl;

                SettedAroundT = InputStructData.around_temp;
                SettedDist = InputStructData.distance;
                SettedTmin = InputStructData.min_t;
                SettedTmax = InputStructData.max_t;
                TmmFlag=1; // значения установлены

                
                answer_size = 4;

                break;




            case 0: // reload
                std::cout << "Reload Parameters" << std::endl;   
                sdk_release(pSdk);
                initial();
                SetSerialCallBack(pSdk, Chan_Info, SerialCallBackMy, NULL);
                SetSnapCallBack(pSdk, Chan_Info, SnapCallBackMy, NULL);
                break;

            case 77:
                sdk_CapSingle(pSdk, Chan_Info);

                response = "Snimok sdelan";
                break;

            case 4: // проверка доступности камеры костылем "считать температуру камеры"

            std::cout << "Проверка камеры..." << std::endl;
                subresult = sdk_get_camera_temp(pSdk, &camTemp);
                if(subresult==0)
                {
                    std::cout << "\tКамера на связи" << std::endl;
                }
                else
                {
                    std::cout << "\tКамера не отвечает" << std::endl;       
                }
                break;    
                
              

            default: 
                std::cout << "неизвестный тип запроса" << std::endl;
                response = "Неизвестный тип запроса";
                break;

            
        }

        //continue;

         // ФОРМИРОВАНИЕ ОТВЕТА
        
        //result = send(exchange_socket, response.c_str(), response.size(), 0);
        
        //if(answer_size == 4)
        //{

        
        result = send(exchange_socket, &OutputStructData, answer_size, 0);
        std::cout << "Отправляем обратно количество байт: " << result << std::endl;
        //}
        //else
        //{
        //    result = send(exchange_socket, &mystruct, sizeof(GETPARAM), 0);
        //}

        

        close(exchange_socket); 

        continue;



    }

    sdk_release(pSdk);
    close(listener_socket);
    return 0;

/*

        

    //sdk_osd_switch(SdkHandle, Chan_Info, 1);

    sdk_set_temp_unit(SdkHandle, Chan_Info, 0);
    
    Alarm_Config alarm_config;
    
    int vvv = sdk_set_temp_alarm(SdkHandle, Chan_Info, 256, alarm_config);
    std::cout << "vvv=" << vvv << std::endl;

    int iUnit;
    sdk_set_temp_unit(SdkHandle, Chan_Info, 0);
    sdk_get_temp_unit(SdkHandle, Chan_Info, &iUnit);

    int zapis = sdk_start_record(SdkHandle, Chan_Info, "./file222");
    std::cout << "zapis res: " << zapis << std::endl;
   
   

   //sdk_osd_switch(pSdk, Chan_Info, 1); // включение/отключение OSD


    
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


    /*
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



*/


}

/*
    DeviceList deviceList = { 0 }; // структура для хранения списка устройств
    //step_result = sdk_search_device(SdkHandle, deviceList); // вернет 0 при успешном результате, -1 при неудачном
    //std::cout << "Результат поиска устройства: " << step_result << std::endl;

    std::cout << "Кол-во устройств: " << deviceList.iNumber << std::endl;

    if(deviceList.iNumber > 0)#include "additional.cpp"
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


    
    

    //chninf.byChanNum = 0;
   
    //step_result =  // связывание структуры и дескриптора?
    //std::cout << "Result: " << step_result << std::endl;
   
    
    //SetSerialCallBack(pHandle, chninf, serialCallback, NULL);
    //sdk_release(mySdkHandle);
*/




// старый метод как я делал, потом через структуры сделали

                /*
                std::memcpy(&SettedAroundT, &luxuary_buf[1], sizeof(uint32_t));
                std::memcpy(&SettedDist, &luxuary_buf[5], sizeof(uint32_t));
                std::memcpy(&SettedTmin, &luxuary_buf[9], sizeof(uint32_t));
                std::memcpy(&SettedTmax, &luxuary_buf[13], sizeof(uint32_t));

                std::cout << "Установленный AroundTemp=" << SettedAroundT << std::endl;
                std::cout << "Установленный SettedDist=" << SettedDist << std::endl;
                std::cout << "Установленный Tmin=" << SettedTmin << std::endl;
                std::cout << "Установленный Tmax=" << SettedTmax << std::endl;

                std::memcpy(&response_temp_data[1], &reserve_byte_1, sizeof(char));
                std::memcpy(&response_temp_data[2], &reserve_byte_2, sizeof(char));
                std::memcpy(&response_temp_data[3], &reserve_byte_3, sizeof(char));
                */