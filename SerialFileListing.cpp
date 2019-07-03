#include "SerialFileListing.h"

SerialFileListing::SerialFileListing()
{
    byte numChars = 64;
    charSize = numChars;
    receivedChars = new char[charSize];
    tempChars = new char[charSize];
    strtokIndx = new char[charSize];
    messageFromPC = new char[charSize];
}

//SerialFileListing::begin()
//{
//  Serial.begin(9600);
//}

void SerialFileListing::setSerial(Stream &streamObject)
{
  _streamRef = streamObject;
  _streamRef.println("Set Stream object");
}

void SerialFileListing::sendText(String text)
{
  _streamRef.println(text);
}

bool SerialFileListing::goFolder(String folderName)
{
  _streamRef.println("Go folder" + folderName);
  dir = folderName;
//  _streamRef->println(":ls:" + folderName);
  return true;
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

    while (_streamRef.available() > 0 && newData == false) {
        rc = _streamRef.read();

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

//    _streamRef->println(rc);
//    _streamRef->println(receivedChars);
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
        //_streamRef->println(fileListing);
        newData = false;
    }
}

void SerialFileListing::parseData()
{
    // We need an array here, add items to array
    static long ndx = 0;
    char* cmd = strtok(tempChars,":");
//    _streamRef->println(cmd);
//    _streamRef->println(String(cmd) == "count");
    char* s = strtok(NULL,",");

    if (String(cmd) == "ls")
    {
      while(s) {
        //fileListing[ndx] = s;
        s = strtok(NULL, ",");
        ndx++;
      }
    }

    if (String(cmd) == "count") 
    {
      _streamRef.println("getting count");
      countVal = atof(s);
      fetchingCount = false;
    }

    if (String(cmd) == "entryIdx") 
    {
      _streamRef.println("getting entry idx");
      entryIdxVal = atol(s);
      fetchingEntryIdx = false;
    }

    if (String(cmd) == "entry") 
    {
      _streamRef.println("getting entry");
      entryVal = String(s);
      fetchingEntry = false;
    }
    
}


long SerialFileListing::count()
{
    _streamRef.print(":count:");
    _streamRef.println(dir);

    fetchingCount = true;

    while (fetchingCount)
    {
      poll();
    }

    return countVal;
}

long SerialFileListing::entryIdx(String name)
{
    _streamRef.print(":entryIdx:");
    _streamRef.print(dir);
    _streamRef.println(":" + name);

    fetchingEntryIdx = true;

    while (fetchingEntryIdx)
    {
      poll();
    }

    return entryIdxVal;
}
String SerialFileListing::entry(long idx)
{
    _streamRef.print(":entry:");
    _streamRef.print(dir);
    _streamRef.print(":");
    _streamRef.println(idx);

    fetchingEntry = true;

    while (fetchingEntry)
    {
      poll();
    }

    return entryVal;
}

void SerialFileListing::printList()
{
//    for (int i=0; i<sizeof fileListing/sizeof fileListing[0]; i++) {
//      _streamRef->println(i);
//      String str(fileListing[i]);
//      _streamRef->println(str);
//
//      //_streamRef->println(fileListing[i]);
//    }
}

SerialFileListing SFL;
