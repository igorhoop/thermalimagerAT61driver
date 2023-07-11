/*
    РЕАЛИЗАЦИЯ КЛАССА СЕТЕВОЙ АБСТРАКЦИИ
*/

#include "../headers/Netabstraction.h"

// конструктор
Netabstraction::Netabstraction(int Port)
{
    sockaddr_in ServerAddr;                  // специальная адресная структура, которую свяжем с сокетом
    ServerAddr.sin_family = AF_INET;         // заполняем ее
    ServerAddr.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY, для входящих от всех интерфейсов. Ранее было inet_addr(my_ip), таким образом привязывались к конкретному сетевому интерфейсу
    ServerAddr.sin_port = htons(Port);       // занимаемый порт на моем компьютере

    ServerSocket = socket(AF_INET,SOCK_STREAM, 0); // Семейство Internet, потоковые сокеты, TCP
    if (bind(ServerSocket, (sockaddr *) &ServerAddr, sizeof(ServerAddr)) < 0)
    {
        std::cerr << "Ошибка bind. Попробуйте перезапустить через минуту" << std::endl; // МИНУТА!
        exit(1);
    }

    // для TCP нужно возиться с соединениями (использовать listen() и accept())
    if(listen(ServerSocket, 5) < 0) // создание очереди запросов на соединение. Указывается размер. Не блокирует программу
    {
        std::cerr << "Ошибка Listen";
        exit(1);
    }

    if(Receive_Buff)
        delete [] Receive_Buff;

    Receive_Buff = new char[100];

    ClientAddrSize = sizeof(ClientAddr); // адрес этой переменной, в которой лежит размер структуры, передадим функции accept()
}


// деструктор
Netabstraction::~Netabstraction()
{
    close(ServerSocket);

    if(Receive_Buff != nullptr)
    {
        delete [] Receive_Buff;
    }
}


// методы доступа
int Netabstraction::GetRecvBytes()
{
    return CountBytesRecv;
}


void Netabstraction::Receive()
{
    ClientSocket = accept(ServerSocket, (sockaddr *) &ClientAddr, (socklen_t *) &ClientAddrSize); // а вот здесь уже блокируется программа. Извлекает первый запрос из очереди либо если очередь пустая ждет и блокирует программу до первого соединения
    if(ClientSocket >= 0)
    {
        printf("Принят запрос на соединение:\n");
    }
    else
    {
        printf("Что то не так с сокетом обмена:\n");
        std::cout << errno;
    }

    std::string ClientIp = "";
    ClientIp = inet_ntoa(ClientAddr.sin_addr);
    printf("\tIP-адрес подключившегося: %s \n", ClientIp.c_str()) ;
    printf("\tЕго порт: %d\n", ClientAddr.sin_port);
    //std::cout << "\tВремя подключения: " << GetCurrentTimestamp(1) << std::endl;
    
    // получение данных из сокета обмена данными
    CountBytesRecv = recv(ClientSocket, Receive_Buff, 100, 0);
    std::cout << "\tПришло байт: " << CountBytesRecv << std::endl << "\tСостав посылки: \n\t";
    for(int i=0; i < CountBytesRecv; i++)
    {
        printf("0x%02X, ", Receive_Buff[i]); // отображаем принятые байты
    }
}


void Netabstraction::Send(void * buf, int len)
{
    int res;
    res=send(ClientSocket, buf, len, 0);
    std::cout << "Отправляем обратно количество байт: " << res << std::endl;
    close(ClientSocket); // закрываем соединение сразу после отправки
}