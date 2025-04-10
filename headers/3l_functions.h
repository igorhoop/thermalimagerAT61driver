#include <array>


void InitialSDK();
int DeviceConnect();

std::string GetContentFromFile(const std::string & filePath);
int EnterConfigForConnectCAM(std::string path);
std::string GetCurrentTimestamp(int format);
uint8_t CheckHTTPRequest(std::string request);
void ReinitialAndConnect();
void ConfigDevice();
int PingDevice();
int GetDeviceInfo();
int WaitDevice();
int SetEnvirParams();
int RewriteFileContent(std::string file_path, std::string option_name, std::string value);
int RequestTemperatures(struct SENDPARAM * OutputData);
int MakeCapture(std::string capture_path, std::string capture_name, struct SENDPARAM * OutputData);
int SetAirTemp(std::string config_path, int8_t AirTemp);
int SetDistance(std::string config_path, int8_t Distance);
int SetEmissivityHumidity(std::string config_path, float Emissivity, float Humidity);
int SetTemperatureLimit(int32_t tmin, int32_t tmax);
int GetTemperaturePixel(std::string capture_path, int32_t x, int32_t y, struct SENDPARAM * OutputData);
int GetMapPixel(std::string capture_path, std::array<uint8_t, 327680> &response_temp_data);
int MakeVideo(std::string video_path, std::string video_name,  struct SENDPARAM * OutputData);
int RecordInit(const char * filename);
int StopRecord();
int RTSP_Start();

int ReadConfigFromJSON();
int WriteConfigToJSON();


int LogWrite(std::string logfile_path, std::string log_text);

// net
int PortOpen();

// threads
void * PingDeviceThread(void * args);
void * WindowThread(void * args);
void * WindowVideoThread(void * args);
void * VideoThread(void * args); // наверное надо удалить



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

enum RESPONSE_TYPES {
    TEMPERATURES,
    TEMPERATURES_AND_SIGNAL,
    RESULT_OF_COMMAND,
    TEMPERATURE_PIXEL,
    MAP,
    NONE
};



struct PROGRAM_CONFIG
{
    std::string AT61F_CONFIG_PATH;
    std::string AT61F_CAPTURE_PATH;
    std::string AT61F_VIDEO_PATH;
    std::string AT61F_LOG_PATH;
    std::string AT61F_IP;
    std::string AT61F_PORT;
    std::string AT61F_LOGIN;
    std::string AT61F_PASS;
    std::string AT61F_AIRTEMP;
    std::string AT61F_EMISSIVITY;
    std::string AT61F_HUMIDITY;
    std::string AT61F_DISTANCE;
    std::string AT61F_TMIN;
    std::string AT61F_TMAX;
    std::string AT61F_RTSP_URL;
    std::string AT61F_WINDOW_MODE;
    std::string AT61F_RTSP_MODE;

};