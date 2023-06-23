#include <array>
#include <sys/socket.h> // расположение /usr/include/x86_64-linux-gnu/sys/
#include <netdb.h> // этот хедер ради типа sockaddr_in, потому что в socket.h тлько обычный sockaddr
#include <unistd.h>
#include <arpa/inet.h> // это ради функции inet_addr(), которая преобразует символьный IP-адрес в сетевой с нужным порядком байт





void InitialSDK();
int DeviceConnect();

std::string GetContentFromFile(const std::string & filePath);
int GetConfigForConnectCAM(std::string path);
std::string GetCurrentTimestamp(int format);
uint8_t CheckHTTPRequest(std::string request);
void ReinitialAndConnect();
void ConfigDevice();
int PingDevice();
int GetDeviceInfo();
int WaitDevice();
int SetEnvirParams();
int RewriteFileContent(std::string file_path, std::string option_name, std::string value);
int SetTempLimit(int32_t tmin, int32_t tmax);
int RequestTemperatures(struct SENDPARAM * OutputData);
int MakeCapture(std::string capture_path, std::string capture_name, struct SENDPARAM * OutputData);
int SetAirTemp(std::string config_path, int8_t AirTemp);
int SetDistance(std::string config_path, int8_t Distance);
int SetEmissivityHumidity(std::string config_path, float Emissivity, float Humidity);
int SetTemperatureLimit(std::string config_path, int32_t tmin, int32_t tmax);
int GetTemperaturePixel(std::string capture_path, int32_t x, int32_t y, struct SENDPARAM * OutputData);
int GetMapPixel(std::string capture_path, std::array<uint8_t, 327680> &response_temp_data);

// net
int PortOpen();

// threads
void * PingDeviceThread(void * args);




// callbacks
void TempCallBackMy(char *pBuffer, long BufferLen, void* pContext);
void SerialCallBackMy(char *pRecvDataBuff, int BuffSize, void *context);
void SnapCallBackMy(int m_ch, char *pBuffer, int size, void *context);



#pragma pack(push, 1)
struct SENDPARAM {
 uint8_t error;
 int32_t average_t;
 int32_t min_t;
 int32_t max_t;
 uint8_t signal;
};

struct GETAIRTEMP {
 uint8_t cmd; 
 int8_t air_temp;
};

struct GETDISTANCE {
 uint8_t cmd; 
 uint8_t distance;
};

struct GETENVIRPARAMS {
 uint8_t cmd; 
 float emissivity;
 float humidity;
};

struct GETTEMPLIM {
 uint8_t cmd; 
 int32_t min_t;
 int32_t max_t;
};
#pragma pack(pop)



class NETABSTRACTION
{
    private:

    public:
        int port;
        int listener_socket;    // дескриптор слушающего сокета
        int exchange_socket;    // десприктор сокета для обмена данными с клиентом
        char * Receive_Buff = nullptr;    // сетевой приемный буфер
        int bytes_recv; // число полученных байт
        int bytes_send; // число отправленных байт

        sockaddr_in ClientAddr; // адресная структура клиента для инфы о нем, которая будет заполняться при приходе от него сообщений

        int ClientAddrSize; // адрес этой переменной, в которой лежит размер структуры, передадим функции accept()

        void Send(void * buf, int len)
        {
            int res;
            res=send(exchange_socket, buf, len, 0);
            std::cout << "Отправляем обратно количество байт: " << result << std::endl;


            close(exchange_socket); // закрываем соединение
        }


        void Receive()
        {
            exchange_socket = accept(listener_socket, (sockaddr *) &ClientAddr, (socklen_t *) &ClientAddrSize); // а вот здесь уже блокируется программа. Извлекает первый запрос из очереди либо если очередь пустая ждет и блокирует программу до первого соединения
            if(exchange_socket >= 0)
            {
                printf("Принят запрос на соединение:\n");
            }
            else
            {
                printf("Что то не так с сокетом обмена:\n");
                std::cout << errno;
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
        }

        int Initialization(int port)
        {
            sockaddr_in ServerAddr;                  // специальная адресная структура, которую свяжем с сокетом
            ServerAddr.sin_family = AF_INET;         // заполняем ее
            ServerAddr.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY, для входящих от всех интерфейсов. Ранее было inet_addr(my_ip), таким образом привязывались к конкретному сетевому интерфейсу
            ServerAddr.sin_port = htons(port);       // занимаемый порт на моем компьютере

            listener_socket = socket(AF_INET,SOCK_STREAM, 0); // Семейство Internet, потоковые сокеты, TCP
            if (bind(listener_socket, (sockaddr *) &ServerAddr, sizeof(ServerAddr)) < 0)
            {
                std::cerr << "Ошибка bind. Попробуйте перезапустить через минуту" << std::endl; // МИНУТА!
                return 1;
            }

            // для TCP нужно возиться с соединениями (использовать listen() и accept())
            if(listen(listener_socket, 5) < 0) // создание очереди запросов на соединение. Указывается размер. Не блокирует программу
            {
                std::cerr << "Ошибка Listen";
                return 2;
            }

            if(Receive_Buff)
                delete [] Receive_Buff;

            Receive_Buff = new char[100];



            ClientAddrSize = sizeof(ClientAddr); // адрес этой переменной, в которой лежит размер структуры, передадим функции accept()
            return 0;
        }

        void End()
        {
            close(listener_socket);
        }


        ~NETABSTRACTION()
        {
            if(Receive_Buff != nullptr)
            {
                delete [] Receive_Buff;
            }
        }
};