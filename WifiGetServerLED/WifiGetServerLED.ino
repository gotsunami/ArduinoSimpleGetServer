/*
  WiFi Web Server using GET parameters

 A simple web server that handles GET parameters
 using a WiFi shield and turns on and off the onboard LED.

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 * WiFi shield attached (or MKR1000)

 created 11 August 2016
 by ab@outofpluto.com
 */

#include <SPI.h>
#include <WiFi101.h>

char ssid[] = "yourssid";
char pass[] = "yourpassword";

int status = WL_IDLE_STATUS;
int ledPin = 6;

WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }

  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  server.begin();
  printWifiStatus();

  // turns all LED on for a second
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(ledPin, LOW);
  printUsage();
}

void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void printUsage() {
  IPAddress ip = WiFi.localIP();
  Serial.println("Usage: use http://<ip>/?led=1 to turn on or http://<ip>/?led=0 to turn off");
}

void sendResponse(WiFiClient client, String resp) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println(resp.c_str());
  client.println("</html>");
}

int countGET(String msg) {
  msg = msg.substring(msg.indexOf("?")+1);
  msg = msg.substring(0, msg.indexOf(" "));

  int eq = 0;
  int nb = -1;
  do {
    eq = msg.indexOf("=", eq+1);
    nb += 1; 
  } while (eq != -1);

  return nb;
}

void readGET(String msg, String params[][2], int nb) {
  msg = msg.substring(msg.indexOf("?")+1);
  msg = msg.substring(0, msg.indexOf(" "));

  int eq = 0;
  int es = -1;
  for (int cnt = 0; cnt < nb; cnt++) {
    eq = msg.indexOf("=", es+1);
    params[cnt][0] = msg.substring(es+1, eq);
    es = msg.indexOf("&", eq+1);
    if (es == -1) {
      params[cnt][1] = msg.substring(eq+1);
    } else {
      params[cnt][1] = msg.substring(eq+1, es);
    }
  }
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    delay(200);
    boolean currentLineIsBlank = true;
    String msg;
    String resp;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
	msg += c;
        if (c == '\n' && currentLineIsBlank) {
          sendResponse(client, resp);
	  resp = String();
	  break;
        }
        if (c == '\n') {
	  if (msg.startsWith("GET")) {
	    int nb = countGET(msg);
	    String params[nb][2];
	    readGET(msg, params, nb);
	    resp = handle(params, nb);
	  }
	  msg = "";
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          currentLineIsBlank = false;
        }
      } else {
	break;
      }
    }
    delay(1);

    client.stop();
  }
}

String handle(String params[][2], int nb) {
  String concat("LED is ");
  for (int cnt = 0; cnt < nb; cnt++) {
    if (params[cnt][0].compareTo("led") == 0) {
      int val = params[cnt][1].toInt();
      if (val == 1) {
	digitalWrite(ledPin, HIGH);
	concat.concat("ON<br />");
      } else {
	digitalWrite(ledPin, LOW);
	concat.concat("OFF<br />");
      }
    }
  }
  return concat;
} 