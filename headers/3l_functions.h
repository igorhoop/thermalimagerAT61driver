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
void * WindowThread(void * args);


// windows
void DrawMap(std::array<uint8_t, 327680> data);

// callbacks
void TempCallBackMy(char *pBuffer, long BufferLen, void* pContext);
void SerialCallBackMy(char *pRecvDataBuff, int BuffSize, void *context);
void SnapCallBackMy(int m_ch, char *pBuffer, int size, void *context);
void VideoCallBackMy(char *pBuffer, long BufferLen, int width, int height, void* pContext);



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



