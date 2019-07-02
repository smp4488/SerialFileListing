#include "SerialFileListing.h"

SerialFileListing::SerialFileListing(byte numChars)
{
    charSize = numChars;
    receivedChars = new char[charSize];
    tempChars = new char[charSize];
    strtokIndx = new char[charSize];
    messageFromPC = new char[charSize];
    fileListing = new char[charSize];
}
void SerialFileListing::setSerial(Stream *streamObject)
{
  _streamRef = streamObject;
}

void SerialFileListing::sendText(char *text)
{
  _streamRef->println(text);
}

bool SerialFileListing::goFolder(String folderName)
{
  _streamRef->println(":ls:" + folderName);
  dir = folderName;
}

void SerialFileListing::poll()
{
  recvWithStartEndMarkers();
  recieveData();
}


void SerialFileListing::recvWithStartEndMarkers()
{
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (_streamRef->available() > 0 && newData == false) {
        rc = _streamRef->read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= charSize) {
                    ndx = charSize - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

void SerialFileListing::recieveData()
{
    if (newData == true) {
        strcpy(tempChars, receivedChars);
            // this temporary copy is necessary to protect the original data
            //   because strtok() used in parseData() replaces the commas with \0
        parseData();
        //showParsedData();
//        _streamRef->println(messageFromPC);
        _streamRef->println(fileListing);
        newData = false;
    }
}

void SerialFileListing::parseData()
{
    // We need an array here, add items to array
    static byte ndx = 0;
    char* s = strtok(tempChars,",");
    while(s) {
      _streamRef->println(s);
      fileListing[ndx] = new char[strlen(s) + 1];
      strcpy(fileListing[ndx],s);
      //strcat(messageFromPC, s); // copy it to messageFromPC
      s = strtok(NULL, ",");
      ndx++;
    }
    //strtokIndx = strtok(tempChars,",");      // get the first part - the string
    
}

long SerialFileListing::count()
{
    return sizeof(fileListing);
}

long SerialFileListing::entryIdx(String name)
{
    
}
String SerialFileListing::entry(long idx)
{
    return fileListing[idx];
}
