void initial();




std::string GetContentFromFile(const std::string & filePath);
void GetConfigForConnectCAM(std::string path);
std::string GetCurrentTimestamp(int format);


// callbacks
void TempCallBackMy(char *pBuffer, long BufferLen, void* pContext);
void SerialCallBackMy(char *pRecvDataBuff, int BuffSize, void *context);
void SnapCallBackMy(int m_ch, char *pBuffer, int size, void *context);
//void MessageCallBackReceiveMy(IRNETHANDLE SdkHandle, WPARAM wParam, LPARAM iParam, void * context);


