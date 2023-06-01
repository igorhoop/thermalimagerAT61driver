#include <iostream>
#include <string>
#include "include/InfraredTempSDK.h"

#include "headers/myfunctions.h"


IRNETHANDLE pSdk;               // дескриптор для работы с SDK
struct ChannelInfo Chan_Info;   // структура с информацией о подключении

bool isLogin = false;           // флаг прохождения успешной авторизации в устройстве

DeviceList devicelist;

char curCapName[50] = "capture3";



// ФУНКЦИЯ-ОБРАБОТЧИК ПРИХОДА CООБЩЕНИЙ
void MessageCallBackReceiveMy(IRNETHANDLE SdkHandle, WPARAM wParam, LPARAM iParam, void * context)
{
    std::cout << "wParam: " << wParam << std::endl;
    std::cout << "iParam: " << iParam << std::endl;
    std::cout << "Мессаги! Мессаги! " << iParam << std::endl;

}




int main()
{
    float camTemp;
    int subresult;
    char camSN[30];
    char camPN[50];

    std::cout << "Это драйвер для тепловизора AT61F (производства Infiray)." << std::endl;
    initial();
    std::cout << "Пробуем подключиться..." << std::endl;

    // установка функций-обработчиков
    //int res = SetTempCallBack(pSdk, TempCallBackMy, NULL);
    SetSerialCallBack(pSdk, Chan_Info, SerialCallBackMy, NULL);
    //SetSnapCallBack(pSdk, Chan_Info, SnapCallBackMy, NULL);
    SetMessageCallBack(pSdk, MessageCallBackReceiveMy, NULL);
    sdk_set_capture_format(pSdk, Chan_Info, 4);

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
        std::cout << "Ожидание входящих команд..." << std::endl;

        /*
        printf("\n\n\n ==== НАЧАЛО ОЖИДАНИЯ КЛИЕНТА ==== \n____________\n\n");
        
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

        bytes_recv = recv(exchange_socket, luxuary_buf, 1024, 0);
        std::string currentRequest(luxuary_buf, bytes_recv);
        std::cout << "Пришло в буфер: \n\n" << currentRequest << std::endl;

        int typereq = GetTypeOfRequest(currentRequest);
        */

        IRG_Param irg_param;
        
        int typereq = 4;
        std::string PointName = "CurrentPoint";
        int zz;

        switch(typereq)
        {
            case 1: // проверка доступности камеры костылем "считать температуру камеры"
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

            

            case 2:
                sdk_get_temp_data(pSdk, Chan_Info, 256, area_temp);
                
                std::cout << "TempMax: " << (float) area_temp.iTempMax/10 << std::endl;
                std::cout << "TempMin: " << (float) area_temp.iTempMin/10 << std::endl;
                std::cout << "TempCent: " << (float) area_temp.iTempCenter/10 << std::endl;
                std::cout << "TempAvg: " << (float) area_temp.iTempAvg/10 << std::endl;
                break;

            case 3:
                sdk_CapSingle(pSdk, Chan_Info);

                response = "Snimok sdelan";
                break;

            case 4:
                
                sdk_snapshot(pSdk, Chan_Info, 1, (char *) PointName.c_str());
                // далее извлекаем из irg файла данные
                zz = sdk_get_irg_param( (char *) PointName.append(".irg").c_str(), &irg_param);
                std::cout << zz << std::endl;

                //PointName.append(GetCurrentTimestamp(1));
                
                std::cout << "\tСнэпшот сделан: " << GetCurrentTimestamp(1) << std::endl;

                response = "Snapshot sdelan";
                break;

            case 0: // reload
                std::cout << "Reload Parameters" << std::endl;   
                sdk_release(pSdk);
                initial();
                SetSerialCallBack(pSdk, Chan_Info, SerialCallBackMy, NULL);
                SetSnapCallBack(pSdk, Chan_Info, SnapCallBackMy, NULL);
                SetMessageCallBack(pSdk, MessageCallBackReceiveMy, NULL);
                break;
        }

        continue;

         // ФОРМИРОВАНИЕ ОТВЕТА
        
        result = send(exchange_socket, response.c_str(), response.size(), 0);
        std::cout << "Отправляем обратно количество байт: " << result << std::endl;

        close(exchange_socket); 

        continue;



    }

    close(listener_socket);
    return 0;

/*
    while(sdk_get_camera_temp(pSdk, &camTemp) == -1)


        //uint8_t sendCmd[] = { 0xAA, 0x04, 0x01, 0x70, 0x00, 0x1F, 0xEB, 0xAA };
        //sdk_serial_cmd_send(pSdk, reinterpret_cast<char*>(sendCmd), 8);

        

    //sdk_osd_switch(SdkHandle, Chan_Info, 1);

    sdk_set_temp_unit(SdkHandle, Chan_Info, 0);
    
    
    // установка функций обработки приходящих данных
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
    
    

    */
    
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



    std::cout << "\n\n\n\n" << std::endl;
   
    // шаг 5. Подключение к устройству
    
    
    

    //chninf.byChanNum = 0;
   
    //step_result =  // связывание структуры и дескриптора?
    //std::cout << "Result: " << step_result << std::endl;
   
    
    //SetSerialCallBack(pHandle, chninf, serialCallback, NULL);
    //sdk_release(mySdkHandle);
*/