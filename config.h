// **********************************************************************************
// ESP8266 Teleinfo WEB Server configuration Include file
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// For any explanation about teleinfo ou use , see my blog
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
#ifndef __CONFIG_H__
#define __CONFIG_H__

// Include main project include file
#include "Wifinfo.h"

#define CFG_SSID_SIZE 		32
#define CFG_PSK_SIZE  		64
#define CFG_HOSTNAME_SIZE 16
#define CFG_AP_DEFAULT_RETCNT 25

#define CFG_EMON_HOST_SIZE 		32
#define CFG_EMON_APIKEY_SIZE 	32
#define CFG_EMON_URL_SIZE 		32
#define CFG_EMON_DEFAULT_PORT 80
#define CFG_EMON_DEFAULT_HOST "emoncms.org"
#define CFG_EMON_DEFAULT_URL  "/input/post.json"

#define CFG_JDOM_HOST_SIZE    32
#define CFG_JDOM_APIKEY_SIZE  48
#define CFG_JDOM_URL_SIZE     64
#define CFG_JDOM_ADCO_SIZE    12
#define CFG_JDOM_DEFAULT_PORT 80
#define CFG_JDOM_DEFAULT_HOST "jeedom.local"
#define CFG_JDOM_DEFAULT_URL  "/jeedom/plugins/teleinfo/core/php/jeeTeleinfo.php"
#define CFG_JDOM_DEFAULT_ADCO "0000111122223333"

#define CFG_DMCZ_HOST_SIZE    32
#define CFG_DMCZ_URL_SIZE     64
#define CFG_DMCZ_USR_SIZE     32
#define CFG_DMCZ_PWD_SIZE     32
#define CFG_DMCZ_DEFAULT_PORT 80
#define CFG_DMCZ_DEFAULT_HOST "domoticz.local"
#define CFG_DMCZ_DEFAULT_URL  "/json.htm"

// Port pour l'OTA
#define DEFAULT_OTA_PORT     8266
#define DEFAULT_OTA_AUTH     "OTA_WifInfo"
//#define DEFAULT_OTA_AUTH     ""

// Bit definition for different configuration modes
#define CFG_LCD				  0x0001	// Enable display
#define CFG_DEBUG			  0x0002	// Enable serial debug
#define CFG_RGB_LED     0x0004  // Enable RGB LED
#define CFG_INFO        0x0008  // Enable serial & file info
#define CFG_BAD_CRC     0x8000  // Bad CRC when reading configuration

// Web Interface Configuration Form field names
#define CFG_FORM_SSID      FPSTR("ssid")
#define CFG_FORM_PSK       FPSTR("psk")
#define CFG_FORM_HOST      FPSTR("host")
#define CFG_FORM_AP_PSK    FPSTR("ap_psk")
#define CFG_FORM_AP_RETCNT FPSTR("ap_retrycount")
#define CFG_FORM_OTA_AUTH  FPSTR("ota_auth")
#define CFG_FORM_OTA_PORT  FPSTR("ota_port")
#define CFG_FORM_CFG_DEBUG FPSTR("cfg_debug")
#define CFG_FORM_CFG_INFO  FPSTR("cfg_info")
#define CFG_FORM_CFG_RGB   FPSTR("cfg_rgb")
#define CFG_FORM_CFG_OLED  FPSTR("cfg_oled")

#define CFG_FORM_EMON_HOST  FPSTR("emon_host")
#define CFG_FORM_EMON_PORT  FPSTR("emon_port")
#define CFG_FORM_EMON_URL   FPSTR("emon_url")
#define CFG_FORM_EMON_KEY   FPSTR("emon_apikey")
#define CFG_FORM_EMON_NODE  FPSTR("emon_node")
#define CFG_FORM_EMON_FREQ  FPSTR("emon_freq")

#define CFG_FORM_JDOM_HOST  FPSTR("jdom_host")
#define CFG_FORM_JDOM_PORT  FPSTR("jdom_port")
#define CFG_FORM_JDOM_URL   FPSTR("jdom_url")
#define CFG_FORM_JDOM_KEY   FPSTR("jdom_apikey")
#define CFG_FORM_JDOM_ADCO  FPSTR("jdom_adco")
#define CFG_FORM_JDOM_FREQ  FPSTR("jdom_freq")

#define CFG_FORM_DMCZ_HOST      FPSTR("dmcz_host")
#define CFG_FORM_DMCZ_PORT      FPSTR("dmcz_port")
#define CFG_FORM_DMCZ_URL       FPSTR("dmcz_url")
#define CFG_FORM_DMCZ_USR       FPSTR("dmcz_usr")
#define CFG_FORM_DMCZ_PWD       FPSTR("dmcz_pwd")
#define CFG_FORM_DMCZ_IDX_TXT   FPSTR("dmcz_idx_txt")
#define CFG_FORM_DMCZ_IDX_P1SM  FPSTR("dmcz_idx_p1sm")
#define CFG_FORM_DMCZ_IDX_CRT   FPSTR("dmcz_idx_crt")
#define CFG_FORM_DMCZ_IDX_ELEC  FPSTR("dmcz_idx_elec")
#define CFG_FORM_DMCZ_IDX_KWH   FPSTR("dmcz_idx_kwh")
#define CFG_FORM_DMCZ_IDX_PCT   FPSTR("dmcz_idx_pct")
#define CFG_FORM_DMCZ_FREQ      FPSTR("dmcz_freq")

#define CFG_FORM_IP  FPSTR("wifi_ip");
#define CFG_FORM_GW  FPSTR("wifi_gw");
#define CFG_FORM_MSK FPSTR("wifi_msk");

#pragma pack(push)  // push current alignment to stack
#pragma pack(1)     // set alignment to 1 byte boundary

// Config for emoncms
// 256 Bytes
typedef struct 
{
  char  host[CFG_EMON_HOST_SIZE+1]; 		// FQDN (32+1=33 Bytes)
  char  apikey[CFG_EMON_APIKEY_SIZE+1]; // Secret (32+1=33 Bytes)
  char  url[CFG_EMON_URL_SIZE+1];  			// Post URL (32+1=33 Bytes)
  uint16_t port;    								    // Protocol port (HTTP/HTTPS) (2 Bytes)
  uint8_t  node;     									  // optional node (8 Bytes)
  uint32_t freq;                        // refresh rate (4 Bytes)
  uint8_t  filler[143];  							  // in case adding data in config avoiding loosing current conf by bad crc (143 Bytes)
} _emoncms;

// Config for jeedom
// 256 Bytes
typedef struct 
{
  char  host[CFG_JDOM_HOST_SIZE+1];     // FQDN (32+1=33 Bytes)
  char  apikey[CFG_JDOM_APIKEY_SIZE+1]; // Secret (48+1=49 Bytes)
  char  url[CFG_JDOM_URL_SIZE+1];       // Post URL (64+1=65 Bytes)
  char  adco[CFG_JDOM_ADCO_SIZE+1];     // Identifiant compteur (12+1=13 Bytes)
  uint16_t port;                        // Protocol port (HTTP/HTTPS) (2 Bytes)
  uint32_t freq;                        // refresh rate (4 Bytes)
  uint8_t filler[90];                   // in case adding data in config avoiding loosing current conf by bad crc (90 Bytes)
} _jeedom;

// Config for domoticz
// 256 Bytes
typedef struct 
{
  char  host[CFG_DMCZ_HOST_SIZE+1];     // FQDN (32+1=33 Bytes)
  char  usr[CFG_DMCZ_USR_SIZE+1];       // User (32+1=33 Bytes)
  char  pwd[CFG_DMCZ_PWD_SIZE+1];       // Password (32+1=33 Bytes)
  char  url[CFG_DMCZ_URL_SIZE+1];       // Post URL (64+1=65 Bytes)
  uint16_t port;                        // Protocol port (HTTP/HTTPS) (2 Bytes)
  uint32_t freq;                        // refresh rate (4 Bytes)
  uint16_t idx_txt;                     // Index device domoticz Text (2 Byte)
  uint16_t idx_p1sm;                    // Index device domoticz P1 Smart Meter (2 Bytes)
  uint16_t idx_crt;                     // Index device domoticz Current (2 Bytes)
  uint16_t idx_elec;                    // Index device domoticz Eletric (2 Byte)
  uint16_t idx_kwh;                     // Index device domoticz Kwh (2 Byte)
  uint16_t idx_pct;                     // Index device domoticz Percentage (2 Byte)
  uint8_t filler[74];                   // in case adding data in config avoiding loosing current conf by bad crc (74 Bytes)
} _domoticz;

// Config saved into eeprom
// 1024 bytes total including CRC
typedef struct 
{
  char  ssid[CFG_SSID_SIZE+1]; 		 // SSID (32+1=33 Bytes)
  char  psk[CFG_PSK_SIZE+1]; 		   // Pre shared key (64+1=65 Bytes)
  char  host[CFG_HOSTNAME_SIZE+1]; // Hostname (16+1=17 Bytes)
  char  ap_psk[CFG_PSK_SIZE+1];    // Access Point Pre shared key (64+1=65 Bytes)
  uint8_t ap_retrycount;           // Wifi connect retry count (1 Bytes)
  char  ota_auth[CFG_PSK_SIZE+1];  // OTA Authentication password (64+1=65 Bytes)
  uint32_t config;           		   // Bit field register (4 Bytes)
  uint16_t ota_port;         		   // OTA port (2 Bytes)
  uint8_t  filler[1];      		     // in case adding data in config avoiding loosing current conf by bad crc (1 Bytes)
  _emoncms emoncms;                // Emoncms configuration (256 Bytes)
  _jeedom  jeedom;                 // jeedom configuration (256 Bytes)
  _domoticz  domoticz;             // domoticz configuration (256 Bytes)
  uint8_t  filler1[1];             // Another filler in case we need more (1 Bytes)
  uint16_t crc;                    // CRC (2 Bytes)
} _Config;


// Exported variables/object instancied in main sketch
// ===================================================
extern _Config config;

#pragma pack(pop)
 
// Declared exported function from route.cpp
// ===================================================
bool readConfig(bool clear_on_error=true);
bool saveConfig(void);
void showConfig(void);


#endif 
