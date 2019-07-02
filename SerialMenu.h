/* -*- C++ -*- */
#pragma once
// a full automated SDCard file select
// plugin for arduino menu library
// requires a dynamic menu (MENU_USERAM)
// IO: Serial
// Feb 2019 - Rui Azevedo [ruihfazevedo@gmail.com]
#include <menu.h>
#include "SerialFileListing.h"

using namespace Menu;

//SerialFileListing sfList();

//minimalist SD Card driver (using arduino SD)
//we avoid allocating memory here, instead we read all info from SD
template<class SerialFileListing>
class FSO {
public:
  using Type=SerialFileListing;
  Type& sfc;
  //idx_t selIdx=0;//preserve selection context, because we preserve folder ctx too
  //we should use filename instead! idx is useful for delete operations thou...

  //File dir;
  SerialFileListing sfList;

  FSO(Type& sfc):sfc(sfc) {
//    sfList = new SerialFileListing();
    Serial.begin(9600);
    sfc.setSerial(&Serial);
  }
  virtual ~FSO() {}
  //open a folder
  bool goFolder(String folderName) {
    return sfc.goFolder(folderName);
  }
  //count entries on folder (files and dirs)
  long count() {
    return sfc.count();
  }

  //get entry index by filename
  long entryIdx(String name) {
    return sfc.entryIdx(name);
  }

  //get folder content entry by index
  String entry(long idx) {
    return sfc.entry(idx);
  }

};

//////////////////////////////////////////////////////////////////////
// SD Card cached menu
template<typename SerialFileListing,idx_t maxSz>
class CachedFSO:public FSO<SerialFileListing> {
public:
  using Type=SerialFileListing;
  long cacheStart=0;
  String cache[maxSz];
  long size=0;//folder size (count of files and folders)
  CachedFSO(Type& sfl):FSO<SerialFileListing>(sfl) {}
  
  void refresh(long start=0) {
    if (start<0) start=0;
    // Serial.print("Refreshing from:");
    // Serial.println(start);
    cacheStart=start;
    FSO<SerialFileListing>::dir.rewindDirectory();
    size=0;
//    while(true) {
//      File file=FSO<SDC>::dir.openNextFile();
//      if (!file) {
//        file.close();
//        break;
//      }
//      if (start<=size&&size<start+maxSz)
//        cache[size-start]=String(file.name())+(file.isDirectory()?"/":"");
//      file.close();
//      size++;
//    }
  }
  //open a folder
  bool goFolder(String folderName) {
    if (!FSO<SerialFileListing>::goFolder(folderName)) return false;
    refresh();
    return true;
  }
  long count() {return size;}

  long entryIdx(String name) {
    idx_t sz=min(count(),(long)maxSz);
    for(int i=0;i<sz;i++)
      if (name==cache[i]) return i+cacheStart;
    long at=FSO<SerialFileListing>::entryIdx(name);
    //put cache around the missing item
    refresh(at-(maxSz>>1));
    return at;
  }
  String entry(long idx) {
    if (0>idx||idx>=size) return "";
    if (cacheStart<=idx&&idx<(cacheStart+maxSz)) return cache[idx-cacheStart];
    refresh(idx-(maxSz>>1));
    return entry(idx);
  }
};

////////////////////////////////////////////////////////////////////////////
//#include <SD.h>
// instead of allocating options for each file we will instead customize a menu
// to print the files list, we can opt to use objects for each file for a
// faster reopening.. but its working quite fast
// On this example we assume the existence of an esc button as we are not drawing
// an exit option (or [..] as would be appropriate for a file system)
// not the mennu presents it self as the menu and as the options
// ands does all drawing navigation.
//TODO: we can specialize input too, for typing filename select
  #define USE_BACKDOTS 1

template<typename FS>
class SDMenuT:public menuNode,public FS {
public:
  String folderName="/";//set this to other folder when needed
  String selectedFolder="/";
  String selectedFile="";
  // using menuNode::menuNode;//do not use default constructors as we wont allocate for data
  SDMenuT(typename FS::Type& sd,constText* title,const char* at,Menu::action act=doNothing,Menu::eventMask mask=noEvent)
    :menuNode(title,0,NULL,act,mask,
      wrapStyle,(systemStyles)(_menuData|_canNav))
    ,FS(sd)
    {}

  void begin() {FS::goFolder(folderName);}

  //this requires latest menu version to virtualize data tables
  prompt& operator[](idx_t i) const override {return *(prompt*)this;}//this will serve both as menu and as its own prompt
  result sysHandler(SYS_FUNC_PARAMS) override {
    switch(event) {
      case enterEvent:
        if (nav.root->navFocus!=nav.target) {//on sd card entry
          nav.sel=((SDMenuT<FS>*)(&item))->entryIdx(((SDMenuT<FS>*)(&item))->selectedFile)+USE_BACKDOTS;//restore context
        }
    }
    return proceed;
  }

  void doNav(navNode& nav,navCmd cmd) {
    switch(cmd.cmd) {
      case enterCmd: if (nav.sel>=USE_BACKDOTS) {
          String selFile=SDMenuT<FS>::entry(nav.sel-USE_BACKDOTS);
          if (selFile.endsWith("/")) {
            // Serial.print("\nOpen folder...");
            //open folder (reusing the menu)
            folderName+=selFile;
            SDMenuT<FS>::goFolder(folderName);
            dirty=true;//redraw menu
            nav.sel=0;
          } else {
            //Serial.print("\nFile selected:");
            //select a file and return
            selectedFile=selFile;
            selectedFolder=folderName;
            nav.root->node().event(enterEvent);
            menuNode::doNav(nav,escCmd);
          }
          return;
        }
      case escCmd:
        if(folderName=="/")//at root?
          menuNode::doNav(nav,escCmd);//then exit
        else {//previous folder
          idx_t at=folderName.lastIndexOf("/",folderName.length()-2)+1;
          String fn=folderName.substring(at,folderName.length()-1);
          folderName.remove(folderName.lastIndexOf("/",folderName.length()-2)+1);
          SDMenuT<FS>::goFolder(folderName);
          dirty=true;//redraw menu
          nav.sel=SDMenuT<FS>::entryIdx(fn)+USE_BACKDOTS;
        }
        return;
    }
    menuNode::doNav(nav,cmd);
  }

  //print menu and items as this is a virtual data menu
  Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t pn) {
    if(root.navFocus!=this) {//show given title or filename if selected
      return selectedFile==""?
        menuNode::printTo(root,sel,out,idx,len,pn):
        out.printRaw(selectedFile.c_str(),len);
    } else if(idx==-1) {//when menu open (show folder name)
      ((menuNodeShadow*)shadow)->sz=SDMenuT<FS>::count()+USE_BACKDOTS;
      idx_t at=folderName.lastIndexOf("/",folderName.length()-2)+1;
      String fn=folderName.substring(at,folderName.length()-1);
      return out.printRaw(fn.c_str(),len);
      // return out.printRaw(folderName.c_str(),len);
      // return out.printRaw(SDMenuT<FS>::dir.name(),len);
    }
    //drawing options
    idx_t i=out.tops[root.level]+idx;
    if (i<USE_BACKDOTS) len-=out.printRaw("[..]",len);
    else len-=out.printRaw(SDMenuT<FS>::entry(out.tops[root.level]+idx-USE_BACKDOTS).c_str(),len);
    return len;
  }
};

class SDMenu:public SDMenuT<FSO<decltype(SFL)>> {
public:
  SDMenu(constText* title,const char* at,Menu::action act=doNothing,Menu::eventMask mask=noEvent)
    :SDMenuT<FSO<decltype(SFL)>>(SFL,title,at,act,mask) {}
};

template<idx_t cacheSize>
class CachedSDMenu:public SDMenuT<CachedFSO<decltype(SFL),cacheSize>> {
public:
  CachedSDMenu(constText* title,const char* at,Menu::action act=doNothing,Menu::eventMask mask=noEvent)
    :SDMenuT<CachedFSO<decltype(SFL),cacheSize>>(SFL,title,at,act,mask) {}
};
