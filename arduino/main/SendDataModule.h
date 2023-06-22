#ifndef SendDataModule_h
#define SendDataModule_h

class SendDataModule
{
public:
  void send(HttpClient *client, int gas, int lox, int mode);
};

#endif