// **********************************************************************************
// ESP8266 Teleinfo WEB Server, route web function
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// For any explanation about teleinfo ou use, see my blog
// http://hallard.me/category/tinfo
//
// This program works with the Wifinfo board
// see schematic here https://github.com/hallard/teleinfo/tree/master/Wifinfo
//
// Written by Charles-Henri Hallard (http://hallard.me)
//
// History : V1.00 2015-06-14 - First release
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

// Include Arduino header
#include "webserver.h"

// Optimize string space in flash, avoid duplication
const char FP_JSON_START[] PROGMEM = "{\r\n";
const char FP_JSON_END[] PROGMEM = "\r\n}\r\n";
const char FP_QCQ[] PROGMEM = "\":\"";
const char FP_QCNL[] PROGMEM = "\",\r\n\"";
const char FP_RESTART[] PROGMEM = "OK, Redémarrage en cours\r\n";
const char FP_NL[] PROGMEM = "\r\n";

/* ======================================================================
Function: formatSize 
Purpose : format a asize to human readable format
Input   : size 
Output  : formated string
Comments: -
====================================================================== */
String formatSize(size_t bytes)
{
  if (bytes < 1024){
    return String(bytes) + F(" Byte");
  } else if(bytes < (1024 * 1024)){
    return String(bytes/1024.0) + F(" KB");
  } else if(bytes < (1024 * 1024 * 1024)){
    return String(bytes/1024.0/1024.0) + F(" MB");
  } else {
    return String(bytes/1024.0/1024.0/1024.0) + F(" GB");
  }
}

/* ======================================================================
Function: getContentType 
Purpose : return correct mime content type depending on file extension
Input   : -
Output  : Mime content type 
Comments: -
====================================================================== */
String getContentType(String filename) {
  if(filename.endsWith(".htm")) return F("text/html");
  else if(filename.endsWith(".html")) return F("text/html");
  else if(filename.endsWith(".css")) return F("text/css");
  else if(filename.endsWith(".json")) return F("text/json");
  else if(filename.endsWith(".js")) return F("application/javascript");
  else if(filename.endsWith(".png")) return F("image/png");
  else if(filename.endsWith(".gif")) return F("image/gif");
  else if(filename.endsWith(".jpg")) return F("image/jpeg");
  else if(filename.endsWith(".ico")) return F("image/x-icon");
  else if(filename.endsWith(".xml")) return F("text/xml");
  else if(filename.endsWith(".pdf")) return F("application/x-pdf");
  else if(filename.endsWith(".zip")) return F("application/x-zip");
  else if(filename.endsWith(".gz")) return F("application/x-gzip");
  else if(filename.endsWith(".otf")) return F("application/x-font-opentype");
  else if(filename.endsWith(".eot")) return F("application/vnd.ms-fontobject");
  else if(filename.endsWith(".svg")) return F("image/svg+xml");
  else if(filename.endsWith(".woff")) return F("application/x-font-woff");
  else if(filename.endsWith(".woff2")) return F("application/x-font-woff2");
  else if(filename.endsWith(".ttf")) return F("application/x-font-ttf");
  return "text/plain";
}

/* ======================================================================
Function: handleFileRead 
Purpose : return content of a file stored on SPIFFS file system
Input   : file path
Output  : true if file found and sent
Comments: -
====================================================================== */
bool handleFileRead(String path) {
  if ( path.endsWith("/") ) 
    path += "index.htm";
  
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";

  DebugF("handleFileRead ");
  Debug(path);

  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
    if( SPIFFS.exists(pathWithGz) ){
      path += ".gz";
      DebugF(".gz");
    }

    DebuglnF(" found on FS");
 
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }

  Debugln("");

  server.send(404, "text/plain", "File Not Found");
  return false;
}

/* ======================================================================
Function: handleSpiffsOperation 
Purpose : hadle SPIFFS operation like file upload/replace, file delete
Input   : -
Output  : - 
Comments: -
====================================================================== */
void handleSpiffsOperation(void)
{
  String response="";
  int ret;

  for(int i=0;i<server.args();i++)
  {
    Info(server.argName(i));
    InfoF("=");
    Infoln(server.arg(i));
  }
  Infoflush();
  
  if (server.hasArg("action") && server.hasArg("file"))
  {
    String action=server.arg("action");
    String file=server.arg("file");

    if (action == "delete")
    {
      if (SPIFFS.exists(file))
      {
        if(SPIFFS.remove(file))
        {
          response += "File deleted!";
          ret = 200;
        }
        else
        {
          response += "Unable to delete file!";
          ret = 400;
        }
      }
    }
    else
    {
      response += "Bad argument(s)!";
      ret = 400;
    }
  }
  else
  {
    response += "Missing argument(s)";
    ret = 400;
  }
  

  server.send ( ret, "text/plain", response);
}

/* ======================================================================
Function: handleFormConfig 
Purpose : handle main configuration page
Input   : -
Output  : - 
Comments: -
====================================================================== */
void handleFormConfig(void) 
{
  String response="";
  int ret ;

  LedBluON();

  // We validated config ?
  if (server.hasArg("save"))
  {
    int itemp;
    InfolnF("===== Posted configuration"); 
    
    // WifInfo
    strncpy(config.ssid ,   server.arg("ssid").c_str(),     CFG_SSID_SIZE );
    strncpy(config.psk ,    server.arg("psk").c_str(),      CFG_PSK_SIZE );
    strncpy(config.host ,   server.arg("host").c_str(),     CFG_HOSTNAME_SIZE );
    strncpy(config.ap_psk , server.arg("ap_psk").c_str(),   CFG_PSK_SIZE );
    itemp = server.arg("ap_retrycount").toInt();
    config.ap_retrycount = (itemp>0 && itemp <300) ? itemp : CFG_AP_DEFAULT_RETCNT;
    strncpy(config.ota_auth,server.arg("ota_auth").c_str(), CFG_PSK_SIZE );
    itemp = server.arg("ota_port").toInt();
    config.ota_port = (itemp>=0 && itemp<=65535) ? itemp : DEFAULT_OTA_PORT ;

    if(server.hasArg("cfg_debug")) { config.config |= CFG_DEBUG; } else { config.config &= ~CFG_DEBUG; }
    if(server.hasArg("cfg_oled")) { config.config |= CFG_LCD; } else { config.config &= ~CFG_LCD; }
    if(server.hasArg("cfg_rgb")) { config.config |= CFG_RGB_LED; } else { config.config &= ~CFG_RGB_LED; }
    if(server.hasArg("cfg_info")) { config.config |= CFG_INFO; } else { config.config &= ~CFG_INFO; }

    // Emoncms
    strncpy(config.emoncms.host,   server.arg("emon_host").c_str(),  CFG_EMON_HOST_SIZE );
    strncpy(config.emoncms.url,    server.arg("emon_url").c_str(),   CFG_EMON_URL_SIZE );
    strncpy(config.emoncms.apikey, server.arg("emon_apikey").c_str(),CFG_EMON_APIKEY_SIZE );
    itemp = server.arg("emon_node").toInt();
    config.emoncms.node = (itemp>=0 && itemp<=255) ? itemp : 0 ;
    itemp = server.arg("emon_port").toInt();
    config.emoncms.port = (itemp>=0 && itemp<=65535) ? itemp : CFG_EMON_DEFAULT_PORT ; 
    itemp = server.arg("emon_freq").toInt();
    if (itemp>0 && itemp<=86400){
      // Emoncms Update if needed
      Tick_emoncms.detach();
      Tick_emoncms.attach(itemp, Task_emoncms);
    } else {
      itemp = 0 ; 
    }
    config.emoncms.freq = itemp;

    // jeedom
    strncpy(config.jeedom.host,   server.arg("jdom_host").c_str(),  CFG_JDOM_HOST_SIZE );
    strncpy(config.jeedom.url,    server.arg("jdom_url").c_str(),   CFG_JDOM_URL_SIZE );
    strncpy(config.jeedom.apikey, server.arg("jdom_apikey").c_str(),CFG_JDOM_APIKEY_SIZE );
    strncpy(config.jeedom.adco,   server.arg("jdom_adco").c_str(),CFG_JDOM_ADCO_SIZE );
    itemp = server.arg("jdom_port").toInt();
    config.jeedom.port = (itemp>=0 && itemp<=65535) ? itemp : CFG_JDOM_DEFAULT_PORT ; 
    itemp = server.arg("jdom_freq").toInt();
    if (itemp>0 && itemp<=86400){
      // jeedom Update if needed
      Tick_jeedom.detach();
      Tick_jeedom.attach(itemp, Task_jeedom);
    } else {
      itemp = 0 ; 
    }
    config.jeedom.freq = itemp;

    // domoticz
    strncpy(config.domoticz.host,   server.arg("dmcz_host").c_str(),  CFG_DMCZ_HOST_SIZE );
    strncpy(config.domoticz.url,    server.arg("dmcz_url").c_str(),   CFG_DMCZ_URL_SIZE );
    strncpy(config.domoticz.usr,    server.arg("dmcz_usr").c_str(),   CFG_DMCZ_USR_SIZE );
    strncpy(config.domoticz.pwd,    server.arg("dmcz_pwd").c_str(),   CFG_DMCZ_PWD_SIZE );
    itemp = server.arg("dmcz_idx_txt").toInt();
    config.domoticz.idx_txt = itemp; 
    
    itemp = server.arg("dmcz_idx_p1sm").toInt();
    config.domoticz.idx_p1sm = itemp; 
    
    itemp = server.arg("dmcz_idx_crt").toInt();
    config.domoticz.idx_crt = itemp; 
    
    itemp = server.arg("dmcz_idx_elec").toInt();
    config.domoticz.idx_elec = itemp; 
    
    itemp = server.arg("dmcz_idx_kwh").toInt();
    config.domoticz.idx_kwh = itemp; 
    
    itemp = server.arg("dmcz_idx_pct").toInt();
    config.domoticz.idx_pct = itemp; 
    
    itemp = server.arg("dmcz_port").toInt();
    config.domoticz.port = (itemp>=0 && itemp<=65535) ? itemp : CFG_DMCZ_DEFAULT_PORT ; 
    itemp = server.arg("dmcz_freq").toInt();
    if (itemp>0 && itemp<=86400){
      // domoticz Update if needed
      Tick_domoticz.detach();
      Tick_domoticz.attach(itemp, Task_domoticz);
    } else {
      itemp = 0 ; 
    }
    config.domoticz.freq = itemp;

    if ( saveConfig() ) {
      ret = 200;
      response = "OK";
    } else {
      ret = 412;
      response = "Unable to save configuration";
    }

    showConfig();
  }
  else
  {
    ret = 400;
    response = "Missing Form Field";
  }

  DebugF("Sending response "); 
  Debug(ret); 
  Debug(":"); 
  Debugln(response); 
  server.send ( ret, "text/plain", response);
  LedBluOFF();
}

/* ======================================================================
Function: handleRoot 
Purpose : handle main page /
Input   : -
Output  : - 
Comments: -
====================================================================== */
void handleRoot(void) 
{
  LedBluON();
  handleFileRead("/");
  LedBluOFF();
}

/* ======================================================================
Function: formatNumberJSON 
Purpose : check if data value is full number and send correct JSON format
Input   : String where to add response
          char * value to check 
Output  : - 
Comments: 00150 => 150
          ADCO  => "ADCO"
          1     => 1
====================================================================== */
void formatNumberJSON( String &response, char * value)
{
  // we have at least something ?
  if (value && strlen(value))
  {
    boolean isNumber = true;
    uint8_t c;
    char * p = value;

    // just to be sure
    if (strlen(p)<=16) {
      // check if value is number
      while (*p && isNumber) {
        if ( *p < '0' || *p > '9' )
          isNumber = false;
        p++;
      }

      // this will add "" on not number values
      if (!isNumber) {
        response += '\"' ;
        response += value ;
        response += F("\"") ;
      } else {
        // this will remove leading zero on numbers
        p = value;
        while (*p=='0' && *(p+1) )
          p++;
        response += p ;
      }
    } else {
      Debugln(F("formatNumberJSON error!"));
    }
  }
}


/* ======================================================================
Function: tinfoJSONTable 
Purpose : dump all teleinfo values in JSON table format for browser
Input   : linked list pointer on the concerned data
          true to dump all values, false for only modified ones
Output  : - 
Comments: -
====================================================================== */
void tinfoJSONTable(void)
{
  ValueList * me = tinfo.getList();
  String response = "";

  // Just to debug where we are
  Debug(F("Serving /tinfo page...\r\n"));

  // Got at least one ?
  if (me) {
    uint8_t index=0;

    boolean first_item = true;
    // Json start
    response += F("[\r\n");

    // Loop thru the node
    while (me->next) {

      // we're there
      ESP.wdtFeed();

      // go to next node
      me = me->next;

      // First item do not add , separator
      if (first_item)
        first_item = false;
      else
        response += F(",\r\n");

/*
      Debug(F("(")) ;
      Debug(++index) ;
      Debug(F(") ")) ;

      if (me->name) Debug(me->name) ;
      else Debug(F("NULL")) ;

      Debug(F("=")) ;

      if (me->value) Debug(me->value) ;
      else Debug(F("NULL")) ;

      Debug(F(" '")) ;
      Debug(me->checksum) ;
      Debug(F("' ")); 

      // Flags management
      if ( me->flags) {
        Debug(F("Flags:0x")); 
        Debugf("%02X => ", me->flags); 
        if ( me->flags & TINFO_FLAGS_EXIST)
          Debug(F("Exist ")) ;
        if ( me->flags & TINFO_FLAGS_UPDATED)
          Debug(F("Updated ")) ;
        if ( me->flags & TINFO_FLAGS_ADDED)
          Debug(F("New ")) ;
      }

      Debugln() ;
*/
      response += F("{\"na\":\"");
      response +=  me->name ;
      response += F("\", \"va\":\"") ;
      response += me->value;
      response += F("\", \"ck\":\"") ;
      if (me->checksum == '"' || me->checksum == '\\' || me->checksum == '/')
        response += '\\';
      response += (char) me->checksum;
      response += F("\", \"fl\":");
      response += me->flags ;
      response += '}' ;

    }
   // Json end
   response += F("\r\n]");

  } else {
    Debugln(F("sending 404..."));
    server.send ( 404, "text/plain", "No data" );
  }
  Debug(F("sending..."));
  server.send ( 200, "text/json", response );
  Debugln(F("OK!"));
}


/* ======================================================================
Function: logJSONTable 
Purpose : dump all log values in JSON table format for browser
Input   : -
Output  : - 
Comments: -
====================================================================== */
void logJSONTable(void)
{
  String response = "";
  bool first = true;

  // Just to debug where we are
  Debug(F("Serving /log page...\r\n"));

    // Json start
    response += F("[\r\n");

    if (config.config & CFG_INFO) 
    {

        if (SPIFFS.exists("/log.txt"))
        {
          String temp = "";
          
          File f = SPIFFS.open("/log.txt", "r");
          while (f.available()){
           
            String tempwhile = "";
            String line = f.readStringUntil('\n');
            tempwhile += ",{\"ev\":\"";
            tempwhile += line.substring(0,line.length()-1);
            tempwhile += "\"}\r\n";
            temp = tempwhile + temp;
            
          }
          f.close();

          response += ( first ? temp.substring(1,temp.length()-1) : temp );
        }
        
        if (SPIFFS.exists("/log.1"))
        {
          String temp = "";
          
          File f = SPIFFS.open("/log.1", "r");
          while (f.available()){

            String tempwhile = "";
            String line = f.readStringUntil('\n');
            tempwhile += ",{\"ev\":\"";
            tempwhile += line.substring(0,line.length()-1);
            tempwhile += "\"}\r\n";
            temp = tempwhile + temp;
            
          }
          f.close();

          response += ( first ? temp.substring(1,temp.length()-1) : temp );
        }
    }
    else
    {
        response += F("{\"ev\":\"");
        response +=  "Fonctionnalité non activée" ;
        response += "\"}" ;
    }
    

   // Json end
   response += F("]");
   
  Debug(F("sending..."));
  server.send ( 200, "text/json", response );
  Debugln(F("OK!"));
}


/* ======================================================================
Function: getSysJSONData 
Purpose : Return JSON string containing system data
Input   : Response String
Output  : - 
Comments: -
====================================================================== */
void getSysJSONData(String & response)
{
  response = "";
  char buffer[32];
  int32_t adc = ( 1000 * analogRead(A0) / 1024 );

  // Json start
  response += F("[\r\n");

  response += "{\"na\":\"Uptime\",\"va\":\"";
  response += sysinfo.sys_uptime;
  response += "\"},\r\n";

  response += "{\"na\":\"WifInfo Version\",\"va\":\"" WIFINFO_VERSION "\"},\r\n";

  response += "{\"na\":\"Compile le\",\"va\":\"" __DATE__ " " __TIME__ "\"},\r\n";

  response += "{\"na\":\"SDK Version\",\"va\":\"";
  response += system_get_sdk_version() ;
  response += "\"},\r\n";

  response += "{\"na\":\"Chip ID\",\"va\":\"";
  sprintf_P(buffer, "0x%0X",system_get_chip_id() );
  response += buffer ;
  response += "\"},\r\n";

  response += "{\"na\":\"Boot Version\",\"va\":\"";
  sprintf_P(buffer, "0x%0X",system_get_boot_version() );
  response += buffer ;
  response += "\"},\r\n";

  response += "{\"na\":\"Flash Real Size\",\"va\":\"";
  response += formatSize(ESP.getFlashChipRealSize()) ;
  response += "\"},\r\n";

  response += "{\"na\":\"Firmware Size\",\"va\":\"";
  response += formatSize(ESP.getSketchSize()) ;
  response += "\"},\r\n";

  response += "{\"na\":\"Free Size\",\"va\":\"";
  response += formatSize(ESP.getFreeSketchSpace()) ;
  response += "\"},\r\n";

  response += "{\"na\":\"Analog\",\"va\":\"";
  adc = ( (1000 * analogRead(A0)) / 1024);
  sprintf_P( buffer, PSTR("%d mV"), adc);
  response += buffer ;
  response += "\"},\r\n";

  FSInfo info;
  SPIFFS.info(info);

  response += "{\"na\":\"SPIFFS Total\",\"va\":\"";
  response += formatSize(info.totalBytes) ;
  response += "\"},\r\n";

  response += "{\"na\":\"SPIFFS Used\",\"va\":\"";
  response += formatSize(info.usedBytes) ;
  response += "\"},\r\n";

  response += "{\"na\":\"SPIFFS Occupation\",\"va\":\"";
  sprintf_P(buffer, "%d%%",100*info.usedBytes/info.totalBytes);
  response += buffer ;
  response += "\"},\r\n"; 

  // Free mem should be last one 
  response += "{\"na\":\"Free Ram\",\"va\":\"";
  response += formatSize(system_get_free_heap_size()) ;
  response += "\"}\r\n"; // Last don't have comma at end

  // Json end
  response += F("]\r\n");
}

/* ======================================================================
Function: sysJSONTable 
Purpose : dump all sysinfo values in JSON table format for browser
Input   : -
Output  : - 
Comments: -
====================================================================== */
void sysJSONTable()
{
  String response = "";
  
  getSysJSONData(response);

  // Just to debug where we are
  Debug(F("Serving /system page..."));
  server.send ( 200, "text/json", response );
  Debugln(F("Ok!"));
}



/* ======================================================================
Function: getConfigJSONData 
Purpose : Return JSON string containing configuration data
Input   : Response String
Output  : - 
Comments: -
====================================================================== */
void getConfJSONData(String & r)
{
  // Json start
  r = FPSTR(FP_JSON_START); 

  r+="\"";
  r+=CFG_FORM_SSID;      r+=FPSTR(FP_QCQ); r+=config.ssid;           r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_PSK;       r+=FPSTR(FP_QCQ); r+=config.psk;            r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_HOST;      r+=FPSTR(FP_QCQ); r+=config.host;           r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_AP_PSK;    r+=FPSTR(FP_QCQ); r+=config.ap_psk;         r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_AP_RETCNT; r+=FPSTR(FP_QCQ); r+=config.ap_retrycount;  r+= FPSTR(FP_QCNL);
  
  r+=CFG_FORM_EMON_HOST; r+=FPSTR(FP_QCQ); r+=config.emoncms.host;   r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_EMON_PORT; r+=FPSTR(FP_QCQ); r+=config.emoncms.port;   r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_EMON_URL;  r+=FPSTR(FP_QCQ); r+=config.emoncms.url;    r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_EMON_KEY;  r+=FPSTR(FP_QCQ); r+=config.emoncms.apikey; r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_EMON_NODE; r+=FPSTR(FP_QCQ); r+=config.emoncms.node;   r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_EMON_FREQ; r+=FPSTR(FP_QCQ); r+=config.emoncms.freq;   r+= FPSTR(FP_QCNL); 
  
  r+=CFG_FORM_OTA_AUTH;  r+=FPSTR(FP_QCQ); r+=config.ota_auth;       r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_OTA_PORT;  r+=FPSTR(FP_QCQ); r+=config.ota_port;       r+= FPSTR(FP_QCNL);
  
  if (config.config & CFG_DEBUG)   { r+=CFG_FORM_CFG_DEBUG;  r+=FPSTR(FP_QCQ); r+= FPSTR(FP_QCNL); }
  if (config.config & CFG_INFO)    { r+=CFG_FORM_CFG_INFO;   r+=FPSTR(FP_QCQ); r+= FPSTR(FP_QCNL); }
  if (config.config & CFG_RGB_LED) { r+=CFG_FORM_CFG_RGB;    r+=FPSTR(FP_QCQ); r+= FPSTR(FP_QCNL); }
  if (config.config & CFG_LCD)     { r+=CFG_FORM_CFG_OLED;   r+=FPSTR(FP_QCQ); r+= FPSTR(FP_QCNL); }

  r+=CFG_FORM_DMCZ_HOST;     r+=FPSTR(FP_QCQ); r+=config.domoticz.host;     r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_DMCZ_PORT;     r+=FPSTR(FP_QCQ); r+=config.domoticz.port;     r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_DMCZ_URL;      r+=FPSTR(FP_QCQ); r+=config.domoticz.url;      r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_DMCZ_USR;      r+=FPSTR(FP_QCQ); r+=config.domoticz.usr;      r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_DMCZ_PWD;      r+=FPSTR(FP_QCQ); r+=config.domoticz.pwd;      r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_DMCZ_IDX_TXT;  r+=FPSTR(FP_QCQ); r+=config.domoticz.idx_txt;  r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_DMCZ_IDX_P1SM; r+=FPSTR(FP_QCQ); r+=config.domoticz.idx_p1sm; r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_DMCZ_IDX_CRT;  r+=FPSTR(FP_QCQ); r+=config.domoticz.idx_crt;  r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_DMCZ_IDX_ELEC; r+=FPSTR(FP_QCQ); r+=config.domoticz.idx_elec; r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_DMCZ_IDX_KWH;  r+=FPSTR(FP_QCQ); r+=config.domoticz.idx_kwh;  r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_DMCZ_IDX_PCT;  r+=FPSTR(FP_QCQ); r+=config.domoticz.idx_pct;  r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_DMCZ_FREQ;     r+=FPSTR(FP_QCQ); r+=config.domoticz.freq;     r+= FPSTR(FP_QCNL); 

  r+=CFG_FORM_JDOM_HOST; r+=FPSTR(FP_QCQ); r+=config.jeedom.host;   r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_JDOM_PORT; r+=FPSTR(FP_QCQ); r+=config.jeedom.port;   r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_JDOM_URL;  r+=FPSTR(FP_QCQ); r+=config.jeedom.url;    r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_JDOM_KEY;  r+=FPSTR(FP_QCQ); r+=config.jeedom.apikey; r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_JDOM_ADCO; r+=FPSTR(FP_QCQ); r+=config.jeedom.adco;   r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_JDOM_FREQ; r+=FPSTR(FP_QCQ); r+=config.jeedom.freq;  

  r+= F("\""); 
  // Json end
  r += FPSTR(FP_JSON_END);

}

/* ======================================================================
Function: confJSONTable 
Purpose : dump all config values in JSON table format for browser
Input   : -
Output  : - 
Comments: -
====================================================================== */
void confJSONTable()
{
  String response = "";
  getConfJSONData(response);
  // Just to debug where we are
  Debug(F("Serving /config page..."));
  server.send ( 200, "text/json", response );
  Debugln(F("Ok!"));
}

/* ======================================================================
Function: getSpiffsJSONData 
Purpose : Return JSON string containing list of SPIFFS files
Input   : Response String
Output  : - 
Comments: -
====================================================================== */
void getSpiffsJSONData(String & response)
{
  char buffer[32];
  bool first_item = true;

  // Json start
  response = FPSTR(FP_JSON_START);

  // Files Array  
  response += F("\"files\":[\r\n");

  // Loop trough all files
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {    
    String fileName = dir.fileName();
    size_t fileSize = dir.fileSize();
    if (first_item)  
      first_item=false;
    else
      response += ",";

    response += F("{\"na\":\"");
    response += fileName.c_str();
    response += F("\",\"va\":\"");
    response += fileSize;
    response += F("\",\"act\":\"");
    response += fileName.c_str();
    response += F("\"}\r\n");
  }
  response += F("],\r\n");


  // SPIFFS File system array
  response += F("\"spiffs\":[\r\n{");
  
  // Get SPIFFS File system informations
  FSInfo info;
  SPIFFS.info(info);
  response += F("\"Total\":");
  response += info.totalBytes ;
  response += F(", \"Used\":");
  response += info.usedBytes ;
  response += F(", \"Ram\":");
  response += system_get_free_heap_size() ;
  response += F("}\r\n]"); 

  // Json end
  response += FPSTR(FP_JSON_END);
}

/* ======================================================================
Function: spiffsJSONTable 
Purpose : dump all spiffs system in JSON table format for browser
Input   : -
Output  : - 
Comments: -
====================================================================== */
void spiffsJSONTable()
{
  String response = "";
  getSpiffsJSONData(response);
  server.send ( 200, "text/json", response );
}

/* ======================================================================
Function: sendJSON 
Purpose : dump all values in JSON
Input   : linked list pointer on the concerned data
          true to dump all values, false for only modified ones
Output  : - 
Comments: -
====================================================================== */
void sendJSON(void)
{
  ValueList * me = tinfo.getList();
  String response = "";
  
  // Got at least one ?
  if (me) {
    // Json start
    response += FPSTR(FP_JSON_START);
    response += F("\"_UPTIME\":");
    response += seconds;

    // Loop thru the node
    while (me->next) {
      // go to next node
      me = me->next;
      response += F(",\"") ;
      response += me->name ;
      response += F("\":") ;
      formatNumberJSON(response, me->value);
    }
   // Json end
   response += FPSTR(FP_JSON_END) ;

  } else {
    server.send ( 404, "text/plain", "No data" );
  }
  server.send ( 200, "text/json", response );
}


/* ======================================================================
Function: wifiScanJSON 
Purpose : scan Wifi Access Point and return JSON code
Input   : -
Output  : - 
Comments: -
====================================================================== */
void wifiScanJSON(void)
{
  String response = "";
  bool first = true;

  // Just to debug where we are
  Debug(F("Serving /wifiscan page..."));

  int n = WiFi.scanNetworks();

  // Json start
  response += F("[\r\n");

  for (uint8_t i = 0; i < n; ++i)
  {
    int8_t rssi = WiFi.RSSI(i);
    
    uint8_t percent;

    // dBm to Quality
    if(rssi<=-100)      percent = 0;
    else if (rssi>=-50) percent = 100;
    else                percent = 2 * (rssi + 100);

    if (first) 
      first = false;
    else
      response += F(",");

    response += F("{\"ssid\":\"");
    response += WiFi.SSID(i);
    response += F("\",\"rssi\":") ;
    response += rssi;
    response += FPSTR(FP_JSON_END);
  }

  // Json end
  response += FPSTR("]\r\n");

  Debug(F("sending..."));
  server.send ( 200, "text/json", response );
  Debugln(F("Ok!"));
}

/* ======================================================================
Function: handleFactoryReset 
Purpose : reset the module to factory settingd
Input   : -
Output  : - 
Comments: -
====================================================================== */
void handleFactoryReset(void)
{
  // Just to debug where we are
  Debug(F("Serving /factory_reset page..."));
  ResetConfig();
  ESP.eraseConfig();
  Debug(F("sending..."));
  server.send ( 200, "text/plain", FPSTR(FP_RESTART) );
  Debugln(F("Ok!"));
  delay(1000);
  ESP.restart();
  while (true)
    delay(1);
}

/* ======================================================================
Function: handleReset 
Purpose : reset the module
Input   : -
Output  : - 
Comments: -
====================================================================== */
void handleReset(void)
{
  // Just to debug where we are
  Debug(F("Serving /reset page..."));
  Debug(F("sending..."));
  server.send ( 200, "text/plain", FPSTR(FP_RESTART) );
  Debugln(F("Ok!"));
  delay(1000);
  ESP.restart();
  while (true)
    delay(1);

}


/* ======================================================================
Function: handleNotFound 
Purpose : default WEB routing when URI is not found
Input   : -
Output  : - 
Comments: -
====================================================================== */
void handleNotFound(void) 
{
  String response = "";
  boolean found = false;  

  // Led on
  LedBluON();

  // try to return SPIFFS file
  found = handleFileRead(server.uri());

  // Try Teleinfo ETIQUETTE
  if (!found) {
    // We check for an known label
    ValueList * me = tinfo.getList();
    const char * uri;
    // convert uri to char * for compare
    uri = server.uri().c_str();

    Debugf("handleNotFound(%s)\r\n", uri);

    // Got at least one and consistent URI ?
    if (me && uri && *uri=='/' && *++uri ) {
      
      // Loop thru the linked list of values
      while (me->next && !found) {

        // go to next node
        me = me->next;

        //Debugf("compare to '%s' ", me->name);
        // Do we have this one ?
        if (strcmp (me->name, uri) == 0 )
        {
          // no need to continue
          found = true;

          // Add to respone
          response += F("{\"") ;
          response += me->name ;
          response += F("\":") ;
          formatNumberJSON(response, me->value);
          response += F("}\r\n");
        }
      }
    }

    // Got it, send json
    if (found) 
      server.send ( 200, "text/json", response );
  }

  // All trys failed
  if (!found) {
    // send error message in plain text
    String message = F("File Not Found\n\n");
    message += F("URI: ");
    message += server.uri();
    message += F("\nMethod: ");
    message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
    message += F("\nArguments: ");
    message += server.args();
    message += FPSTR(FP_NL);

    for ( uint8_t i = 0; i < server.args(); i++ ) {
      message += " " + server.argName ( i ) + ": " + server.arg ( i ) + FPSTR(FP_NL);
    }

    server.send ( 404, "text/plain", message );
  }

  // Led off
  LedBluOFF();
}
