#include <fstream>
#include "include/InfraredTempSDK.h"
#include "headers/3l_functions.h"
#include <ctime>
#include <cstdlib>
#include <cstring>


extern IRNETHANDLE pSdk;
extern struct ChannelInfo Device_Info;
extern std::string CapturePath;






// =========== ПРОВЕРКА HTTP-ЗАПРОСОВ  ===========
uint8_t CheckHTTPRequest(std::string request)
{
    std::string find_substring;     // для поиска подстрок

    // проверка что пришел запрос на MONITOR
    find_substring = "GET /reset_envir";
    auto position = request.find(find_substring);

    if(position != std::string::npos)
    {     
        return 0xF0; // сброс значений окружающей среды, по умолчанию: AirTemp 25, Emissivity 1, ReflectTemp 25, Humidity 1, Distance 2   
    }

    // запрос на установку конфигурационных параметров тепловизора
    find_substring = "GET /set_params";
    position = request.find(find_substring);
    if(position != std::string::npos)
    {     
        return 0xF3; 
    }

    return 0xFF; // если ничего не подошло
}


void * PingDeviceThread(void * args)
{
    int vsp = 0;
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
        }
    }
}


int WaitDevice()
{
    Area_Temp test_area_temp;
    while(sdk_get_temp_data(pSdk, Device_Info, 256, test_area_temp)!=0)
    {
        std::cout << "\tОжидание ответа устройства..." << std::endl;
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


    sdk_set_color_plate(pSdk, Device_Info, 2);    // установка цветовой гаммы

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



    //envir_data.airTemp = 25 * 10000;
    //envir_data.emissivity = 0.9 *10000;
    //envir_data.reflectTemp = envir_data.airTemp;
    //envir_data.humidity = 2 * 10000;
    //envir_data.distance = 2 * 10000;
    //sdk_set_envir_param(pSdk, Device_Info, envir_data); // функция для установки подготовленных параметров окружающей среды

       // установка формата снимков, будет создаваться оба файла: jpg и irg. ДОЛЖЕН БЫТЬ ВКЛЮЧЕН ТЕПЛОВИЗОР, иначе подвиснет
    int res = sdk_set_capture_format(pSdk, Device_Info, 4);
    std::cout << "\tУстановка формата снимков: " << res << std::endl;
    

    sdk_set_temp_unit(pSdk, Device_Info, 0);


}



// === Реинициализация SDK ===
void ReinitialAndConnect()
{
    //if(NeedInit) // реиницициализация SDK в случае сбоев (отключение питания камеры)
    //{
        std::cout << "Реинициализация SDK и переподключение..." << std::endl;
        sdk_release(pSdk);
        InitialSDK();
        DeviceConnect();
    //}

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
    GetConfigForConnectCAM("config"); // сначала заполненяем структуру ChannelInfo исходя из config-файла: IP, порт, логин, пароль
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





// чтение файла конфига для подключения к камере и частичное заполнение структуры ChannelInfo
void GetConfigForConnectCAM(std::string path)
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

    vsp1 = (int) config.find("CapturePath=");
    vsp2 = (int) config.find(";", vsp2+1);
    CapturePath = config.substr(vsp1+12, vsp2-vsp1-12);




    std::cout << "Читаем данные подключения к камере..." << std::endl;
    std::cout << "\tIP: " << host << std::endl;
    std::cout << "\tPORT: " << port << std::endl;
    std::cout << "\tLOGIN: " << login << std::endl;
    std::cout << "\tPASS: " << pass << std::endl;

    std::cout << "\n\n\tПуть для сохранения снимков:" << CapturePath <<  std::endl;

    // заполнение данных для подключения
    strcpy_s(Device_Info.szServerName, "AT61F-CAM");
    strcpy_s(Device_Info.szIP, host.c_str());
    strcpy_s(Device_Info.szUserName, login.c_str());
    strcpy_s(Device_Info.szPWD, pass.c_str());
    Device_Info.wPortNum = atoi(port.c_str());
    Device_Info.channel = 0; // что за канал, одному богу известно
}




// =========== ВЗЯТЬ СОДЕРЖИМОЕ ФАЙЛА ===========
std::string GetContentFromFile(const std::string & filePath) // функция от Алертера
{
    std::ifstream f1;
    std::vector<char> buf(2024, 0);
    
    f1.open(filePath, std::ios::in);
    f1.read(&buf[0], buf.size());

    return buf.data();
}


// функция для работы со временем. Форматы: 0 - выдача строки с датой, 1 - выдача строки с датой и временем, 2 - чисто время
std::string GetCurrentTimestamp(int format)
{
    char date_format[20];

    std::time_t unixtimestamp = time(0);
    tm * ltm = localtime(&unixtimestamp);

    switch(format)
    {
        case 0:
            strftime(date_format, sizeof(date_format), "%Y-%m-%d", ltm);
            break;
        case 1:
            strftime(date_format, sizeof(date_format), "%Y-%m-%d_%H:%M:%S", ltm);
            break;
        case 2:
            strftime(date_format, sizeof(date_format), "%H:%M:%S", ltm);
            break;
    }

    std::string date_string = date_format;
    return date_string;   
}

