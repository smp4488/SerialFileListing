#include <menu.h>
#include <TimerOne.h>
#include <menuIO/serialIO.h>
#include <menuIO/chainStream.h>
#include <menuIO/clickEncoderIn.h>
#include "SerialMenu.h"

using namespace Menu;

#define MAX_DEPTH 3
#define SERIAL_BUFFER_SIZE 256

// Serial File Listing /////////////////////////////////////
result filePick(eventMask event, navNode& nav, prompt &item);

SerialMenu filePickMenu("Backing Tracks", "/", filePick, enterEvent);

//implementing the handler here after filePick is defined...
result filePick(eventMask event, navNode& nav, prompt &item) {
  if (nav.root->navFocus == (navTarget*)&filePickMenu) {
    Serial.println(":play:" + filePickMenu.selectedFolder + filePickMenu.selectedFile);
  }
  return proceed;
}

result stopAudio(eventMask event, navNode& nav, prompt &item) {
  switch(event){
    case enterEvent:
      Serial.println(":stop");
  }
  return proceed;
}

// Encoder /////////////////////////////////////
#define encA A1
#define encB A0
#define encBtn 2

ClickEncoder clickEncoder(encA,encB,encBtn,4);
ClickEncoderStream encStream(clickEncoder,1);
void timerIsr() { clickEncoder.service(); }

MENU(mainMenu, "Guitarix Pedalboard Menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,SUBMENU(filePickMenu)
  ,OP("Stop Backing Track",stopAudio,anyEvent)
  ,OP("Sub2",Menu::doNothing,anyEvent)
  ,OP("Sub3",Menu::doNothing,anyEvent)
  ,EXIT("<Back\r\n")
);

serialIn serial(Serial);
MENU_INPUTS(in,&encStream,&serial);

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);

void setup() {
  Serial.begin(115200);
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);
  filePickMenu.begin();
}

void loop() {
  nav.poll();
}
