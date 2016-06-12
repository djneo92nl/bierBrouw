#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266SSDP.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>


byte webMode; //decides if we are in setup, normal or local only mode
String st; //WiFi Stations HTML list


//-----------------------------------standard arduino setup and loop-----------------------------------------------------------------------
void setup() {
  // put your setup code here, to run once:

  EEPROM.begin(512);
  //write a magic byte to eeprom 196 to determine if we've ever booted on this device before
  if (EEPROM.read(500) != 196) {
    //if not load default config files to EEPROM
    writeInitalConfig();
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}

void initWiFi() {
  Serial.println();
  Serial.println();
  Serial.println("Startup");
  esid.trim();
  if (webMode == 2) {
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, netMsk);
    WiFi.softAP(ssid);
    //    WiFi.begin((char*) ssid.c_str()); // not sure if need but works
    //dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    //dnsServer.start(DNS_PORT, "*", apIP);
    Serial.println("USP Server started");
    Serial.print("Access point started with name ");
    Serial.println(ssid);
    //server.on("/generate_204", handleRoot);  //Android captive
    server.onNotFound(handleRoot);
    launchWeb(2);
    return;

  }
  if (webMode == 1) {
    // test esid
    WiFi.disconnect();
    delay(100);
    WiFi.mode(WIFI_STA);
    Serial.print("Connecting to WiFi ");
    Serial.println(esid);
    WiFi.begin(esid.c_str(), epass.c_str());
    if ( testWifi() == 20 ) {
      launchWeb(1);
      return;
    }
  }
  logo();
  clockleds->Show();
  setupAP();
}


void setupAP(void) {

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");

  if (n == 0) {
    Serial.println("no networks found");
    st = "<label><input type='radio' name='ssid' value='No networks found' onClick='regularssid()'>No networks found</input></label><br>";
  } else {
    Serial.print(n);
    Serial.println("Networks found");
    st = "";
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " (OPEN)" : "*");

      // Print to web SSID and RSSI for each network found
      st += "<label><input type='radio' name='ssid' value='";
      st += WiFi.SSID(i);
      st += "' onClick='regularssid()'>";
      st += i + 1;
      st += ": ";
      st += WiFi.SSID(i);
      st += " (";
      st += WiFi.RSSI(i);
      st += ")";
      st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " (OPEN)" : "*";
      st += "</input></label><br>";
      delay(10);
    }
    //st += "</ul>";
  }
  Serial.println("");
  WiFi.disconnect();
  delay(100);

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(ssid);
  //WiFi.begin((char*) ssid.c_str()); // not sure if need but works
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);
  Serial.println("USP Server started");
  Serial.print("Access point started with name ");
  Serial.println(ssid);
  //WiFi.begin((char*) ssid.c_str()); // not sure if need but works
  Serial.print("Access point started with name ");
  Serial.println(ssid);
  launchWeb(0);
}
