/* IDE Driver test */
#include<types.h>
#include<VirtualConsole.h>
#include<DiskDriver.h>

#ifndef _MONA_MIDEDRIVER_MJT
#define _MONA_MIDEDRIVER_MJT


class IDEDevice : public DiskDriver {
  public:
    bool read(dword lba, byte* buf);
    bool write(dword lba, byte* buf);
    IDEDevice(class IDEDriver *bus,unsigned int device);
    ~IDEDevice();
    static class IDEDriver* Bus;
  private:
    static unsigned int device_;
};



class IDEDriver {
  public:
    IDEDriver(VirtualConsole* console,unsigned int port);
    ~IDEDriver();

  public:
    static IDEDevice* Master;
    static IDEDevice* Slave;
    static bool HasMaster;
    static bool HasSlave;
  
  public:
    bool sendcmd(int cmd,char *bfr,int bfrsize /* ignored */);
    bool senddevice(int drive);
    bool waithdc(unsigned long timeout);
    bool waitdata(unsigned long timeout);
    bool waitready(unsigned long timeout);
    bool initilize();
    static VirtualConsole* console_;
  private:
    byte version_;
    static unsigned int control_;
    static unsigned int data_;
    static unsigned int error_;
    static unsigned int status_;
    static unsigned int sector_;
    static unsigned int count_;
    static unsigned int cylinderL_;
    static unsigned int cylinderH_;
    static unsigned int head_;
    static unsigned int status2_;
    static unsigned int driveaddr_;
    
    static byte results_[10];
    static int resultsLength_;
    static bool interrupt_;
    static byte* dmabuff_;
};

extern IDEDriver* gIDEDriver;

#endif
