/*
Copyright (c) 2015-2020, Anatoli Arkhipenko.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software without
   specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _JSONCONFIG_H_
#define _JSONCONFIG_H_


#include <Arduino.h>

#ifdef _USE_DICTIONARY_
#include <Dictionary.h>
#endif

#if defined( ARDUINO_ARCH_ESP8266 )
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#endif

#if defined( ARDUINO_ARCH_ESP32 )
#include <WiFiClient.h>
#include <HTTPClient.h>
#endif

#define JSON_OK         0
#define JSON_ERR      (-1)
#define JSON_COMMA    (-2)
#define JSON_COLON    (-3)
#define JSON_QUOTE    (-4)
#define JSON_BCKSL    (-5)
#define JSON_HTTPERR  (-97)
#define JSON_NOWIFI   (-98)
#define JSON_EOF      (-99)

class JsonConfig {
  public:
    JsonConfig();
    ~JsonConfig();

#ifdef _USE_DICTIONARY_
    int8_t   parseHttp(const String aUrl, Dictionary& aDict, int aNum = 0);
#else
    int8_t   parseHttp(const String aUrl, char** aMap, int aNum);
#endif
};

static JsonConfig JSONConfig;

JsonConfig::JsonConfig() {}
JsonConfig::~JsonConfig() {}

#ifdef _USE_DICTIONARY_
int8_t JsonConfig::parseHttp(const String aUrl, Dictionary& aDict, int aNum) {
#else
int8_t JsonConfig::parseHttp(const String aUrl, char** aMap, int aNum) {
#endif
  WiFiClient client;
  HTTPClient http;
  String payload;

  if (WiFi.status() != WL_CONNECTED) return JSON_NOWIFI;
#ifdef _LIBDEBUG_
Serial.printf("JsonConfig: Connecting to: %s\n", aUrl.c_str());
#endif
  if (http.begin(client, aUrl)) {
    int httpCode = http.GET();
    // httpCode will be negative on error
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        payload = http.getString();

        bool insideQoute = false;
        bool nextVerbatim = false;
        bool isValue = false;
        const char* c = payload.c_str();
        int len = payload.length();
        int p = 0;
        String currentKey;
        String currentValue;

        for (int i = 0; i < len; i++, c++) {
          char toAdd = *c;
          if (nextVerbatim) {
            nextVerbatim = false;
          }
          else {
            // process all special cases: '\', '"', ':', and ','
            if (*c == '\\' ) {
              nextVerbatim = true;
              continue;
            }
            if (*c == '\"') {
              if (!insideQoute) {
                insideQoute = true;
                continue;
              }
              else {
                insideQoute = false;
                if (isValue) {
#ifdef _USE_DICTIONARY_
                  aDict(currentKey, currentValue);
#else
                  strcpy(aMap[p++], currentValue.c_str());
#endif
                  currentValue = String();
                  currentKey = String();
                  if (aNum > 0 && p >= aNum) break;
                }
              }
            }
            if (*c == '\n') {
              if ( insideQoute ) return JSON_QUOTE;
              if ( nextVerbatim ) return JSON_BCKSL;
            }
            if (!insideQoute) {
              if (*c == ':') {
                if ( isValue ) return JSON_COMMA; //missing comma probably
                isValue = true;
                continue;
              }
              if (*c == ',') {
                if ( !isValue ) return JSON_COLON; //missing colon probably
                isValue = false;
                continue;
              }
            }
          }
          if (insideQoute) {
            if (isValue) currentValue.concat(*c);
            else currentKey.concat(*c);
          }
        }
        if (insideQoute || nextVerbatim || (aNum > 0 && p < aNum )) return JSON_EOF;
        return JSON_OK;
      }
    }
    else {
      return httpCode;
    }
    http.end();
  }
  else {
    return JSON_HTTPERR;
  }
}


#endif // _JSONCONFIG_H_