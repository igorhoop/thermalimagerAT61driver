/*
    БАЗИСНЫЕ (ОПОРНЫЕ) САМОПИСНЫЕ ФУНКЦИИ, НЕ СВЯЗАННЫЕ С SDK
*/

#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <iostream>
#include "../headers/3l_functions.h"







// ===== ЗАПИСЬ ЛОГА В ФАЙЛ =====
int LogWrite(std::string logfile_path, std::string log_text)
{
    std::ofstream file(logfile_path.c_str(), std::ios::app);
    file << log_text;
}



// ===== Замена значения в файле =====
int RewriteFileContent(std::string file_path, std::string option_name, std::string value)
{   
    int size = option_name.length();   
    std::string content = GetContentFromFile(file_path.c_str());
    int vsp1;
    int vsp2;
    vsp1 = (int) content.find(option_name);
    if(vsp1 == -1)
        return 1; // опция не найдена
    vsp2 = (int) content.find(";", vsp1);
    content.erase(vsp1+size, vsp2-vsp1-size);
    content.insert(vsp1+size, value);

    std::ofstream file(file_path.c_str());
    file << content;
    
    return 0;
}





// возможно требуется переписать чтобы возвращал результат при неудаче
// ===== Взять содержимое файла =====
std::string GetContentFromFile(const std::string & filePath) // функция от Алертера
{
    std::ifstream f1;
    std::vector<char> buf(2024, 0);
    f1.open(filePath, std::ios::in);
    if(!f1.is_open())
    {
        std::cout << "Не найден конфигурационный файл" << std::endl;
        exit(1);
    }
    f1.read(&buf[0], buf.size());
    return buf.data();
}





// ===== Получить текущее время в определенном формате: 0 - дата, 1 - дата и время, 2 - время =====
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