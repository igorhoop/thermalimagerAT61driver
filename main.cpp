#include <iostream>
#include <string>
#include "include/InfraredTempSDK.h"
#include "headers/3l_functions.h"
#include <array>
#include <cstdint>
#include <cstring>
#include <string_view>


constexpr std::string_view version = "1.0"; // версия это программы
std::string CapturePath;                    // путь к снимкам

#define NETPORT 30001             // порт на который сядет эта программа


IRNETHANDLE pSdk;                 // дескриптор для работы с SDK
struct ChannelInfo Device_Info;   // структура с информацией о подключении к устройству, заполняется автоматически чтением config-файла

bool NeedInit = true;           // флаг сигнализирующий что требуется инициализация SDK


int main()
{

    sdk_set_temp_unit(pSdk, Device_Info, 0);
  

    float DeviceTemp;               // температура тепловизора
    char DeviceSN[30];              // массив для серийного номера
    char DevicePN[50];              // массив для part-номера
    Area_Temp area_temp = { 0 };    // структура куда SDK по запросу будет класть температурные данные текущего кадра
    envir_param envir_data;         //структура для установки физических параметров (окружающей среды)

    // Устанавливаемые физические параметры для тепловизора
    int32_t SettedDist = 0;         // расстояние до объекта
    int32_t SettedEmissivity = 0;   // излучаемость объекта
    int32_t SettedReflectTemp = 0;  // отражающаяся от поверхности температура
    int32_t SettedHumidity = 0;     // влажность

    // Устанавливаемые программные параметры для взаимодействия с ПО Вектор
    int32_t SettedTmax = 0;         // максимальный порог температуры для срабатывания сигнала
    int32_t SettedTmin = 0;         // минимальный порог температуры для срабатывания сигнала
    bool TminmaxFlag = false;       // флаг установки порога минимальной и максимальной температуры

    int32_t PixCoordX = 0;          // переменная для хранения координаты X пикселя
    int32_t PixCoordY = 0;          // переменная для хранения координаты Y пикселя
    
    unsigned short temp_data[640*512] = { 0 };      // буфер для хранения температурной матрицы
    unsigned char image_data[1000*1000] = { 0 };    // буфер для хранения RGB-данных снимка. С размером буфера не разобрался

    std::cout << "Version: " << version << std::endl;
    std::cout << "Это драйвер для тепловизора AT61F (производства Infiray)." << std::endl;
    
    if(NeedInit)
    {
        initial(); // Инициализация SDK
    }

    


    
    std::cout << "Проверка подключения к тепловизору..." << std::endl;
    int subresult; // вспомогательная переменная
    subresult = sdk_get_camera_temp(pSdk, &DeviceTemp);
  
    if(subresult == 0)
    {
        std::cout << "\tСигнал от тепловизора есть" << std::endl;
        sdk_get_SN_PN(pSdk, Device_Info, DeviceSN, DevicePN);
        std::cout << "\tS/n=" << DeviceSN << std::endl;
        std::cout << "\tP/n=" << DevicePN << std::endl;
        sdk_get_camera_temp(pSdk, &DeviceTemp);
        std::cout << "\tDeviceTemp: " << DeviceTemp << " C°\n" << std::endl;
    }
    else
    {
        std::cout << "\tСигнала от камеры нет. Завершаемся\n" << std::endl;
        return 0;
    }


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


    

    // !!!!! ЭТО УДАЛИТЬ ПОСЛЕ ТЕСТОВ !!!!!
    //envir_data.airTemp = 25 * 10000;
    //envir_data.emissivity = 0.9 *10000;
    //envir_data.reflectTemp = envir_data.airTemp;
    //envir_data.humidity = 2 * 10000;
    //envir_data.distance = 2 * 10000;
    //sdk_set_envir_param(pSdk, Device_Info, envir_data); // функция для установки подготовленных параметров окружающей среды

    
    


    while(true)
    {
        reinitial();

        SENDPARAM OutputStructData = {0};
        GETPARAM InputStructData = {0};

        std::string TempPath = CapturePath;    // начало формируемого пути для файла (берется из config файла)
        


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

        // Шаг проверки на HTTP
        std::string currentRequest(Receive_Buff, bytes_recv);
        uint8_t httpreq = CheckHTTPRequest(currentRequest);

        
        if(httpreq != 0xFF)
        {
            std::cout << "Это HTTP-запрос. Тип запроса:" << httpreq << std::endl;
            Command = httpreq;
            printf("Тип HTTP-запроса: %02X \n", Command);
        }
        else
        {
            std::cout << "Похоже это не HTTP-запрос. Тип запроса:" << httpreq << std::endl;
            // берем первый байт чтобы узнать тип запроса
            Command = Receive_Buff[0];
            printf("Тип запроса: %02X \n", Command);
        }
       
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

        int Setparams_result; // результат установки параметров (в команде 3)
       

        
 
        switch(Command)
        {
            
            case 1: // СДЕЛАТЬ СНИМОК
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

                    mkdir(TempPath.c_str(), 0777);          // создаем основной каталог если он отсутствует
                    TodayDirName = GetCurrentTimestamp(0);  // готовимся к созданию подкаталога с именем-датой (сегодняшней)
                    TempPath.append(TodayDirName);          // добавляем к основному пути имя подкаталога-дату
                    mkdir(TempPath.c_str(), 0777);          // создаем подкаталог-дату если он отсутствует
                    TempPath.append("/");                   // добавляем заход в этот подкаталог
                    
                    CaptureName.append("_");                     // формируем имя для снимка добавлением к принятому имени 
                    CaptureName.append(GetCurrentTimestamp(2));  // нижнего подчеркивания и текущего времени

                    TempPath.append(CaptureName);
                    std::cout << "Итоговый путь сохранения снимка: " << TempPath << std::endl;

                    // делаем jpeg и irg файл
                    sdk_snapshot(pSdk, Device_Info, 1, (char *) TempPath.c_str());

                    // далее извлекаем из irg файла данные
                    TempPath.append(".irg");
                    sdk_get_irg_data( (char *) TempPath.c_str(), 4, temp_data, image_data);

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

                    std::cout << "\tСнимок сделан, данные считаны" << std::endl;
                    std::cout << "\tCalcTavg=" << CalcTavg<< std::endl;
                    std::cout << "\tCalcTmin=" << CalcTmin << std::endl;
                    std::cout << "\tCalcTmax=" << CalcTmax << std::endl;


                    OutputStructData.average_t = CalcTavg;
                    OutputStructData.min_t = CalcTmin;
                    OutputStructData.max_t = CalcTmax;
                    

                    if(TminmaxFlag == 1)
                    {
                        if((CalcTmax >= SettedTmax) && (CalcTmin <= SettedTmin))
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

                remove(TempPath.c_str());   // удаляем irg-файл, так как уже вытащили из него всю нужную информацию
                Answer_size=14;
                break;   


            case 2: // ЗАПРОС ТЕМПЕРАТУР
                std::cout << "Принята команда на запрос температур" << std::endl;

                sdk_get_temp_data(pSdk, Device_Info, 256, area_temp);

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
                    if((Frame_Tmax >= SettedTmax) && (Frame_Tmin <= SettedTmin))
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
                Answer_size = 14;
                break;


            case 3: // УСТАНОВКА ПАРАМЕТРОВ ОКРУЖАЮЩЕЙ СРЕДЫ И НУЖНЫХ НАМ ТЕМПЕРАТУРНЫХ ПОРОГОВ
                std::cout << "Пришла команда на установку параметров" << std::endl;

                if((bytes_recv) == sizeof(GETPARAM))
                {
                    memcpy(&InputStructData, Receive_Buff, sizeof(InputStructData));
                }
                
                //std::cout << "command: " << InputStructData.cmd << std::endl;
                std::cout << "\tAroundTemp: " << InputStructData.around_temp << std::endl;
                std::cout << "\tDistance: " << InputStructData.distance << std::endl;
                std::cout << "\tMin_t: " << InputStructData.min_t << std::endl;
                std::cout << "\tMax_t: " << InputStructData.max_t << std::endl << std::endl;

                std::cout << "\tУстанавливаем эти значения..." << std::endl;

                SettedTmin = InputStructData.min_t; // наш температурный порог
                SettedTmax = InputStructData.max_t; // наш температурный порог

                // физические параметры окружающей среды
                envir_data.airTemp = InputStructData.around_temp * 10000;
                envir_data.reflectTemp = envir_data.airTemp;
                envir_data.distance = InputStructData.distance * 10000;
                //envir_data.humidity = ;       // установку влажности добавить когда потребуется
                //envir_data.emissivity = ;     // установку излучаемости добавить когда потребуется

                Setparams_result = sdk_set_envir_param(pSdk, Device_Info, envir_data); // функция для установки подготовленных параметров окружающей среды
                if(Setparams_result != -1)
                {
                    std::cout << "\tЗначения установлены: " << Setparams_result <<  std::endl;
                    TminmaxFlag=1; // поднимаем флаг о том что значения установлены
                    OutputStructData.error = 0x00; // флаг для ответа что все хорошо
                }
                else
                {
                    std::cout << "\tНЕУДАЧА. Требуется реинициализация SDK (возможно камера отключалась)" << std::endl;
                    NeedInit=true;
                    OutputStructData.error = 0x03; // ошибка связи с тепловизором
                }
                
                Answer_size = 4;
                break;


            case 4:
                std::cout << "Пришла команда на чтение температуры пикселя" << std::endl;
                CaptureName="temp_pixel";
                std::cout << "Принята команда на чтение температуры пикселя. Сначала делаем снимок, имя для него берем временное: " << CaptureName << std::endl;


                mkdir(TempPath.c_str(), 0777);          // создаем основной каталог если он отсутствует
                TodayDirName = GetCurrentTimestamp(0);  // готовимся к созданию подкаталога с именем-датой (сегодняшней)
                TempPath.append(TodayDirName);          // добавляем к основному пути имя подкаталога-дату
                mkdir(TempPath.c_str(), 0777);          // создаем подкаталог-дату если он отсутствует
                TempPath.append("/");                   // добавляем заход в этот подкаталог

                TempPath.append(CaptureName);
                std::cout << "Итоговый путь сохранения снимка: " << TempPath << std::endl;

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
               
                remove(TempPath.c_str());
                Answer_size=5;
                break;

                
            case 0xF0: // сброс параметров окружающей среды
                std::cout << "Пришел HTTP-запрос на сброс параметров окружающей среды" << std::endl;   
                std::cout << sdk_reset_param(pSdk, Device_Info);
                break;

            case 0xF3: // 
                std::cout << "Пришел HTTP-запрос на конфигурирование тепловизора" << std::endl;   
                ConfigDevice();
                break;


            case 0x91: // принудительная реинициализация
                std::cout << "Принудительная реинициализация...\n" << std::endl;   
                reinitial();
                break;

            case 92:
                sdk_CapSingle(pSdk, Device_Info);
                break;

            case 93: // проверка доступности камеры костылем "считать температуру камеры"
            std::cout << "Проверка камеры..." << std::endl;
                subresult = sdk_get_camera_temp(pSdk, &DeviceTemp);
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
                break;

            
        }


        // ФОРМИРОВАНИЕ ОТВЕТА
        int result = send(exchange_socket, &OutputStructData, Answer_size, 0);
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



*/







// старый метод как я делал, потом через структуры сделали

                /*

                std::array<uint8_t, 14> response_temp_data = {0};



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
    