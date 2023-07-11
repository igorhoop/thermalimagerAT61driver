#include <iostream>         // ради printf, cout, string

#include <sys/socket.h>     // расположение /usr/include/x86_64-linux-gnu/sys/
#include <netdb.h>          // этот хедер ради типа sockaddr_in, потому что в socket.h тлько обычный sockaddr
#include <arpa/inet.h>      // это ради функции inet_addr(), которая преобразует символьный IP-адрес в сетевой с нужным порядком байт
#include <unistd.h>         // ради функции close(), чтобы закрыть сокет

class Netabstraction
{
    public:

        char * Receive_Buff = nullptr;  // указатель на приемный буфер. Здесь будут располагаться вновь полученные данные

        // конструктор и деструктор
        Netabstraction(int);
        ~Netabstraction();

        // метод доступа
        int GetRecvBytes();

        void Receive();                     // получить данные
        void Send(void * buf, int len);     // отправить данные и разорвать соединение

    private:
        int ServerSocket;      // дескриптор слушающего сокета
        int ClientSocket;      // дескриптор сокета для обмена данными с клиентом

        sockaddr_in ClientAddr; // адресная структура для инфы о клиенте, которая будет заполняться при приходе от него сообщений
        int ClientAddrSize;     // размер структуры. адрес этой переменной, передадим функции accept()

        int CountBytesRecv;         // число вновь полученных байт
        int CountBytesSend;         // число вновь отправленных байт

};