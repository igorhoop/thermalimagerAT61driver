void initial();

std::string GetContentFromFile(const std::string & filePath);
void GetConfigForConnectCAM(std::string path);
std::string GetCurrentTimestamp(int format);
uint8_t CheckHTTPRequest(std::string request);
void reinitial();
void ConfigDevice();




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

struct GETPARAM {
 uint8_t cmd; 
 int32_t around_temp;
 int32_t distance;
 int32_t min_t;
 int32_t max_t;
};
#pragma pack(pop)