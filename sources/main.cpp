#include <iostream>
#include "../include/InfraredTempSDK.h"
#include "../headers/3l_functions.h"
#include <cstring>
#include <pthread.h>


#include <fstream>

#include <nlohmann/json.hpp>

#include "../headers/Netabstraction.h"




bool SDK_INIT = false;

IRNETHANDLE pSdk;                 // дескриптор для работы с SDK
struct ChannelInfo Device_Info;   // структура с информацией о подключении к устройству, заполняется автоматически чтением config-файла
struct PROGRAM_CONFIG CONFIG;


int main()
{
    std::string CapturePath;    // путь к снимкам, который требуется взять из переменной среды
    std::string VideoPath;    // путь к видео, который требуется взять из переменной среды
    std::string LogPath;        // путь к лог-файлу
    
    std::cout << "Старт модуля взаимодействия с тепловизором AT61F (Infiray)." << std::endl;

    

    // чтение переменных среды
    if(getenv("AT61F_CONFIG_PATH")==NULL)
    {
        std::cout << "Не задана переменная среды с путем расположения config-файла. Завершение работы" << std::endl;
        exit(1);
    }
    else
    {
        CONFIG.AT61F_CONFIG_PATH = getenv("AT61F_CONFIG_PATH");
        std::cout << "Расположение config-файла: " << CONFIG.AT61F_CONFIG_PATH <<  std::endl;
    }


    // чтение конфигурации для работы программы
    ReadConfigFromJSON();

    std::cout << "==== Параметры программы ====" << std::endl;
    std::cout << "Путь для сохранения снимков: " << CONFIG.AT61F_CAPTURE_PATH <<  std::endl;
    std::cout << "Путь для сохранения видео: " << CONFIG.AT61F_VIDEO_PATH <<  std::endl;
    std::cout << "Путь для лог-файла: " << CONFIG.AT61F_LOG_PATH <<  std::endl;
    std::cout << "URL сервера для RTSP: " << CONFIG.AT61F_RTSP_URL << std::endl;

    std::cout << "==== Параметры тепловизора ====" << std::endl;
    std::cout << "IP: " << CONFIG.AT61F_IP <<  std::endl;
    std::cout << "PORT: " << CONFIG.AT61F_PORT <<  std::endl;
    std::cout << "LOGIN: " << CONFIG.AT61F_LOGIN <<  std::endl;
    std::cout << "PASS: " << CONFIG.AT61F_PASS <<  std::endl;
    std::cout << "AIRTEMP: " << CONFIG.AT61F_AIRTEMP <<  std::endl;
    std::cout << "EMISSIVITY: " << CONFIG.AT61F_EMISSIVITY <<  std::endl;
    std::cout << "HUMIDITY: " << CONFIG.AT61F_HUMIDITY <<  std::endl;
    std::cout << "DISTANCE: " << CONFIG.AT61F_DISTANCE <<  std::endl;
    std::cout << "TMIN: " << CONFIG.AT61F_TMIN <<  std::endl;
    std::cout << "TMAX: " << CONFIG.AT61F_TMAX <<  std::endl;

  

    // Старт потока, контролирующего соединение с тепловизором
    pthread_t thread;
    int result_thread;
    result_thread = pthread_create(&thread, NULL, &PingDeviceThread, NULL);


    // Старт потока для работы с окном
    pthread_t window_thread;
    int result_window_thread;
    result_window_thread = pthread_create(&window_thread, NULL, &WindowVideoThread, NULL);

    result_window_thread = pthread_create(&window_thread, NULL, &VideoThread, NULL);
    

    // записываем лог о включении программы
    std::string log_message;
    log_message.append(GetCurrentTimestamp(1));
    log_message.append(" - Запуск программы\n");
    LogWrite(LogPath, log_message);

    // создаем абстракционный сетевой объект
    Netabstraction NetObject(30001);

    // Старт rtsp-стримминга
    //RTSP_Start();
        

    // ОСНОВНОЙ ЦИКЛ
    while(true)
    {
        // === переменные необходимые для итерации ===
        std::array<uint8_t, 327680> response_temp_data = {0};       // массив для всех пикселей
        int32_t PixCoordX = 0;                                      // переменная для хранения координаты X пикселя
        int32_t PixCoordY = 0;                                      // переменная для хранения координаты Y пикселя

        RESPONSE_TYPES TypeResponse; // для формирования ответа

        envir_param get_envir_data;

        // переменная от которой будет зависеть длина ответного пакета
        int32_t Answer_size = 0;

        SENDPARAM OutputStructData = {0};
        GETTEMPLIM TempLimitStructData = {0};
        GETDISTANCE DistanceStructData = {0};
        GETAIRTEMP AirTempStructData = {0};
        GETENVIRPARAMS EnvirParamStructData = {0};

        printf("\n ==== ЖДЕМ ОЧЕРЕДНУЮ КОМАНДУ ОТ РОБОТА ==== \n____________\n\n");
        NetObject.Receive(); // ЗДЕСЬ ТЕКУЩИЙ ПОТОК ПОДВИСАЕТ В ОЖИДАНИИ ВХОДЯЩИХ ЗАПРОСОВ   

        // берем первый байт из буфера приема, чтобы узнать тип запроса
        // оставшуюся часть запроса сохраняем в виде текста
        uint8_t TypeRequest = NetObject.Receive_Buff[0];
        printf("Тип запроса: %02X \n", TypeRequest);
        std::string RequestText(NetObject.Receive_Buff+1, NetObject.GetRecvBytes()-1); 

        
        
        switch(TypeRequest)
        {
            case 1: // ФОРМИРОВАНИЕ СНИМКА
                // сначала формируем имя снимка
                MakeCapture(CapturePath, RequestText, &OutputStructData);
                Answer_size=14;
                break;

            case 111: // ЗАПИСЬ ВИДЕО
                // сначала формируем имя видео
                MakeVideo(VideoPath, RequestText, &OutputStructData);
                Answer_size=14;
                break;   

            case 222: // СТОП ВИДЕО
                StopRecord();                
                Answer_size=14;
                break;



            case 2: // ЗАПРОС ТЕМПЕРАТУР
                RequestTemperatures(&OutputStructData);
                TypeResponse = TEMPERATURES;
                break;


            case 31: // УСТАНОВКА ТЕМПЕРАТУРЫ ОКРУЖАЮЩЕЙ СРЕДЫ
                if((NetObject.GetRecvBytes()) == sizeof(GETAIRTEMP))
                {
                    memcpy(&AirTempStructData, NetObject.Receive_Buff, sizeof(AirTempStructData));
                    SetAirTemp(CONFIG.AT61F_CONFIG_PATH, AirTempStructData.air_temp);
                    OutputStructData.error = 0x00;   
                }
                else
                {
                    std::cout << "\tНеверный размер структуры. Ответим ошибкой" << std::endl; 
                    OutputStructData.error = 0x06;
                }
                TypeResponse = RESULT_OF_COMMAND;
                break;


            case 32: // УСТАНОВКА ДИСТАНЦИИ
                if((NetObject.GetRecvBytes()) == sizeof(GETDISTANCE))
                {
                    memcpy(&DistanceStructData, NetObject.Receive_Buff, sizeof(DistanceStructData));
                    SetDistance(CONFIG.AT61F_CONFIG_PATH, DistanceStructData.distance);
                    OutputStructData.error = 0x00;
                }
                else
                {
                    std::cout << "\tНеверный размер структуры. Ответим ошибкой" << std::endl; 
                    OutputStructData.error = 0x06;
                }
                TypeResponse = RESULT_OF_COMMAND;
                break;

            case 33: // УСТАНОВКА ИЗЛУЧАЕМОСТИ И ВЛАЖНОСТИ
                if((NetObject.GetRecvBytes()) == sizeof(GETENVIRPARAMS))
                {
                    memcpy(&EnvirParamStructData, NetObject.Receive_Buff, sizeof(EnvirParamStructData));
                    SetEmissivityHumidity(CONFIG.AT61F_CONFIG_PATH, EnvirParamStructData.emissivity, EnvirParamStructData.humidity);
                    OutputStructData.error = 0x00;
                }
                else
                {
                    std::cout << "\tНеверный размер структуры. Ответим ошибкой" << std::endl; 
                    OutputStructData.error = 0x06;
                }
                TypeResponse = RESULT_OF_COMMAND;
                break;


            case 4: // ЗАПРОС ТЕМПЕРАТУРЫ ПИКСЕЛЯ
                std::memcpy(&PixCoordX, &NetObject.Receive_Buff[1], sizeof(uint32_t));
                std::memcpy(&PixCoordY, &NetObject.Receive_Buff[5], sizeof(uint32_t));
                GetTemperaturePixel(CapturePath, PixCoordX, PixCoordY, &OutputStructData);
                OutputStructData.error = 0x00;
                TypeResponse = TEMPERATURE_PIXEL;
                break;


            case 5: // ЗАПРОС КАРТЫ ПИКСЕЛЕЙ
                if(GetMapPixel(CapturePath, response_temp_data) == 0)
                {
                    TypeResponse = MAP;
                    //DrawMap(response_temp_data);
                }
                else
                {
                    TypeResponse = RESULT_OF_COMMAND;
                    OutputStructData.error = 0x05;
                }
                break;    

            case 6: // УСТАНОВКА НУЖНЫХ НАМ ТЕМПЕРАТУРНЫХ ПОРОГОВ
                if((NetObject.GetRecvBytes()) == sizeof(GETTEMPLIM))
                {
                    memcpy(&TempLimitStructData, NetObject.Receive_Buff, sizeof(TempLimitStructData));
                    SetTemperatureLimit(TempLimitStructData.min_t, TempLimitStructData.max_t);
                    OutputStructData.error = 0x00; // флаг для ответа что все хорошо
                }
                else
                {
                    std::cout << "\tНеверный размер структуры. Ответим ошибкой" << std::endl; 
                    OutputStructData.error = 0x06;
                }
                TypeResponse = RESULT_OF_COMMAND;
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
                std::cout << "\tTmin: " << CONFIG.AT61F_TMIN << std::endl;
                std::cout << "\tTmax: " << CONFIG.AT61F_TMAX << std::endl;
                break;

            default: 
                std::cout << "Неизвестный тип запроса" << std::endl;
                TypeResponse = NONE;
                break;
        }


        // ФОРМИРОВАНИЕ ОТВЕТА
        switch(TypeResponse)
        {
            case MAP:
                NetObject.Send(&response_temp_data, 327680);
                break;

            case TEMPERATURES:
                NetObject.Send(&OutputStructData, 14);
                // покажем в консоли что было отправлено
                //char ArrayStructData[100];  // сюда скопируем байты структуры чтобы вывести в консоли побайтово  
                //std::memcpy(ArrayStructData, &OutputStructData, 14);
                //std::cout << "\tСостав ответной посылки: ";
                //for(int i=0; i < 14; i++)
                //{
                //    printf("0x%02X, ", ArrayStructData[i]); // отображаем принятые байты
                //}
                break;

            case TEMPERATURES_AND_SIGNAL:
                NetObject.Send(&OutputStructData, 14);
                break;

            case RESULT_OF_COMMAND:
                NetObject.Send(&OutputStructData, 4);
                break;

            case TEMPERATURE_PIXEL:
                NetObject.Send(&OutputStructData, 5);
                break;

            case NONE:
                std::cout << "Ничего не отправляем обратно" << std::endl;
                break;

            default:
                std::cout << "Тип ответа не задан. ERROR." << std::endl;
                break;
        }
        
        continue; // возврат на новую итерацию цикла
    }


    // конец программы
    sdk_release(pSdk);
    return 0;
}







    
    