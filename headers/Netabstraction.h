#include <sys/socket.h>     // расположение /usr/include/x86_64-linux-gnu/sys/
#include <netdb.h>          // этот хедер ради типа sockaddr_in, потому что в socket.h тлько обычный sockaddr
#include <arpa/inet.h>      // это ради функции inet_addr(), которая преобразует символьный IP-адрес в сетевой с нужным порядком байт
#include <unistd.h>         // ради функции close(), чтобы закрыть сокет

#include <iostream>         // ради printf, cout, string

class Netabstraction
{
    public:

        int bytes_recv;         // число вновь полученных байт
        int bytes_send;         // число вновь отправленных байт
        
        char * Receive_Buff = nullptr;    // указатель на приемный буфер. Здесь будут располагаться вновь полученные данные

        Netabstraction(int);
        ~Netabstraction();
        
        void Receive();
        void Send(void * buf, int len);

 

    private:
        int ServerSocket;      // дескриптор слушающего сокета
        int ClientSocket;      // дескриптор сокета для обмена данными с клиентом

        sockaddr_in ClientAddr; // адресная структура для инфы о клиенте, которая будет заполняться при приходе от него сообщений
        int ClientAddrSize;     //  размер структуры. адрес этой переменной, передадим функции accept()

        


};