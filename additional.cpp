#include <fstream>
#include "include/InfraredTempSDK.h"
#include "headers/3l_functions.h"
#include <ctime>
#include <cstdlib>


extern IRNETHANDLE pSdk;
extern struct ChannelInfo Device_Info;
extern std::string CapturePath;

// === Инициализация SDK и подключение к тевловизору ===
void initial()
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