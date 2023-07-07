#include <iostream>
#include "../include/InfraredTempSDK.h"
#include "../headers/3l_functions.h"
#include <cstring>
#include <pthread.h>


constexpr std::string_view version = "1.0";     // версия это программы

bool SDK_INIT = false;

int32_t SettedTmax = 0;         // максимальный порог температуры для срабатывания сигнала
int32_t SettedTmin = 0;         // минимальный порог температуры для срабатывания сигнала
bool TminmaxFlag = false;       // флаг установки порога минимальной и максимальной температуры

IRNETHANDLE pSdk;                 // дескриптор для работы с SDK
struct ChannelInfo Device_Info;   // структура с информацией о подключении к устройству, заполняется автоматически чтением config-файла



int main()
{
    std::string CapturePath;    // путь к снимкам, который требуется взять из переменной среды
    std::string ConfigPath;     // путь к конфигурационному файлу, который требуется взять из переменной среды
    
    std::cout << "Старт модуля взаимодействия с тепловизором AT61F (Infiray)." << std::endl;
    std::cout << "Version: " << version << std::endl;




    // чтение переменных среды
    if(getenv("AT61F_CONFIG_PATH")==NULL)
    {
        std::cout << "Не задана переменная среды с путем расположения config-файла. Завершение работы" << std::endl;
        exit(1);
    }
    else
    {
        ConfigPath = getenv("AT61F_CONFIG_PATH");
        std::cout << "Расположение config-файла: " << ConfigPath <<  std::endl;
    }

    if(getenv("AT61F_CAPTURE_PATH")==NULL)
    {
        std::cout << "Не задана переменная среды с путем сохранения снимков. Завершение работы" << std::endl;
        exit(1);
    }
    else
    {
        CapturePath = getenv("AT61F_CAPTURE_PATH");
        std::cout << "Путь для сохранения снимков:" << CapturePath <<  std::endl;
    }
 
    // Старт контролирующего соединение потока
    pthread_t thread;
    int result_thread;
    result_thread = pthread_create(&thread, NULL, &PingDeviceThread, NULL);


    // Старт потока для работы с окном
    pthread_t window_thread;
    int result_window_thread;
    result_window_thread = pthread_create(&window_thread, NULL, &WindowThread, NULL);



    // создаем абстракционный сетевой объект
    NETABSTRACTION NetObject;
    //NetObject.port = NETPORT;
    int vsp = NetObject.Initialization(30001);
    if(vsp != 0)
        exit(1);

    /*
    // СЕТЕВЫЕ УСТАНОВКИ

   
    sockaddr * addr;
    socklen_t * addr_size;


    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = INADDR_ANY; // раннее было inet_addr(my_ip), таким образом привяжемся к конкретному сетевому интерфейсу. Либо использовать INADDR_ANY, для входящих от всех интерфейсов
    ServerAddr.sin_port = htons(NETPORT);      // занимаемый порт на моем компьютере
    listener_socket = socket(AF_INET,SOCK_STREAM, 0); // Семейство Internet, потоковые сокеты, TCP    
    if (bind(listener_socket, (sockaddr *) &ServerAddr, sizeof(ServerAddr)) < 0)
    {
        std::cerr << "Ошибка bind\nПерезапустить через минуту?\n"; // МИНУТА!
        return 0;;
    }
    // для TCP нужно возиться с соединениями (использовать listen() и accept())
    if(listen(listener_socket, 5) < 0) // создание очереди запросов на соединение. Указывается размер. Блокирует программу (??? походу нет) и ждет подключений
    {
        std::cerr << "Ошибка Listen";
    }
    char * Receive_Buff = new char[100];    // сетевой приемный буфер
    */  


    while(true)
    {
        // === переменные необходимые для итерации ===
        std::array<uint8_t, 327680> response_temp_data = {0};       // массив для всех пикселей
        int32_t PixCoordX = 0;                                      // переменная для хранения координаты X пикселя
        int32_t PixCoordY = 0;                                      // переменная для хранения координаты Y пикселя

        envir_param get_envir_data;

        uint8_t Command;   // переменная куда мы положим номер команды, которая пришла

        // переменная от которой будет зависеть длина ответного пакета
        int32_t Answer_size = 0;


        SENDPARAM OutputStructData = {0};
        GETTEMPLIM TempLimitStructData = {0};
        GETDISTANCE DistanceStructData = {0};
        GETAIRTEMP AirTempStructData = {0};
        GETENVIRPARAMS EnvirParamStructData = {0};

        printf("\n ==== ЖДЕМ КОМАНДУ ОТ РОБОТА ==== \n____________\n\n");
        NetObject.Receive();

        /*
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
        printf("\tIP-адрес подключившегося: %s \n", his_ip.c_str()) ;
        printf("\tЕго порт: %d\n", ClientAddr.sin_port);
        std::cout << "\tВремя подключения: " << GetCurrentTimestamp(1) << std::endl;
        // получение данных из сокета
        bytes_recv = recv(exchange_socket, Receive_Buff, 100, 0);
        std::cout << "\tПришло байт: " << bytes_recv << std::endl;
        std::cout << "\tСостав посылки: \n\t";
        for(int i=0; i < bytes_recv; i++)
        {
            printf("0x%02X, ", Receive_Buff[i]); // отображаем принятые байты
        }
        std::cout << std::endl << std::endl;
        */



    
        //std::string currentRequest(Receive_Buff, bytes_recv);

        // берем первый байт чтобы узнать тип запроса
        Command = NetObject.Receive_Buff[0];
        printf("Тип запроса: %02X \n", Command);

       
        std::string RestRequestText(NetObject.Receive_Buff+1, NetObject.bytes_recv-1); // сохраняем оставшуюся часть запроса в виде текста
        

        switch(Command)
        {
            case 1: // ФОРМИРОВАНИЕ СНИМКА
                // сначала формируем имя снимка
                MakeCapture(CapturePath, RestRequestText, &OutputStructData);
                Answer_size=14;
                break;   


            case 2: // ЗАПРОС ТЕМПЕРАТУР
                RequestTemperatures(&OutputStructData);
                Answer_size = 14;
                break;


            case 31: // УСТАНОВКА ТЕМПЕРАТУРЫ ОКРУЖАЮЩЕЙ СРЕДЫ
                if((NetObject.bytes_recv) == sizeof(GETAIRTEMP))
                {
                    memcpy(&AirTempStructData, NetObject.Receive_Buff, sizeof(AirTempStructData));
                    SetAirTemp(ConfigPath, AirTempStructData.air_temp);
                    OutputStructData.error = 0x00;
                }
                else
                {
                    std::cout << "\tНеверный размер структуры. Ответим ошибкой" << std::endl; 
                    OutputStructData.error = 0x06;
                }
                Answer_size = 4;
                break;


            case 32: // УСТАНОВКА ДИСТАНЦИИ
                if((NetObject.bytes_recv) == sizeof(GETDISTANCE))
                {
                    memcpy(&DistanceStructData, NetObject.Receive_Buff, sizeof(DistanceStructData));
                    SetDistance(ConfigPath, DistanceStructData.distance);
                    OutputStructData.error = 0x00;
                }
                else
                {
                    std::cout << "\tНеверный размер структуры. Ответим ошибкой" << std::endl; 
                    OutputStructData.error = 0x06;
                }
                Answer_size = 4;
                break;

            case 33: // УСТАНОВКА ИЗЛУЧАЕМОСТИ И ВЛАЖНОСТИ
                if((NetObject.bytes_recv) == sizeof(GETENVIRPARAMS))
                {
                    memcpy(&EnvirParamStructData, NetObject.Receive_Buff, sizeof(EnvirParamStructData));
                    SetEmissivityHumidity(ConfigPath, EnvirParamStructData.emissivity, EnvirParamStructData.humidity);
                    OutputStructData.error = 0x00;
                }
                else
                {
                    std::cout << "\tНеверный размер структуры. Ответим ошибкой" << std::endl; 
                    OutputStructData.error = 0x06;
                }
                Answer_size = 4;
                break;



            case 4: // ЗАПРОС ТЕМПЕРАТУРЫ ПИКСЕЛЯ
                std::memcpy(&PixCoordX, &NetObject.Receive_Buff[1], sizeof(uint32_t));
                std::memcpy(&PixCoordY, &NetObject.Receive_Buff[5], sizeof(uint32_t));
                GetTemperaturePixel(CapturePath, PixCoordX, PixCoordY, &OutputStructData);
                OutputStructData.error = 0x00;

                Answer_size=5;
                break;


            case 5: // ЗАПРОС КАРТЫ ПИКСЕЛЕЙ
                if(GetMapPixel(CapturePath, response_temp_data) == 0)
                {
                    Answer_size=327680;
                    DrawMap(response_temp_data);
                }
                else
                {
                    Answer_size=4;
                    OutputStructData.error = 0x05;
                }
                
                break;    



            case 6: // УСТАНОВКА НУЖНЫХ НАМ ТЕМПЕРАТУРНЫХ ПОРОГОВ
                if((NetObject.bytes_recv) == sizeof(GETTEMPLIM))
                {
                    memcpy(&TempLimitStructData, NetObject.Receive_Buff, sizeof(TempLimitStructData));
                    SetTemperatureLimit(ConfigPath, TempLimitStructData.min_t, TempLimitStructData.max_t);
                    OutputStructData.error = 0x00; // флаг для ответа что все хорошо
                }
                else
                {
                    std::cout << "\tНеверный размер структуры. Ответим ошибкой" << std::endl; 
                    OutputStructData.error = 0x06;
                }
                Answer_size = 4;
                break;

                
            case 0xF0: // сброс параметров окружающей среды
                std::cout << "Пришел HTTP-запрос на сброс параметров окружающей среды" << std::endl;   
                std::cout << sdk_reset_param(pSdk, Device_Info);
                break;

            case 0xF3: // 
                std::cout << "Пришел HTTP-запрос на конфигурирование тепловизора" << std::endl;   
                ConfigDevice();
                break;

            case 0xF4:
                if(!PingDevice()) // поэтому перед вызовом проверяем связь с устройством
                {
                    OutputStructData.error = 0x55;
                    Answer_size = 1;
                }
                else
                {
                    OutputStructData.error = 0x65;
                    Answer_size = 1;
                }
                break;


            // === отладочные случаи ===
            case 0x91: // принудительная реинициализация
                std::cout << "Принудительная реинициализация...\n" << std::endl;   
                ReinitialAndConnect();
                break;
            case 0x92: // чтение текущих параметров окружающей среды
                std::cout << "Чтение текущих установленных параметров окружающей среды...\n" << std::endl;   
                sdk_get_envir_param(pSdk, Device_Info, &get_envir_data);
                std::cout << "Current physical paramerers:" << std::endl; 
                std::cout << "\tairTemp: " << get_envir_data.airTemp << std::endl;
                std::cout << "\temissivity: " << get_envir_data.emissivity << std::endl;
                std::cout << "\treflectTemp: " << get_envir_data.reflectTemp << std::endl;
                std::cout << "\thumidity: " << get_envir_data.humidity << std::endl;
                std::cout << "\tdistance: " << get_envir_data.distance << std::endl;

                std::cout << std::endl;
                std::cout << "Current temperature limits:" << std::endl; 
                std::cout << "\tTmin: " << SettedTmin << std::endl;
                std::cout << "\tTmax: " << SettedTmax << std::endl;
                break;

            default: 
                std::cout << "Неизвестный тип запроса. Ответ не отправляем" << std::endl;
                continue;
                break;
        }


        // ФОРМИРОВАНИЕ ОТВЕТА
        int result;
        if(Answer_size==327680)
        {
            NetObject.Send(&response_temp_data, Answer_size);
        }
        else
        {
            NetObject.Send(&OutputStructData, Answer_size);

            // покажем в консоли что было отправлено
            char ArrayStructData[100];  // сюда скопируем байты структуры чтобы вывести в консоли побайтово  
            std::memcpy(ArrayStructData, &OutputStructData, Answer_size);
            std::cout << "\tСостав ответной посылки: ";
            for(int i=0; i < Answer_size; i++)
            {
                printf("0x%02X, ", ArrayStructData[i]); // отображаем принятые байты
            }
            std::cout << std::endl << std::endl << std::endl;
        }

      
        continue;

    }

    sdk_release(pSdk);

    NetObject.End();
    //close(listener_socket);
    return 0;
}


/*

        

    //sdk_osd_switch(SdkHandle, Device_Info, 1);

    
    
    Alarm_Config alarm_config;
    
    int vvv = sdk_set_temp_alarm(SdkHandle, Device_Info, 256, alarm_config);
    std::cout << "vvv=" << vvv << std::endl;

    int iUnit;
    sdk_set_temp_unit(SdkHandle, Device_Info, 0);
    sdk_get_temp_unit(SdkHandle, Device_Info, &iUnit);

    int zapis = sdk_start_record(SdkHandle, Device_Info, "./file222");
    std::cout << "zapis res: " << zapis << std::endl;
   
   

   //sdk_osd_switch(pSdk, Device_Info, 1); // включение/отключение OSD


    
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
    sdk_set_area_pos(SdkHandle, Device_Info, 5, area1_pos);
    */

    

    //sdk_remove_area_pos(SdkHandle, Device_Info, 6, 2);


    /*


    unsigned short port; 
    sdk_get_onvif_port(SdkHandle, Device_Info, &port);
    std::cout << port;



    
    while(true)
    {
        
        //sleep(10);
        //std::cout << "metka123 " << std::endl;

        //int z = sdk_serial_cmd_send(SdkHandle, reinterpret_cast<char*>(sendCmd), length);
        //std::cout << "Результат отправки: " << z << std::endl;


        


        //z = sdk_serial_cmd_receive(SdkHandle, RecieveBuff, &buflen);
        //std::cout << "Результат приема: " << z << std::endl;
        //sdk_start_url(SdkHandle, Device_Info.szIP);
        //getchar(); // прокручивает сразу много циклов если много символов
        std::cin.get();


    int res = sdk_get_temp_data(SdkHandle, Device_Info, 256, area_temp);
    std::cout << "Result: " << res << std::endl;



    sdk_CapSingle(SdkHandle, Device_Info);

        

    //pallette_num++;

        //sdk_set_device_ip(SdkHandle, Device_Info, "192.168.1.156", 3000);

        //s
        //int zz = sdk_snapshot(SdkHandle, Device_Info, 1, "./ddd");
        //std::cout << "Результат сэпшота: " << zz << std::endl;

    }
    // sdk_start_url(SdkHandle, Device_Info.szIP); // что это? зачем?

    std::cin.get();
    
    sdk_release(SdkHandle); // походу удаление дескриптора
    return 0;



            case 92:
                sdk_CapSingle(pSdk, Device_Info);
                break;

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






    
    