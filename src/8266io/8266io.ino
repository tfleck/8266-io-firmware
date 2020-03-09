// ESP-8266 Firmware for Adafruit IO
// This firmware allows the user to configure the WiFi information, Adafruit IO
// credentials, and publish data to feeds using serial commands
//
// Written by Theo Fleck

/************************** Configuration ***********************************/

#include "AdafruitIO_WiFi.h"

/************************ Firmware Starts Here *******************************/

// Define configuration parameter defaults
String IO_USERNAME = "";
String IO_KEY = "";
String WIFI_SSID = "";
String WIFI_PASS = "";
bool isSetup = false;

// Define all 10 usable feeds
AdafruitIO_WiFi *io;
AdafruitIO_Feed *feed1;
AdafruitIO_Feed *feed2;
AdafruitIO_Feed *feed3;
AdafruitIO_Feed *feed4;
AdafruitIO_Feed *feed5;
AdafruitIO_Feed *feed6;
AdafruitIO_Feed *feed7;
AdafruitIO_Feed *feed8;
AdafruitIO_Feed *feed9;
AdafruitIO_Feed *feed10;

void setup() {
  // Start the serial connection
  Serial.begin(9600);
}

void loop() {
  // Keep IO connection running
  if(isSetup){
    io->run();
  }
  if(Serial.available()){
    // If a serial command was received, process it
    String str = Serial.readString();
    if(str.indexOf("get_macaddr") >= 0){
      Serial.println(WiFi.macAddress());
    }
    else if(str.indexOf("get_version") >= 0){
      Serial.println("v0.2");
    }
    else if(str.indexOf("wifi_ssid=") >= 0){
      int ind = str.indexOf("wifi_ssid=")+10;
      WIFI_SSID = str.substring(ind);
      WIFI_SSID.trim();
      Serial.println("WiFi SSID changed to: "+WIFI_SSID);
    }
    else if(str.indexOf("wifi_pass=") >= 0){
      int ind = str.indexOf("wifi_pass=")+10;
      WIFI_PASS = str.substring(ind);
      WIFI_PASS.trim();
      Serial.println("WiFi Pass changed to: "+WIFI_PASS);
    }
    else if(str.indexOf("io_user=") >= 0){
      int ind = str.indexOf("io_user=")+8;
      IO_USERNAME = str.substring(ind);
      IO_USERNAME.trim();
      Serial.println("IO User changed to: "+IO_USERNAME);
    }
    else if(str.indexOf("io_key=") >= 0){
      int ind = str.indexOf("io_key=")+7;
      IO_KEY = str.substring(ind);
      IO_KEY.trim();
      Serial.println("IO Key changed to: "+IO_KEY);
    }
    else if(str.indexOf("setup_io") >= 0){
      Serial.println("Setting up IO");
      setupIO();
    }
    else if(str.indexOf("setup_feed=") >= 0){
      int ind = str.indexOf("setup_feed=")+11;
      int ind2 = str.indexOf(",",ind);
      String feedIndex = str.substring(ind,ind2);
      feedIndex.trim();
      String feedName = str.substring(ind2+1);
      feedName.trim();
      setupFeed(feedIndex.toInt(),feedName);
    }
    else if(str.indexOf("send_data=") >= 0){
      int ind = str.indexOf("send_data=")+10;
      int ind2 = str.indexOf(",",ind);
      String feedIndex = str.substring(ind,ind2);
      feedIndex.trim();
      String data = str.substring(ind2+1);
      data.trim();
      Serial.println("Sending:"+data);
      sendNum(feedIndex.toInt(),data.toInt());
    }
    else if(str.indexOf("get_data=") >= 0){
      int ind = str.indexOf("get_data=")+9;
      String feedIndex = str.substring(ind);
      feedIndex.trim();
      getData(feedIndex.toInt());
    }
    else if(str.indexOf("get_name=") >= 0){
      int ind = str.indexOf("get_name=")+9;
      String feedIndex = str.substring(ind);
      feedIndex.trim();
      getName(feedIndex.toInt());
    }
  }
}

void sendNum(int feed_index, int data){
  // save data to feed specified by provided index
  AdafruitIO_Feed** feed = getFeed(feed_index);
  if(feed){
    (*feed)->save(data);
  }
  else{
    Serial.println("Error: Feed Not Found");
  }
}

void getData(int feed_index){
  // query data from feed specified by index
  AdafruitIO_Feed** feed = getFeed(feed_index);
  if(feed){
    AdafruitIO_Data *data = (*feed)->lastValue();
    if(data != NULL){
      Serial.println(data->toString());
    }
    else{
      Serial.println("Error: Data Was Null");
    }
  }
  else{
    Serial.println("Error: Feed Not Found");
  }
}

void getName(int feed_index){
  // query name from feed specified by index
  AdafruitIO_Feed** feed = getFeed(feed_index);
  if(feed){
    Serial.println((*feed)->name);
  }
  else{
    Serial.println("Error: Feed Not Found");
  }
}

void setupIO(){
  io = new AdafruitIO_WiFi(IO_USERNAME.c_str(), IO_KEY.c_str(), WIFI_SSID.c_str(), WIFI_PASS.c_str());

  // connect to io.adafruit.com
  io->connect();

  // wait for a connection
  while(io->status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io->statusText());
  isSetup = true;
}

AdafruitIO_Feed** getFeed(int feed_index){
  // translate index number into IO feed object, return NULL if invalid
  if(feed_index == 1){
    return &feed1;
  }
  else if(feed_index == 2){
    return &feed2;
  }
  else if(feed_index == 3){
    return &feed3;
  }
  else if(feed_index == 4){
    return &feed4;
  }
  else if(feed_index == 5){
    return &feed5;
  }
  else if(feed_index == 6){
    return &feed6;
  }
  else if(feed_index == 7){
    return &feed7;
  }
  else if(feed_index == 8){
    return &feed8;
  }
  else if(feed_index == 9){
    return &feed9;
  }
  else if(feed_index == 10){
    return &feed10;
  }
  return NULL;
}

void setupFeed(int feed_index, String feed_name){
  AdafruitIO_Feed** feed = getFeed(feed_index);
  //make copy of name on heap, so the pointer in the feed doesn't get deallocated
  char* f_name = (char*)calloc(feed_name.length()+1,sizeof(char));
  strncpy(f_name,feed_name.c_str(),feed_name.length());
  if(feed){
    *feed = io->feed(f_name);
    Serial.println("Feed "+String(feed_index)+" setup");
  }
  else{
    Serial.println("Error setting up feed "+String(feed_index)); 
  }
}
