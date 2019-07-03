
//#include "SerialFileListing.h"
#include <menu.h>
#include <TimerOne.h>
#include <menuIO/serialIO.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>
#include <menuIO/chainStream.h>
#include <menuIO/clickEncoderIn.h>
#include "SerialMenu.h"

using namespace Menu;

#define MAX_DEPTH 3

// Max size 64
//SerialFileListing sfList(64);

// Serial File Listing /////////////////////////////////////
result filePick(eventMask event, navNode& nav, prompt &item);
CachedSDMenu<32> filePickMenu("Backing Tracks","/",filePick,enterEvent);

//implementing the handler here after filePick is defined...
result filePick(eventMask event, navNode& nav, prompt &item) {
  // switch(event) {//for now events are filtered only for enter, so we dont need this checking
  //   case enterCmd:
      if (nav.root->navFocus==(navTarget*)&filePickMenu) {
        Serial.println();
        Serial.print("selected file:");
        Serial.println(filePickMenu.selectedFile);
        Serial.print("from folder:");
        Serial.println(filePickMenu.selectedFolder);
      }
  //     break;
  // }
  return proceed;
}

// Encoder /////////////////////////////////////
#define encA A1
#define encB A0
#define encBtn 2

ClickEncoder clickEncoder(encA,encB,encBtn,4);
ClickEncoderStream encStream(clickEncoder,1);

MENU(mainMenu, "Guitarix Pedalboard Menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,SUBMENU(filePick)
  ,OP("Sub1",Menu::doNothing,anyEvent)
  ,OP("Sub2",Menu::doNothing,anyEvent)
  ,OP("Sub3",Menu::doNothing,anyEvent)
  ,EXIT("<Back\r\n")
);

serialIn serial(Serial);
MENU_INPUTS(in,&encStream,&serial);
void timerIsr() {clickEncoder.service();}

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);

void setup() {
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);

  //filePickMenu.begin();
  // put your setup code here, to run once:
//  Serial.begin(9600);
//  Serial.println("Starting");
//  sfList.setSerial(&Serial);
//  sfList.goFolder("/");
//  sfList.poll();
//  Serial.println(sfList.count());
}


void loop() {
  nav.poll();
//  sfList.poll();
//  delay(3000);
//  Serial.println(sfList.count());
//  Serial.println(sfList.entryIdx("jazz"));
//  Serial.println(sfList.entry(1));
//  delay(3000);
//  sfList.goFolder("/jazz");
}
