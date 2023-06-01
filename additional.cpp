#include <fstream>
#include "include/InfraredTempSDK.h"
#include "headers/myfunctions.h"
#include <ctime>





extern IRNETHANDLE pSdk;
extern struct ChannelInfo Chan_Info;

void initial()
{
    std::cout << "Логинимся в SDK..." << std::endl;

    // на этом шаге похоже на бесполезное действие
    int res = 999; 
    char UserName[30] = "888888";
    char UserPass[30] = "888888";
    
    // шаг 1. Установка типа девайса (нах здесь логин и пароль?)
    sdk_set_type(0, UserName, UserPass); // ничего не возвращает. Зачем здесь юзер и пароль неясно
    
    // шаг 2. Инициализация SDK
    res = sdk_initialize();         // инициализация SDK. Вернет 0 при успешном результате
    std::cout << " \tИнициализация SDK: " << res << std::endl;

    // шаг 3. Создание дескриптора SDK
    pSdk = sdk_create();
    if (pSdk == NULL)
    {
        std::cout << "\tНеудачное создание SDK :(" << std::endl;
    }
    else
    {
        std::cout << "\tСоздание SDK - успешно :)" << std::endl << std::endl;
    }

    // шаг 4. непосредственно логин в устройство
    Chan_Info.channel = 0;
    Chan_Info.wPortNum = 3000;
 
    // заполнение ChannelInfo исходя из файла: ip, логин, пароль
    GetConfigForConnectCAM("config");
    
    
    // применение параметров подключения для SDK (то есть SDK теперь будет знать куда стучаться)
    int isLogin = (sdk_loginDevice(pSdk, Chan_Info) == 0);
    std::cout << (isLogin?"\tПрименение параметров к SDK - ОК":"Применение параметров к SDK - Fail :(") << std::endl;
    if(!isLogin)
        exit(1);
}



// =========== ОПРЕДЕЛЕНИЕ ТИПА ЗАПРОСА ===========
int GetTypeOfRequest(std::string request)
{
    int justvar;                    // для вычислений
    std::string find_substring;     // для поиска подстрок

    find_substring = "command_";
    int vsp1;
    int vsp2;
    vsp1 = request.find(find_substring);
    vsp2 = request.find(".");

    std::string commandStr = request.substr(vsp1+find_substring.size(), vsp2 - (vsp1+find_substring.size()));
    std::cout << "Пришла команда: " << commandStr << std::endl;

    if(commandStr == "2")
        return 2;

    if(commandStr == "3")
        return 3;

    if(commandStr == "5")
        return 5;

    return 999; // если ничего не подошло
}






// чтение файла конфига для подключения к камере и частичное заполнение структуры ChannelInfo
void GetConfigForConnectCAM(std::string path)
{    
    std::string config = GetContentFromFile(path);
    int vsp1 = (int) config.find("ip=");
    int vsp2 = (int) config.find(";");
    std::string host = config.substr(vsp1+3, vsp2-vsp1-3);

    vsp1 = (int) config.find("login=");
    vsp2 = (int) config.find(";", vsp2+1);
    std::string login = config.substr(vsp1+6, vsp2-vsp1-6);  

    vsp1 = (int) config.find("pass=");
    vsp2 = (int) config.find(";", vsp2+1);
    std::string pass = config.substr(vsp1+5, vsp2-vsp1-5);  

    std::cout << "Читаем данные подключения к камере..." << std::endl;
    std::cout << "\tIP: " << host << std::endl;
    std::cout << "\tLOGIN: " << login << std::endl;
    std::cout << "\tPASS: " << pass << std::endl;

    // заполнение данных для подключения
    strcpy_s(Chan_Info.szServerName, "AT61F-CAM");
    strcpy_s(Chan_Info.szIP, host.c_str());
    strcpy_s(Chan_Info.szUserName, login.c_str());
    strcpy_s(Chan_Info.szPWD, pass.c_str());

    
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



// функция для работы со временем. Форматы: 0 - выдача строки с датой, 1 - выдача строки с датой и временем
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
            strftime(date_format, sizeof(date_format), "%Y-%m-%d %H:%M:%S", ltm);
            break;
    }

    std::string date_string = date_format;
    
    return date_string;

    /*
    обратную процедуру скорее всего будем делать так
    SELECT extract(epoch from now() at time zone 'utc');
    и получать сразу UNIX таймстамп


    работало так EXTRACT(EPOCH FROM keys.activationdate)


    // делаем запрос на чтение новой даты активации для ключа--
    sprintf(temp_query, "SELECT EXTRACT(EPOCH FROM keys.activationdate) FROM keys WHERE activationkey='%s'", product_code.c_str());


    // редактирование строки даты
    
    
    std::cout << "Текущий час: " << ltm->tm_hour << std::endl;

    */
    
}






/*






int counter = 0;
int snap_counter = 0;







void VideoCallBackReceiveMy(char *pBuffer, long BufferLen, int width, int height, void* pContext)
{
    
}


char RecieveBuff[500] = { 0 };
int buflen;
*/