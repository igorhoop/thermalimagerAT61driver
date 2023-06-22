/*
    ПРЕДМЕТНЫЕ САМОПИСНЫЕ ФУНКЦИИ, КОТОРЫЕ ИСПОЛЬЗУЮТ SDK
*/

#include <fstream>
#include "../include/InfraredTempSDK.h"
#include "../headers/3l_functions.h"
#include <cstdlib>
#include <cstring>




extern IRNETHANDLE pSdk;
extern struct ChannelInfo Device_Info;
extern envir_param envir_data;

extern std::string AirTemp;
extern std::string Emissivity;
extern std::string Humidity;
extern std::string Distance;

extern bool SDK_INIT;

extern int32_t SettedTmin;  
extern int32_t SettedTmax;  
extern bool TminmaxFlag;


// ===== Запрос карты пикселей =====
int GetMapPixel(std::string capture_path, std::array<uint8_t, 327680> &response_temp_data)
{
    unsigned short temp_data[640*512] = { 0 };     // !!! могут быть минусовые, поправить тип. буфер для хранения температурной матрицы 
    unsigned char image_data[1000*1000] = { 0 };    // буфер для хранения RGB-данных снимка. С размером буфера не разобрался

    std::cout << "Пришла команда на массив всех пикселей" << std::endl;
    
    std::string FullPath = capture_path;    // начало формируемого пути для файла (берется из config файла)
    std::string FullPathJpg;

    std::string CaptureName="temp_pixelmap_file";

    mkdir(FullPath.c_str(), 0777);          // создаем основной каталог если он отсутствует
    std::string TodayDirName = GetCurrentTimestamp(0);  // готовимся к созданию подкаталога с именем-датой (сегодняшней)
    FullPath.append(TodayDirName);          // добавляем к основному пути имя подкаталога-дату
    mkdir(FullPath.c_str(), 0777);          // создаем подкаталог-дату если он отсутствует
    FullPath.append("/");                   // добавляем заход в этот подкаталог

    FullPath.append(CaptureName);
    std::cout << "Полный путь сохранения временного пиксельного файла: " << FullPath << std::endl;
    
    if(!PingDevice()) // поэтому перед вызовом проверяем связь с устройством
    {
        // делаем jpeg и irg файл
        sdk_snapshot(pSdk, Device_Info, 1, (char *) FullPath.c_str());

        // сразу удаляем JPG, ведь оно нас сейчас не интересует
        FullPathJpg = FullPath;
        FullPathJpg.append(".jpg");
        remove(FullPathJpg.c_str());
        

        // далее извлекаем из irg файла данные
        FullPath.append(".irg");
        sdk_get_irg_data( (char *) FullPath.c_str(), 4, temp_data, image_data);
        
        for(int i=0; i < 327680; i++)
        {
            response_temp_data[i] = temp_data[i]/10 - 273.2;
        }


        std::cout << "\tМассив подготовлен" << std::endl;
        remove(FullPath.c_str()); // удаляем irg файл

        return 0;
    }
    else
    {
            std::cout << "\tОбратно отправлять будем с ошибкой: нет связи с устройством" << std::endl;
            remove(FullPath.c_str());
            return 1;                 
    }
    


    return 0;
}


// ===== Запрос температуры пикселя =====
int GetTemperaturePixel(std::string capture_path, int32_t x, int32_t y, struct SENDPARAM * OutputData)
{
    int32_t CalcTavg = 0;
    int32_t CalcTmax = 0;
    int32_t CalcTmin = 0;
    int32_t CalcTpix = 0;

    // переменная в которую запишем номер нужного пикселя, потребуется при запросе температуры пикселя по X и Y
    int NumPixel = 0;

    unsigned short temp_data[640*512] = { 0 };     // !!! могут быть минусовые, поправить тип. буфер для хранения температурной матрицы 
    unsigned char image_data[1000*1000] = { 0 };    // буфер для хранения RGB-данных снимка. С размером буфера не разобрался

    std::cout << "Пришла команда на чтение температуры пикселя" << std::endl;

    std::string FullPath = capture_path;    // начало формируемого пути для файла (берется из config файла)
    std::string CaptureName="temp_file";
    std::string TodayDirName = ""; 

    mkdir(FullPath.c_str(), 0777);          // создаем основной каталог если он отсутствует
    TodayDirName = GetCurrentTimestamp(0);  // готовимся к созданию подкаталога с именем-датой (сегодняшней)
    FullPath.append(TodayDirName);          // добавляем к основному пути имя подкаталога-дату
    mkdir(FullPath.c_str(), 0777);          // создаем подкаталог-дату если он отсутствует
    FullPath.append("/");                   // добавляем заход в этот подкаталог

    FullPath.append(CaptureName);
    std::cout << "Полный путь сохранения временного файла: " << FullPath << std::endl;

    if(!PingDevice()) // поэтому перед вызовом проверяем связь с устройством
    {

        // делаем jpeg и irg файл
        sdk_snapshot(pSdk, Device_Info, 1, (char *) FullPath.c_str());

        // далее извлекаем из irg файла данные
        FullPath.append(".irg");
        sdk_get_irg_data( (char *) FullPath.c_str(), 4, temp_data, image_data);

        CalcTpix = temp_data[0];



        std::cout << "\tКоордината x=" << x << std::endl;
        std::cout << "\tКоордината y=" << y << std::endl;

        NumPixel = (y * 640) + x;

        CalcTpix = temp_data[NumPixel];

        CalcTpix = (CalcTpix)/10-273.2; // посчитанное значение температуры пикселя


        std::cout << "\tCalcTpix=" << CalcTpix << std::endl;
        OutputData->average_t = CalcTpix;

        if((x > 640) || (y > 512))
        {
            OutputData->error = 0x01;
        }
    }
    else
    {
            OutputData->error = 0x05;
            std::cout << "\tОбратно отправлять будем с ошибкой: нет связи с устройством" << std::endl;
    }
    remove(FullPath.c_str());

}


// ===== Установка температурных порогов =====
int SetTemperatureLimit(std::string config_path, int32_t tmin, int32_t tmax)
{
    std::cout << "Пришла команда на установку температурных порогов" << std::endl;

    std::cout << "\tMin_t: " << tmin << std::endl;
    std::cout << "\tMax_t: " << tmax << std::endl << std::endl;

    std::cout << "\tУстанавливаем эти пороги..." << std::endl;    

    RewriteFileContent(config_path, "Tmin=",  std::to_string(tmin));
    RewriteFileContent(config_path, "Tmax=",  std::to_string(tmax));
    ReinitialAndConnect();
    ConfigDevice();

    return 0;
}


// ===== Установка излучаемости и влажности =====
int SetEmissivityHumidity(std::string config_path, float Emissivity, float Humidity)
{
    std::cout << "Пришла команда на установку излучаемости и влажности" << std::endl;
    std::cout << "\tEmissivity: " << Emissivity << std::endl;
    std::cout << "\tHumidity: " << Humidity << std::endl;
    std::cout << "\tУстанавливаем эти значения..." << std::endl;

    RewriteFileContent(config_path, "Emissivity=", std::to_string(Emissivity));
    RewriteFileContent(config_path, "Humidity=",  std::to_string(Humidity));
    ReinitialAndConnect();
    ConfigDevice();

    return 0;
}


// ===== Установка дистанции =====
int SetDistance(std::string config_path, int8_t Distance)
{
    std::cout << "Пришла команда на установку дистанции" << std::endl;
    std::cout << "\tDistance: " << (int) Distance << std::endl;
    std::cout << "\tУстанавливаем это значение..." << std::endl;

    RewriteFileContent(config_path, "Distance=", std::to_string(Distance));
    ReinitialAndConnect();
    ConfigDevice();

    return 0;
}



// ===== Установка температуры окружающей среды =====
int SetAirTemp(std::string config_path, int8_t AirTemp)
{
    std::cout << "Пришла команда на установку температуры окружающей среды" << std::endl;
    std::cout << "\tAirTemp: " << (int) AirTemp << std::endl;
    std::cout << "\tУстанавливаем это значение..." << std::endl;

    RewriteFileContent(config_path, "AirTemp=", std::to_string(AirTemp));
    
    ReinitialAndConnect();
    ConfigDevice();

    return 0;
}




void * PingDeviceThread(void * args)
{
    int vsp = 0;
    while(SDK_INIT==false) // первичная инициализация
    {
        InitialSDK(); // Инициализация SDK

        while(DeviceConnect()) 
        {
            sleep(5);
        }
        ConfigDevice();
        SDK_INIT=true;
    }

    Area_Temp local_area_temp;
    while(1)
    {
        vsp = PingDevice();
        if(vsp == 0)
        {
            std::cout << "\tСвязь с устройством: ОК" << std::endl;
            sleep(10);
        }
        else 
        {
            std::cout << "\tСвязи нет. Пытаемся наладить..." << std::endl;
            WaitDevice();
            ReinitialAndConnect();
            ConfigDevice();
        }
    }
}


int WaitDevice()
{
    std::cerr << "\tОжидание ответа устройства..." << std::endl;
    Area_Temp test_area_temp;
    while(sdk_get_temp_data(pSdk, Device_Info, 256, test_area_temp)!=0)
    {
        std::cout << "\tОжидание ответа устройства..." << std::endl;
        sleep(3);
    }

    return 0;
}


// === Проверка доступности тепловизора ===
int PingDevice()
{
    float DeviceTemp;
    if(sdk_get_camera_temp(pSdk, &DeviceTemp)==0)
    {
        std::cout << "\tпинг есть" << std::endl;
        return 0;
    }
    else
    {
        std::cout << "\tпинга нет" << std::endl;
        return 1;
    }
}


// === Получение информации о тепловизоре ===
int GetDeviceInfo()
{
    float DeviceTemp;       // сюда положим температуру тепловизора
    char DeviceSN[30];      // массив для серийного номера
    char DevicePN[50];      // массив для part-номера

    std::cout << "Получаем информацию об устройстве..." << std::endl;
    if(sdk_get_camera_temp(pSdk, &DeviceTemp)==0)
    {
        std::cout << "\tOK. Устройство на связи" << std::endl;
        sdk_get_SN_PN(pSdk, Device_Info, DeviceSN, DevicePN);
        std::cout << "\tS/n=" << DeviceSN << std::endl;
        std::cout << "\tP/n=" << DevicePN << std::endl;
        sdk_get_camera_temp(pSdk, &DeviceTemp);
        std::cout << "\tDeviceTemp: " << DeviceTemp << " C°\n" << std::endl;
    }
    else
    {
        std::cout << "\tFAIL. Устройство не отвечает" << std::endl;
        return 1;
    }

    return 0;
}



// === Конфигурирование тепловизора ===
void ConfigDevice()
{
    /*
        после отключения тепловизора по питанию слетают следующие вещи:
            v: Формат снимка нужно нужно перезадать, иначе jpeg и irg файлы создаваться не будут,
                при этом из SDK будут выдаваться последние данные считанные из irg 
            v: Цветовую палитру нужно перезадать: но выдержать перед этим паузу?
            
            v: Параметры окружающей среды
        
            Единицы измерения (цельсии) ??? под вопросом
    */
    std::cout << "Конфигурирование устройства..." << std::endl;
    // установка формата снимков, будет создаваться оба файла: jpg и irg. ДОЛЖЕН БЫТЬ ВКЛЮЧЕН ТЕПЛОВИЗОР, иначе подвиснет
    int res = sdk_set_capture_format(pSdk, Device_Info, 4);
    res==0?std::cout << "\tУстановка формата снимков: ОК":std::cout << "\tУстановка формата снимков: FAIL";

    while(sdk_set_color_plate(pSdk, Device_Info, 2) != 0)    // установка цветовой гаммы
    {
        std::cout << "Попытка установить палитру" << std::endl;
        sleep(1);
    }
    std::cout << "\tУстановка палитры: ОК" << std::endl;


    //while(sdk_set_temp_unit(pSdk, Device_Info, 0) != 0)    // установка единиц измерения
    //{
    //    std::cout << "Попытка установить единицы измерения" << std::endl;
    //    sleep(1);
    //}
    //std::cout << "\tУстановка единиц измерения: ОК" << std::endl;

    Time_Param timeData;
    time_t now_time=time(NULL);
    tm*  t_tm = localtime(&now_time);
    timeData.m_year = t_tm->tm_year+1900;
    timeData.m_month = t_tm->tm_mon + 1;
    timeData.m_dayofmonth = t_tm->tm_mday;
    timeData.m_hour = t_tm->tm_hour;
    timeData.m_minute = t_tm->tm_min;
    timeData.m_second = t_tm->tm_sec;


    sdk_synchronised_time(pSdk, Device_Info, timeData);


    // Установка отрисовки экранных измерений
    const char * Watermark = "3Logic Group Robotic Systems";
    Custom_String osdContent;
    osdContent.iFormat = 2;
    osdContent.iFormatTime = 5;
    osdContent.iShow= 1;
    osdContent.iIndex = 3;
    std::memcpy(osdContent.m_szString, Watermark, sizeof(osdContent.m_szString));
    osdContent.iWidth = 100;
    osdContent.iDeviceWidth = 640;
    osdContent.iDeviceHeight = 512;
    osdContent.iX = 5;
    osdContent.iY = 50;
    osdContent.iStringX = 5;
    osdContent.iStringY = 490;
    sdk_set_osd_display(pSdk, Device_Info, osdContent);


    SetEnvirParams();

}



// === Реинициализация SDK ===
void ReinitialAndConnect()
{
        std::cout << "Реинициализация SDK и переконнект..." << std::endl;
        sdk_release(pSdk);
        InitialSDK();
        DeviceConnect();
}


// === Инициализация SDK, подключение к тепловизору, его настройка ===
void InitialSDK() // здесь не должны быть функции SDK, которые могут подвиснуть программу
{
    std::cout << "Логинимся в SDK..." << std::endl;

    int res = 999; // вспомогательная переменная
    
    // шаг 1. Установка типа девайса (нах здесь логин и пароль?). Похоже на бесполезное действие
    char UserName[30] = "888888";
    char UserPass[30] = "888888";
    sdk_set_type(0, UserName, UserPass); // ничего не возвращает. Зачем здесь юзер и пароль неясно
    
    // шаг 2. Инициализация SDK
    res = sdk_initialize();         // инициализация SDK. Вернет 0 при успешном результате
    res == 0 ? std::cout << " \tИнициализация SDK: ОК " << std::endl : std::cout << " \tИнициализация SDK: FAIL " << std::endl;

    // шаг 3. Создание дескриптора SDK
    pSdk = sdk_create();
    pSdk != NULL ? std::cout << "\tСоздание дескриптора SDK - ОК\n" << std::endl : std::cout << "\tСоздание дескриптора SDK: FAIL\n" << std::endl;

    // шаг 4. логин в устройство. Хотя скорее это применение параметров подключения к устройству для SDK (то есть SDK теперь будет знать куда стучаться)
    std::string ConfigPath;
    if(getenv("AT61F_CONFIG_PATH")==NULL)
    {
        std::cout << "Не задана переменная среды с путем расположения config-файла. Завершение работы" << std::endl;
        exit(1);
    }
    else
    {
        ConfigPath = getenv("AT61F_CONFIG_PATH");
    }
    
    GetConfigForConnectCAM(ConfigPath); // сначала заполненяем структуру ChannelInfo исходя из config-файла: IP, порт, логин, пароль
    int isLogin = (sdk_loginDevice(pSdk, Device_Info) == 0);
    std::cout << (isLogin?"\tПрименение параметров к SDK - ОК\n":"Применение параметров к SDK - FAIL\n") << std::endl;
    if(!isLogin)
        exit(1);
    
    std::cout << "Инициализация SDK завершена" << std::endl;

}


int DeviceConnect()
{
    std::cout << "Попытка подключения к устройству..." << std::endl;
    if(WaitDevice()==0)
    {
        std::cout << "\tСвязь ОК. Устанавливаем функции-обработчики..." << std::endl;
        // после инициализации требуется установить функции-обработчики (callback'и). Настройка относящайся к SDK, а значит не подвиснет программу при выключенном устройстве
        int res = SetSerialCallBack(pSdk, Device_Info, SerialCallBackMy, NULL);   // установка обработчика приема серийных данных
        std::cout << "\tУстановка функций обработчиков: " << res << std::endl;

        return 0;
    }
    else
    {
        std::cout << "Нет пинга. Пробовать подключиться не будем" << std::endl;
        return 1;
    }
}







int SetTempLimit(int32_t tmin, int32_t tmax)
{
    std::cout << "\tУстановка температурных порогов..." << std::endl;  
    SettedTmin = tmin; // наш температурный порог
    SettedTmax = tmax; // наш температурный порог
    TminmaxFlag=true;                          // поднимаем флаг о том что значения установлены
    return 0;
}


int SetEnvirParams()
{
    std::cout << "Устанавливаем параметры окружающей среды" << std::endl;
    envir_param envir_data;         //структура для установки физических параметров (окружающей среды)

    envir_data.airTemp = strtof(AirTemp.c_str(), nullptr) * 10000;
    envir_data.emissivity = strtof(Emissivity.c_str(), nullptr) * 10000;
    envir_data.reflectTemp = envir_data.airTemp;
    envir_data.humidity = strtof(Humidity.c_str(), nullptr) * 10000;
    envir_data.distance = strtof(Distance.c_str(), nullptr) * 10000;

    if(sdk_set_envir_param(pSdk, Device_Info, envir_data)==-1)
    {
        std::cout << "Не получилось установить параметры окружающей среды" << std::endl;
        return 1;
    }
        std::cout << "Установка параметров окружающей среды: ОК" << std::endl;


    return 0;
}












// ===== Чтение файла конфигурации и заполнение структуры ChannelInfo =====
int GetConfigForConnectCAM(std::string path)
{    
    std::string config = GetContentFromFile(path);
    int vsp1 = (int) config.find("ip=");
    int vsp2 = (int) config.find(";");
    std::string host = config.substr(vsp1+3, vsp2-vsp1-3);

    vsp1 = (int) config.find("port=");
    vsp2 = (int) config.find(";", vsp2+1);
    std::string port = config.substr(vsp1+5, vsp2-vsp1-5);

    vsp1 = (int) config.find("login=");
    vsp2 = (int) config.find(";", vsp2+1);
    std::string login = config.substr(vsp1+6, vsp2-vsp1-6);  

    vsp1 = (int) config.find("pass=");
    vsp2 = (int) config.find(";", vsp2+1);
    std::string pass = config.substr(vsp1+5, vsp2-vsp1-5);




    vsp1 = (int) config.find("AirTemp=");
    vsp2 = (int) config.find(";", vsp2+1);
    AirTemp = config.substr(vsp1+8, vsp2-vsp1-8);

    vsp1 = (int) config.find("Emissivity=");
    vsp2 = (int) config.find(";", vsp2+1);
    Emissivity = config.substr(vsp1+11, vsp2-vsp1-11);

    //vsp1 = (int) config.find("ReflectTemp=");
    //vsp2 = (int) config.find(";", vsp2+1);
    //ReflectTemp = config.substr(vsp1+12, vsp2-vsp1-12);

    vsp1 = (int) config.find("Humidity=");
    vsp2 = (int) config.find(";", vsp2+1);
    Humidity = config.substr(vsp1+9, vsp2-vsp1-9);

    vsp1 = (int) config.find("Distance=");
    vsp2 = (int) config.find(";", vsp2+1);
    Distance = config.substr(vsp1+9, vsp2-vsp1-9);

    vsp1 = (int) config.find("Tmin=");
    vsp2 = (int) config.find(";", vsp2+1);
    std::string tmin = config.substr(vsp1+5, vsp2-vsp1-5);

    vsp1 = (int) config.find("Tmax=");
    vsp2 = (int) config.find(";", vsp2+1);
    std::string tmax = config.substr(vsp1+5, vsp2-vsp1-5);

    


    std::cout << "Чтение config-файла..." << std::endl;
    std::cout << "\tIP: " << host << std::endl;
    std::cout << "\tPORT: " << port << std::endl;
    std::cout << "\tLOGIN: " << login << std::endl;
    std::cout << "\tPASS: " << pass << std::endl;

    std::cout << "Указанные параметры окружающей среды..." << std::endl;
    std::cout << "\tAirTemp: " << AirTemp << std::endl;
    std::cout << "\tEmissivity: " << Emissivity << std::endl;
    //std::cout << "\tReflectTemp: " << ReflectTemp << std::endl;
    std::cout << "\tHumidity: " << Humidity << std::endl;
    std::cout << "\tDistance: " << Distance << std::endl;

    std::cout << "\tTmin: " << tmin << std::endl;
    std::cout << "\tTmax: " << tmax << std::endl;

    // заполнение данных для подключения
    strcpy_s(Device_Info.szServerName, "AT61F-CAM");
    strcpy_s(Device_Info.szIP, host.c_str());
    strcpy_s(Device_Info.szUserName, login.c_str());
    strcpy_s(Device_Info.szPWD, pass.c_str());
    Device_Info.wPortNum = atoi(port.c_str());
    Device_Info.channel = 0; // что за канал, одному богу известно

    SetTempLimit(atoi(tmin.c_str()), atoi(tmax.c_str()));

    return 0;
}









// ===== Запрос температур в кадре: MIN, MAX, AVG =====
int RequestTemperatures(struct SENDPARAM * OutputData)
{
    Area_Temp area_temp = { 0 };    // структура куда SDK по запросу будет класть температурные данные текущего кадра

    // переменные в которые запишем значения температур, полученных из специальной SDK-шной структуры 
    int32_t Frame_Tavg = 0;
    int32_t Frame_Tmax = 0;
    int32_t Frame_Tmin = 0;

    int vsp_res;

    std::cout << "Принята команда на запрос температур" << std::endl;

    if(!PingDevice())
    {   
        // эта функция подвисает программу. Segfault не дает.
        vsp_res = sdk_get_temp_data(pSdk, Device_Info, 256, area_temp);

        Frame_Tavg = area_temp.iTempAvg/10;
        Frame_Tmax = area_temp.iTempMax/10;
        Frame_Tmin = area_temp.iTempMin/10;

        OutputData->average_t = Frame_Tavg;
        OutputData->min_t = Frame_Tmin;
        OutputData->max_t = Frame_Tmax;
        
        std::cout << "TempAvg: " << Frame_Tavg << std::endl;  
        std::cout << "TempMin: " <<  Frame_Tmin << std::endl;             
        std::cout << "TempMax: " << Frame_Tmax << std::endl;

        if(TminmaxFlag == 1)
        {
            if((Frame_Tmax >= SettedTmax) || (Frame_Tmin <= SettedTmin))
            {
                OutputData->signal = 0x01;
                std::cout << "\tЗафиксировано превышение допустимой температуры. Формируем сигнал" << std::endl;
            }
                
        }
        else
        {
            OutputData->error = 0x01;
            std::cout << "\tОбратно отправлять будем с ошибкой, так как не выставлены температурные пороги" << std::endl;
        } 
    }
    else
    {
        std::cout << "Нет связи с тепловизором, возвращаем ошибку" << std::endl;
        OutputData->error = 0x05;
    }

    return 0;
}





// ===== Формирование снимка =====
int MakeCapture(std::string capture_path, std::string capture_name, struct SENDPARAM * OutputData)
{
    int vsp_res;                        // вспомогательная

    int32_t CalcTavg = 0;       // подсчитанное вручную значение средней температуры в кадре
    int32_t CalcTmax = 0;       // подсчитанное вручную значение максимальной температуры в кадре
    int32_t CalcTmin = 0;       // подсчитанное вручную значение минимальной температуры в кадре

    unsigned short temp_data[640*512] = { 0 };     // !!! могут быть минусовые, поправить тип. буфер для хранения температурной матрицы 
    unsigned char image_data[1000*1000] = { 0 };    // буфер для хранения RGB-данных снимка. С размером буфера не разобрался

    std::string FullPath = capture_path;    // формируемый конечный путь для снимка
    std::string CaptureName = capture_name; // формируемое конечное имя снимка
    std::string TodayDirName = "";          // сюда будет класть имя сегодняшней директории-даты     

    std::cout << "Принята команда на формирование снимка" << std::endl;

    
    if((CaptureName.length() < 3) || (CaptureName.length() > 39))
    {
        std::cout << "Неподходящая длина имени снимка" << std::endl;
        OutputData->error = 0x02;
    }
    else
    {
        std::cout << "\tИмя для снимка: " << CaptureName << std::endl;

        mkdir(FullPath.c_str(), 0777);          // создаем основной каталог если он отсутствует
        TodayDirName = GetCurrentTimestamp(0);  // готовимся к созданию подкаталога с именем-датой (сегодняшней)
        FullPath.append(TodayDirName);          // добавляем к основному пути имя подкаталога-дату
        mkdir(FullPath.c_str(), 0777);          // создаем подкаталог-дату если он отсутствует
        FullPath.append("/");                   // добавляем заход в этот подкаталог
        
        CaptureName.append("_");                     // формируем имя для снимка добавлением к принятому имени 
        CaptureName.append(GetCurrentTimestamp(2));  // нижнего подчеркивания и текущего времени

        FullPath.append(CaptureName);
        std::cout << "\tПолный путь сохранения снимка: " << FullPath << std::endl;

        if(!PingDevice()) // поэтому перед вызовом проверяем связь с устройством
        {
            // Создаем jpeg и irg файл. Эта функция выкинет segfault если вызвать ее при отключенном тепловизоре
            // При этом если не выполнена конфигурация, то все равно вернет УСПЕХ, не создав файлов. Просто говно-SDK
            sdk_snapshot(pSdk, Device_Info, 1, (char *) FullPath.c_str());

            // далее извлекаем из irg файла данные. По наличию jpeg и irg файлов ориентируемся на эту функцию, так как sdk_snapshot() ни о чем
            FullPath.append(".irg");
            vsp_res = sdk_get_irg_data( (char *) FullPath.c_str(), 4, temp_data, image_data);
            if(vsp_res == -1)
            {
                std::cout << "\tНе получилось создать файлы: jpeg и irg. Скорее всего не выполнена конфигурация" << std::endl;
                OutputData->error = 0x04;
            }
            else
            {
                std::cout << "\tСоздание файлов: jpeg и irg - ОК. Вытаскиваем данные..." << std::endl;

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

                OutputData->average_t = CalcTavg;
                OutputData->min_t = CalcTmin;
                OutputData->max_t = CalcTmax;

                if(TminmaxFlag == true)
                {
                    if((CalcTmax >= SettedTmax) || (CalcTmin <= SettedTmin))
                        {
                            OutputData->signal = 0x01;
                            std::cout << "\tЗафиксировано превышение допустимой температуры. Формируем сигнал" << std::endl;
                        }
                }
                else
                {
                    OutputData->error = 0x01;
                    std::cout << "\tОбратно отправлять будем с ошибкой, так как не выставлены температурные пороги" << std::endl;
                }
            }
        }
        else
        {
            OutputData->error = 0x05;
            std::cout << "\tОбратно отправлять будем с ошибкой: нет связи с устройством" << std::endl;
        }
    }

    remove(FullPath.c_str());   // удаляем irg-файл, так как уже вытащили из него всю нужную информацию
    return 0;
}