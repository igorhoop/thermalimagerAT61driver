#include <iostream>
#include <string>
#include "../include/InfraredTempSDK.h"
#include "../headers/3l_functions.h"
#include <array>
#include <cstdint>
#include <cstring>
#include <string_view>
#include <pthread.h>
#include <string>


#define NETPORT 30001                           // порт на который сядет эта программа
constexpr std::string_view version = "1.0";     // версия это программы


std::string AirTemp;
std::string Emissivity;
std::string ReflectTemp;
std::string Humidity;
std::string Distance;



bool SDK_INIT = false;

int32_t SettedTmax = 0;         // максимальный порог температуры для срабатывания сигнала
int32_t SettedTmin = 0;         // минимальный порог температуры для срабатывания сигнала
bool TminmaxFlag = false;       // флаг установки порога минимальной и максимальной температуры

IRNETHANDLE pSdk;                 // дескриптор для работы с SDK
struct ChannelInfo Device_Info;   // структура с информацией о подключении к устройству, заполняется автоматически чтением config-файла




int main()
{
    std::string ConfigPath;     // путь к конфигурационному файлу, который требуется взять из переменной среды
    std::string CapturePath;    // путь к снимкам, который требуется взять из переменной среды

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



    //RewriteFileContent(ConfigPath, "port=", std::to_string(888));

    //exit(1);

    Area_Temp area_temp = { 0 };    // структура куда SDK по запросу будет класть температурные данные текущего кадра
    


    // Устанавливаемые физические параметры для тепловизора
    int32_t SettedDist = 0;         // расстояние до объекта
    int32_t SettedEmissivity = 0;   // излучаемость объекта
    int32_t SettedReflectTemp = 0;  // отражающаяся от поверхности температура
    int32_t SettedHumidity = 0;     // влажность

    // Устанавливаемые программные параметры для взаимодействия с ПО Вектор

    int32_t PixCoordX = 0;          // переменная для хранения координаты X пикселя
    int32_t PixCoordY = 0;          // переменная для хранения координаты Y пикселя
    
    unsigned short temp_data[640*512] = { 0 };     // !!! могут быть минусовые, поправить тип. буфер для хранения температурной матрицы 
    unsigned char image_data[1000*1000] = { 0 };    // буфер для хранения RGB-данных снимка. С размером буфера не разобрался

    std::cout << "Version: " << version << std::endl;
    std::cout << "Это драйвер для тепловизора AT61F (производства Infiray)." << std::endl;
    
    
    
    
    
    pthread_t thread;
    int result_thread;
    result_thread = pthread_create(&thread, NULL, &PingDeviceThread, NULL);

    




    // СЕТЕВЫЕ УСТАНОВКИ
    int bytes_send;         // количество отправленных клиенту байт
    int bytes_recv;         // количество полученных от клиента байт
    int listener_socket;    // дескриптор слушающего сокета
    int exchange_socket;    // десприктор сокета для обмена данными с клиентом
    sockaddr_in ServerAddr; // адресная структура, которую нужно заполнить и связать с сокетом. Хранит IP-адрес
    sockaddr_in ClientAddr; // адресная структура клиента для инфы о нем, которая будет заполняться при приходе от него сообщений
    int ClientAddrSize = sizeof(ClientAddr); // адрес этой переменной, в которой лежит размер структуры, передадим функции accept()
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



    while(true)
    {
        std::array<uint8_t, 327680> response_temp_data = {0}; // массив для всех пикселей

        int vsp_res = 999;
        envir_param get_envir_data;

        SENDPARAM OutputStructData = {0};
        GETTEMPLIM TempLimitStructData = {0};
        GETDISTANCE DistanceStructData = {0};
        GETAIRTEMP AirTempStructData = {0};
        GETENVIRPARAMS EnvirParamStructData = {0};

        std::string TempPath;// начало формируемого пути для файла (берется из config файла)

        // СОЗДАНИЕ ИМЕНИ СНИМКОВ И ПОДКАТАЛОГОВ ДЛЯ ИХ МЕСТОПОЛОЖЕНИЯ
        std::string CaptureName = "";          // сюда будет класться имя снимка
        std::string TodayDirName = "";         // сюда будет класть имя сегодняшней директории-даты



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


        uint8_t Command;   // переменная куда мы положим номер команды, которая пришла

    
        std::string currentRequest(Receive_Buff, bytes_recv);

        // берем первый байт чтобы узнать тип запроса
        Command = Receive_Buff[0];
        printf("Тип запроса: %02X \n", Command);

       
       std::string RestRequestText(Receive_Buff+1, bytes_recv-1); // сохраняем оставшуюся часть запроса в виде текста
        
        
    
        // переменные в которые запишем подсчитанные вручную значения температур пикселей (при работе с IRG-файлом)
        int32_t CalcTavg = 0;
        int32_t CalcTmax = 0;
        int32_t CalcTmin = 0;
        int32_t CalcTpix = 0;
        
        // переменная в которую запишем номер нужного пикселя, потребуется при запросе температуры пикселя по X и Y
        int NumPixel = 0;

        // переменные в которые запишем значения температур, полученных из специальной SDK-шной структуры 
        int32_t Frame_Tavg = 0;
        int32_t Frame_Tmax = 0;
        int32_t Frame_Tmin = 0;

        // переменная от которой будет зависеть длина ответного пакета
        int32_t Answer_size = 0;

        

        switch(Command)
        {
            case 0x01: // СДЕЛАТЬ СНИМОК
                // сначала формируем имя снимка
                CaptureName = RestRequestText;
                if((CaptureName.length() < 3) || (CaptureName.length() > 39))
                {
                    std::cout << "Неподходящая длина имени снимка" << std::endl;
                    OutputStructData.error = 0x02;
                }
                else
                {
                    std::cout << "Принята команда на формирование снимка. Имя для него берем из запроса: " << CaptureName << std::endl;
                    TempPath = CapturePath;    // начало формируемого пути для файла (берется из config файла)

                    mkdir(TempPath.c_str(), 0777);          // создаем основной каталог если он отсутствует
                    TodayDirName = GetCurrentTimestamp(0);  // готовимся к созданию подкаталога с именем-датой (сегодняшней)
                    TempPath.append(TodayDirName);          // добавляем к основному пути имя подкаталога-дату
                    mkdir(TempPath.c_str(), 0777);          // создаем подкаталог-дату если он отсутствует
                    TempPath.append("/");                   // добавляем заход в этот подкаталог
                    
                    CaptureName.append("_");                     // формируем имя для снимка добавлением к принятому имени 
                    CaptureName.append(GetCurrentTimestamp(2));  // нижнего подчеркивания и текущего времени

                    TempPath.append(CaptureName);
                    std::cout << "Подготовленный путь сохранения снимка: " << TempPath << std::endl;


                    
                    if(!PingDevice()) // поэтому перед вызовом проверяем связь с устройством
                    {
                        // Создаем jpeg и irg файл. Эта функция выкинет segfault если вызвать ее при отключенном тепловизоре
                        // При этом если не выполнена конфигурация, то все равно вернет УСПЕХ, не создав файлов. Просто говно-SDK
                        sdk_snapshot(pSdk, Device_Info, 1, (char *) TempPath.c_str());

                        // далее извлекаем из irg файла данные. По наличию jpeg и irg файлов ориентируемся на эту функцию, так как sdk_snapshot() ни о чем
                        TempPath.append(".irg");
                        vsp_res = sdk_get_irg_data( (char *) TempPath.c_str(), 4, temp_data, image_data);
                        if(vsp_res == -1)
                        {
                            std::cout << "Не получилось создать файлы: jpeg и irg. Скорее всего не выполнена конфигурация" << std::endl;
                            OutputStructData.error = 0x04;
                        }
                        else
                        {
                            std::cout << "Создание файлов: jpeg и irg - ОК. Вытаскиваем данные..." << std::endl;

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

                            std::cout << "\tДанные получены:" << std::endl;
                            std::cout << "\tCalcTavg=" << CalcTavg<< std::endl;
                            std::cout << "\tCalcTmin=" << CalcTmin << std::endl;
                            std::cout << "\tCalcTmax=" << CalcTmax << std::endl;

                            OutputStructData.average_t = CalcTavg;
                            OutputStructData.min_t = CalcTmin;
                            OutputStructData.max_t = CalcTmax;

                            if(TminmaxFlag == true)
                            {
                                if((CalcTmax >= SettedTmax) || (CalcTmin <= SettedTmin))
                                    {
                                        OutputStructData.signal = 0x01;
                                        std::cout << "\tЗафиксировано превышение допустимой температуры. Формируем сигнал" << std::endl;
                                    }
                            }
                            else
                            {
                                OutputStructData.error = 0x01;
                                std::cout << "\tОбратно отправлять будем с ошибкой, так как не выставлены температурные пороги" << std::endl;
                            }
                        }
                    }
                    else
                    {
                        OutputStructData.error = 0x05;
                        std::cout << "\tОбратно отправлять будем с ошибкой: нет связи с устройством" << std::endl;
                    }
                }

                remove(TempPath.c_str());   // удаляем irg-файл, так как уже вытащили из него всю нужную информацию
                Answer_size=14;
                break;   


            case 0x02: // ЗАПРОС ТЕМПЕРАТУР
                std::cout << "Принята команда на запрос температур" << std::endl;
                // эта функция подвисает программу. Segfault не дает.
                if(!PingDevice())
                {
                    vsp_res = sdk_get_temp_data(pSdk, Device_Info, 256, area_temp);

                    Frame_Tavg = area_temp.iTempAvg/10;
                    Frame_Tmax = area_temp.iTempMax/10;
                    Frame_Tmin = area_temp.iTempMin/10;

                    OutputStructData.average_t = Frame_Tavg;
                    OutputStructData.min_t = Frame_Tmin;
                    OutputStructData.max_t = Frame_Tmax;
                    
                    std::cout << "TempAvg: " << Frame_Tavg << std::endl;  
                    std::cout << "TempMin: " <<  Frame_Tmin << std::endl;             
                    std::cout << "TempMax: " << Frame_Tmax << std::endl;

                    if(TminmaxFlag == 1)
                    {
                        if((Frame_Tmax >= SettedTmax) || (Frame_Tmin <= SettedTmin))
                        {
                            OutputStructData.signal = 0x01;
                            std::cout << "\tЗафиксировано превышение допустимой температуры. Формируем сигнал" << std::endl;
                        }
                            
                    }
                    else
                    {
                        OutputStructData.error = 0x01;
                        std::cout << "\tОбратно отправлять будем с ошибкой, так как не выставлены температурные пороги" << std::endl;
                    } 
                }
                else
                {
                    std::cout << "Нет связи с тепловизором, возвращаем ошибку" << std::endl;
                    OutputStructData.error = 0x05;
                }

                Answer_size = 14;
                break;

            case 31:
                std::cout << "Пришла команда на установку температуры окружающей среды" << std::endl;
 
                if((bytes_recv) == sizeof(GETAIRTEMP))
                {
                    memcpy(&AirTempStructData, Receive_Buff, sizeof(AirTempStructData));
                    std::cout << "\tAirTemp: " << (int) AirTempStructData.around_temp << std::endl;
                    std::cout << "\tУстанавливаем эти значения..." << std::endl;
                    // здесь надо вписывать в файл, после чего сделать реинициализацию SDK и переподключение к устройству, чтобы считать новые параметры из файла
                    RewriteFileContent(ConfigPath, "AirTemp=", std::to_string(AirTempStructData.around_temp));
                    OutputStructData.error = 0x00;
                }
                else
                {
                    std::cout << "\tНеверный размер структуры. Ответим ошибкой" << std::endl; 
                    OutputStructData.error = 0x06;
                }
                Answer_size = 4;
                ReinitialAndConnect();
                ConfigDevice();
                break;

            case 32:
                std::cout << "Пришла команда на установку дистанции" << std::endl;
                if((bytes_recv) == sizeof(GETDISTANCE))
                {
                    memcpy(&DistanceStructData, Receive_Buff, sizeof(DistanceStructData));
                    std::cout << "\tDistance: " << (int) DistanceStructData.distance << std::endl;

                    RewriteFileContent(ConfigPath, "Distance=", std::to_string(DistanceStructData.distance));
                }
                else
                {
                    std::cout << "\tНеверный размер структуры. Ответим ошибкой" << std::endl; 
                    OutputStructData.error = 0x06;
                }
                Answer_size = 4;
                ReinitialAndConnect();
                ConfigDevice();
                break;

            case 33:
                std::cout << "Пришла команда на установку излучаемости и влажности" << std::endl;
                if((bytes_recv) == sizeof(GETENVIRPARAMS))
                {
                    memcpy(&EnvirParamStructData, Receive_Buff, sizeof(EnvirParamStructData));
                    std::cout << "\tEmissivity: " << (float) EnvirParamStructData.emissivity << std::endl;
                    std::cout << "\tHumidity: " << (float) EnvirParamStructData.humidity << std::endl;

                    RewriteFileContent(ConfigPath, "Emissivity=", std::to_string(EnvirParamStructData.emissivity));
                    RewriteFileContent(ConfigPath, "Humidity=",  std::to_string(EnvirParamStructData.humidity));
                }
                else
                {
                    std::cout << "\tНеверный размер структуры. Ответим ошибкой" << std::endl; 
                    OutputStructData.error = 0x06;
                }
                Answer_size = 4;
                ReinitialAndConnect();
                ConfigDevice();
                break;


            case 6: // УСТАНОВКА ПАРАМЕТРОВ ОКРУЖАЮЩЕЙ СРЕДЫ И НУЖНЫХ НАМ ТЕМПЕРАТУРНЫХ ПОРОГОВ
                std::cout << "Пришла команда на установку температурных порогов" << std::endl;
                if((bytes_recv) == sizeof(GETTEMPLIM))
                {
                    memcpy(&TempLimitStructData, Receive_Buff, sizeof(TempLimitStructData));

                    //std::cout << "command: " << TempLimitStructData.cmd << std::endl;
                    std::cout << "\tMin_t: " << TempLimitStructData.min_t << std::endl;
                    std::cout << "\tMax_t: " << TempLimitStructData.max_t << std::endl << std::endl;

                    std::cout << "\tУстанавливаем эти пороги..." << std::endl;    

                    // из
                    RewriteFileContent(ConfigPath, "Tmin=",  std::to_string(TempLimitStructData.min_t));
                    RewriteFileContent(ConfigPath, "Tmax=",  std::to_string(TempLimitStructData.max_t));


                    OutputStructData.error = 0x00; // флаг для ответа что все хорошо
                }
                else
                {
                    std::cout << "\tНеверный размер структуры. Ответим ошибкой" << std::endl; 
                    OutputStructData.error = 0x06;
                }
                Answer_size = 4;
                ReinitialAndConnect();
                ConfigDevice();
                break;


            case 0x04:
                std::cout << "Пришла команда на чтение температуры пикселя" << std::endl;

                TempPath = CapturePath;    // начало формируемого пути для файла (берется из config файла)

                CaptureName="temp_pixel";
                std::cout << "Принята команда на чтение температуры пикселя. Сначала делаем снимок, имя для него берем временное: " << CaptureName << std::endl;

                

                mkdir(TempPath.c_str(), 0777);          // создаем основной каталог если он отсутствует
                TodayDirName = GetCurrentTimestamp(0);  // готовимся к созданию подкаталога с именем-датой (сегодняшней)
                TempPath.append(TodayDirName);          // добавляем к основному пути имя подкаталога-дату
                mkdir(TempPath.c_str(), 0777);          // создаем подкаталог-дату если он отсутствует
                TempPath.append("/");                   // добавляем заход в этот подкаталог

                TempPath.append(CaptureName);
                std::cout << "Итоговый путь сохранения снимка: " << TempPath << std::endl;

                if(!PingDevice()) // поэтому перед вызовом проверяем связь с устройством
                {

                    // делаем jpeg и irg файл
                    sdk_snapshot(pSdk, Device_Info, 1, (char *) TempPath.c_str());

                    // далее извлекаем из irg файла данные
                    TempPath.append(".irg");
                    sdk_get_irg_data( (char *) TempPath.c_str(), 4, temp_data, image_data);

                    CalcTpix = temp_data[0];

                    std::memcpy(&PixCoordX, &Receive_Buff[1], sizeof(uint32_t));
                    std::memcpy(&PixCoordY, &Receive_Buff[5], sizeof(uint32_t));

                    std::cout << "\tКоордината x=" << PixCoordX << std::endl;
                    std::cout << "\tКоордината y=" << PixCoordY << std::endl;

                    NumPixel = (PixCoordY * 640) + PixCoordX;

                    CalcTpix = temp_data[NumPixel];

                    CalcTpix = (CalcTpix)/10-273.2; // посчитанное значение температуры пикселя


                    std::cout << "\tCalcTpix=" << CalcTpix << std::endl;
                    OutputStructData.average_t = CalcTpix;

                    if((PixCoordX > 640) || (PixCoordY > 512))
                    {
                        OutputStructData.error = 0x01;
                    }
                }
                else
                {
                        OutputStructData.error = 0x05;
                        std::cout << "\tОбратно отправлять будем с ошибкой: нет связи с устройством" << std::endl;
                }
                remove(TempPath.c_str());
                Answer_size=5;
                break;

            case 0x05: // запрос карты пикселей
                std::cout << "Пришла команда на массив всех пикселей" << std::endl;

                TempPath = CapturePath;    // начало формируемого пути для файла (берется из config файла)

                CaptureName="temp_pixel";
                std::cout << "Принята команда на чтение температуры пикселя. Сначала делаем снимок, имя для него берем временное: " << CaptureName << std::endl;

                mkdir(TempPath.c_str(), 0777);          // создаем основной каталог если он отсутствует
                TodayDirName = GetCurrentTimestamp(0);  // готовимся к созданию подкаталога с именем-датой (сегодняшней)
                TempPath.append(TodayDirName);          // добавляем к основному пути имя подкаталога-дату
                mkdir(TempPath.c_str(), 0777);          // создаем подкаталог-дату если он отсутствует
                TempPath.append("/");                   // добавляем заход в этот подкаталог

                TempPath.append(CaptureName);
                std::cout << "Итоговый путь сохранения снимка: " << TempPath << std::endl;
                
                if(!PingDevice()) // поэтому перед вызовом проверяем связь с устройством
                {
                    // делаем jpeg и irg файл
                    sdk_snapshot(pSdk, Device_Info, 1, (char *) TempPath.c_str());

                    // далее извлекаем из irg файла данные
                    TempPath.append(".irg");
                    sdk_get_irg_data( (char *) TempPath.c_str(), 4, temp_data, image_data);
                    
                    for(int i=0; i < 327680; i++)
                    {
                        response_temp_data[i] = temp_data[i]/10 - 273.2;
                    }


                    std::cout << "\tМассив подготовлен" << std::endl;
                    Answer_size=327680;
                }
                else
                {
                        response_temp_data[0] = 0x05;
                        std::cout << "\tОбратно отправлять будем с ошибкой: нет связи с устройством" << std::endl;
                        Answer_size=4;                  
                }
               
                remove(TempPath.c_str());
                
                
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


            case 0x91: // принудительная реинициализация
                std::cout << "Принудительная реинициализация...\n" << std::endl;   
                ReinitialAndConnect();
                break;

            case 0x92: // чтение текущих параметров окружающей среды
                std::cout << "Чтение...\n" << std::endl;   
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
                std::cout << "неизвестный тип запроса" << std::endl;
                break;
        }


        // ФОРМИРОВАНИЕ ОТВЕТА
        int result;
        if(Answer_size==327680)
        {
            result = send(exchange_socket, &response_temp_data, Answer_size, 0);
            std::cout << "Отправляем обратно количество байт: " << result << std::endl;

        }
        else
        {
        result = send(exchange_socket, &OutputStructData, Answer_size, 0);
        std::cout << "Отправляем обратно количество байт: " << result << std::endl;

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

        close(exchange_socket); 
        continue;

    }

    sdk_release(pSdk);
    close(listener_socket);
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






    //int res = SetTempCallBack(pSdk, TempCallBackMy, NULL);
    