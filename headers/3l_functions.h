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
 int8_t around_temp;
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