#ifndef SerialFileListing_h
#define SerialFileListing_h

#include "Arduino.h"

class SerialFileListing
{
  public:
    SerialFileListing(byte charSize);
    byte charSize;
    String dir;
    
    void setSerial(Stream *streamObject);
    void poll();
    void sendText(char *text);
    bool goFolder(String folderName);
    long count();
    long entryIdx(String name);
    String entry(long idx);
    void recieveData();

  private:
//    const byte numChars = 64;
    char* receivedChars;
    char* tempChars;        // temporary array for use when parsing
    char* strtokIndx;
    char* messageFromPC;
    char* fileListing;
    boolean newData = false;
    
    Stream *_streamRef;
    void recvWithStartEndMarkers();
    void parseData();
};

#endif
