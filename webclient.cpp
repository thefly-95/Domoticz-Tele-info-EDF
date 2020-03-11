// **********************************************************************************
// ESP8266 Teleinfo WEB Client, web server function
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
// History : V1.00 2015-12-04 - First release
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

#include "webclient.h"

#include <map>
#include <string>

/* ======================================================================
Function: httpPost
Purpose : Do a http post
Input   : hostname
          port
          url
Output  : true if received 200 OK
Comments: -
====================================================================== */
boolean httpPost(char * host, uint16_t port, char * url)
{
    return httpPostBasicAuth(host, port, url, (char *)"", (char *)"");
}

boolean httpPostBasicAuth(char * host, uint16_t port, char * url, char * basicauthusr, char * basicauthpwd)
{
  HTTPClient http;
  //Ajout Olivier
  basicauthusr = config.domoticz.usr;
  basicauthpwd = config.domoticz.pwd;
  bool ret = false;

  unsigned long start = millis();

  // configure traged server and url
  http.begin(host, port, url); 
  if (basicauthusr != "" && basicauthpwd != "")
  {
    http.setAuthorization(basicauthusr, basicauthpwd);
  }
 //http.begin("http://Admin:2sc8fgzh@192.168.1.10:8084/json.htm?type=command&param=udevice&idx=459&nvalue=2&svalue=104");
  //http.begin("http://emoncms.org/input/post.json?node=20&apikey=2f13e4608d411d20354485f72747de7b&json={PAPP:100}");
  //http.begin("emoncms.org", 80, "/input/post.json?node=20&apikey=2f13e4608d411d20354485f72747de7b&json={}"); //HTTP

  Debugf("http%s://%s:%d%s => ", port==443?"s":"", host, port, url);

  // start connection and send HTTP header
  int httpCode = http.GET();
  if(httpCode) {
      // HTTP header has been send and Server response header has been handled
      Debug(httpCode);
      Debug(" ");
      // file found at server
      if(httpCode == 200) {
        String payload = http.getString();
        Debug(payload);
        ret = true;
      }
  } else {
      DebugF("failed!");
  }
  Debugf(" in %d ms\r\n",millis()-start);
  return ret;
}

/* ======================================================================
Function: emoncmsPost
Purpose : Do a http post to emoncms
Input   : 
Output  : true if post returned 200 OK
Comments: -
====================================================================== */
boolean emoncmsPost(void)
{
  boolean ret = false;

  // Some basic checking
  if (*config.emoncms.host) {
    ValueList * me = tinfo.getList();
    // Got at least one ?
    if (me && me->next) {
      String url ; 
      boolean first_item;

      url = *config.emoncms.url ? config.emoncms.url : "/";
      url += "?";
      if (config.emoncms.node>0) {
        url+= F("node=");
        url+= String(config.emoncms.node);
        url+= "&";
      } 

      url += F("apikey=") ;
      url += config.emoncms.apikey;
      url += F("&json={") ;

      first_item = true;

      // Loop thru the node
      while (me->next) {
        // go to next node
        me = me->next;
        // First item do not add , separator
        if (first_item)
          first_item = false;
        else
          url += ",";

        url +=  me->name ;
        url += ":" ;

        // EMONCMS ne sais traiter que des valeurs numériques, donc ici il faut faire une 
        // table de mappage, tout à fait arbitraire, mais c"est celle-ci dont je me sers 
        // depuis mes débuts avec la téléinfo
        if (!strcmp(me->name, "OPTARIF")) {
          // L'option tarifaire choisie (Groupe "OPTARIF") est codée sur 4 caractères alphanumériques 
          /* J'ai pris un nombre arbitraire codé dans l'ordre ci-dessous
          je mets le 4eme char à 0, trop de possibilités
          BASE => Option Base. 
          HC.. => Option Heures Creuses. 
          EJP. => Option EJP. 
          BBRx => Option Tempo
          */
          char * p = me->value;
            
               if (*p=='B'&&*(p+1)=='A'&&*(p+2)=='S') url += "1";
          else if (*p=='H'&&*(p+1)=='C'&&*(p+2)=='.') url += "2";
          else if (*p=='E'&&*(p+1)=='J'&&*(p+2)=='P') url += "3";
          else if (*p=='B'&&*(p+1)=='B'&&*(p+2)=='R') url += "4";
          else url +="0";
        } else if (!strcmp(me->name, "HHPHC")) {
          // L'horaire heures pleines/heures creuses (Groupe "HHPHC") est codé par un caractère A à Y 
          // J'ai choisi de prendre son code ASCII
          int code = *me->value;
          url += String(code);
        } else if (!strcmp(me->name, "PTEC")) {
          // La période tarifaire en cours (Groupe "PTEC"), est codée sur 4 caractères 
          /* J'ai pris un nombre arbitraire codé dans l'ordre ci-dessous
          TH.. => Toutes les Heures. 
          HC.. => Heures Creuses. 
          HP.. => Heures Pleines. 
          HN.. => Heures Normales. 
          PM.. => Heures de Pointe Mobile. 
          HCJB => Heures Creuses Jours Bleus. 
          HCJW => Heures Creuses Jours Blancs (White). 
          HCJR => Heures Creuses Jours Rouges. 
          HPJB => Heures Pleines Jours Bleus. 
          HPJW => Heures Pleines Jours Blancs (White). 
          HPJR => Heures Pleines Jours Rouges. 
          */
               if (!strcmp(me->value, "TH..")) url += "1";
          else if (!strcmp(me->value, "HC..")) url += "2";
          else if (!strcmp(me->value, "HP..")) url += "3";
          else if (!strcmp(me->value, "HN..")) url += "4";
          else if (!strcmp(me->value, "PM..")) url += "5";
          else if (!strcmp(me->value, "HCJB")) url += "6";
          else if (!strcmp(me->value, "HCJW")) url += "7";
          else if (!strcmp(me->value, "HCJR")) url += "8";
          else if (!strcmp(me->value, "HPJB")) url += "9";
          else if (!strcmp(me->value, "HPJW")) url += "10";
          else if (!strcmp(me->value, "HPJR")) url += "11";
          else url +="0";
        } else {
          url += me->value;
        }
      } // While me

      // Json end
      url += "}";

      ret = httpPost( config.emoncms.host, config.emoncms.port, (char *) url.c_str()) ;
    } // if me
  } // if host
  return ret;
}

/* ======================================================================
Function: jeedomPost
Purpose : Do a http post to jeedom server
Input   : 
Output  : true if post returned 200 OK
Comments: -
====================================================================== */
boolean jeedomPost(void)
{
  boolean ret = false;

  // Some basic checking
  if (*config.jeedom.host) {
    ValueList * me = tinfo.getList();
    // Got at least one ?
    if (me && me->next) {
      String url ; 
      boolean skip_item;

      url = *config.jeedom.url ? config.jeedom.url : "/";
      url += "?";

      // Config identifiant forcée ?
      if (*config.jeedom.adco) {
        url+= F("ADCO=");
        url+= config.jeedom.adco;
        url+= "&";
      } 

      url += F("api=") ;
      url += config.jeedom.apikey;
      url += F("&") ;

      // Loop thru the node
      while (me->next) {
        // go to next node
        me = me->next;
        skip_item = false;

        // Si ADCO déjà renseigné, on le remet pas
        if (!strcmp(me->name, "ADCO")) {
          if (*config.jeedom.adco)
            skip_item = true;
        }

        // Si Item virtuel, on le met pas
        if (*me->name =='_')
          skip_item = true;

        // On doit ajouter l'item ?
        if (!skip_item) {
          url +=  me->name ;
          url += "=" ;
          url +=  me->value;
          url += "&" ;
        }
      } // While me

      ret = httpPost( config.jeedom.host, config.jeedom.port, (char *) url.c_str()) ;
    } // if me
  } // if host
  return ret;
}

/* ======================================================================
Function: domoticzPost
Purpose : Do a http post to domoticz server
Input   : 
Output  : true if post returned 200 OK
Comments: -
http://192.168.1.27/json
"_UPTIME":89366,"MOTDETAT":0,"ADCO":61964942782,"OPTARIF":"HC..","ISOUSC":45,"HCHC":296247,"HCHP":294889,"PTEC":"HC..","IINST":20,"IMAX":90,"PAPP":4630,"HHPHC":"A"
====================================================================== */
boolean domoticzPost(void)
{
  boolean ret = true;

    // Some basic checking
  if (*config.domoticz.host) {
    ValueList * me = tinfo.getList();
    std::map<std::string, std::string>  meMap;

    String baseurl;
    String url;
    baseurl = *config.domoticz.url ? config.domoticz.url : "/";    
    baseurl += F("?type=command&param=udevice&"); 
      
    // Got at least one ?
    if (me && me->next) {
      // Loop thru the node
      while (me->next) {
        // go to next node
        me = me->next;
        // Si Item virtuel, on le met pas
        if (*me->name =='_')
        {
          //Nothing
        }
        else
        {
          meMap[me->name] = me->value;
        }
        
      } // While me

    
    /* Remplacer par un interrupteur ci dessous
    // /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=TXT
      if(config.domoticz.idx_txt > 0)
      {
          url = baseurl;
          url += "idx=";
          url += config.domoticz.idx_txt;
          url += "&nvalue=0";
          url += "&svalue=";
          url += meMap["ADCO"].c_str();

          if(!httpPost( config.domoticz.host, config.domoticz.port, (char *) url.c_str()))
          {
            ret = false;
          }
      */

//Debug_OV
//json.htm?type=command&param=switchlight&idx=99&switchcmd=On
      if(config.domoticz.idx_txt > 0)
      {
          url = *config.domoticz.url ? config.domoticz.url : "/";   
          url += F("?type=command&param=switchlight&");       
          url += "idx=";
          url += config.domoticz.idx_txt;
          url += "&switchcmd="; 
          if (!strcmp(meMap["PTEC"].c_str(), "HP..")) url += "On";
          if (!strcmp(meMap["PTEC"].c_str(), "HC..")) url += "Off";
          
          if(!httpPost( config.domoticz.host, config.domoticz.port, (char *) url.c_str()))
          {
            ret = false;
          }
      }
//Debug_OV      

    // HCHP,HCHC,0,0,PAPP,0
   // /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=USAGE1;USAGE2;RETURN1;RETURN2;CONS;PROD
      if(config.domoticz.idx_p1sm > 0)
      {
          url = baseurl;
          url += "idx=";
          url += config.domoticz.idx_p1sm;
          url += "&nvalue=0";
          url += "&svalue=";
          url += String(atoi(meMap["HCHP"].c_str())).c_str();
          url += ";";
          url += String(atoi(meMap["HCHC"].c_str())).c_str();
          url += ";0;0;";
          url += String(atoi(meMap["PAPP"].c_str())).c_str();
          url += ";0";
          if(!httpPost( config.domoticz.host, config.domoticz.port, (char *) url.c_str()))
          {
            ret = false;
          }
       }
       /*
      // /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=USAGE1;USAGE2;RETURN1;RETURN2;CONS;PROD
      if(config.domoticz.idx_p1sm > 0)
      {
          url = baseurl;
          url += "idx=";
          url += config.domoticz.idx_p1sm;
          url += "&nvalue=0";
          url += "&svalue=";
          url += String(atoi(meMap["BASE"].c_str())).c_str();
          url += ";0;0;0;";
          url += String(atoi(meMap["PAPP"].c_str())).c_str();
          url += ";0";

          if(!httpPost( config.domoticz.host, config.domoticz.port, (char *) url.c_str()))
          {
            ret = false;
          }
       }
*/
  
  
      
      // /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=ENERGY
      if(config.domoticz.idx_crt > 0)
      {
          url = baseurl;
          url += "idx=";
          url += config.domoticz.idx_crt;
          url += "&nvalue=0";
          url += "&svalue=";
          url += String(atoi(meMap["IINST"].c_str())).c_str();

          if(!httpPost( config.domoticz.host, config.domoticz.port, (char *) url.c_str()))
          {
            ret = false;
          }

          /*
          Info(config.domoticz.host);
          InfoF(":");
          Info(config.domoticz.port);
          Infoln((char *) url.c_str());
          InfoF("ret=");
          Infoln(ret);
          Infoflush();
          */
      }

      // /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=ENERGY
      if(config.domoticz.idx_elec > 0)
      {
          url = baseurl;
          url += "idx=";
          url += config.domoticz.idx_elec;
          url += "&nvalue=0";
          url += "&svalue=";
          url += String(atoi(meMap["PAPP"].c_str())).c_str();

          if(!httpPost( config.domoticz.host, config.domoticz.port, (char *) url.c_str()))
          {
            ret = false;
          }
      }

      // /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=POWER,ENERGY
      if(config.domoticz.idx_kwh > 0)
      {
          url = baseurl;
          url += "idx=";
          url += config.domoticz.idx_kwh;
          url += "&nvalue=0";
          url += "&svalue=";
          url += String(atoi(meMap["PAPP"].c_str())).c_str();
          url += ";0";
          //url += String(atoi(meMap["IINST"].c_str())).c_str(); Computed by Domoticz

          if(!httpPost( config.domoticz.host, config.domoticz.port, (char *) url.c_str()))
          {
            ret = false;
          }
      }
      // /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=PERCENTAGE
      if(config.domoticz.idx_pct > 0)
      {
          url = baseurl;
          url += "idx=";
          url += config.domoticz.idx_pct;
          url += "&nvalue=0";
          url += "&svalue=";
          url += String( roundf((atof(meMap["IINST"].c_str())* 100) / atof(meMap["ISOUSC"].c_str()) * 100) / 100 ).c_str();

          if(!httpPost( config.domoticz.host, config.domoticz.port, (char *) url.c_str()))
          {
            ret = false;
          }
      }
      
    } // if me
  }
  return ret;
}
