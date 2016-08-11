/*
  WiFi Web Server using GET parameters

 A simple web server that handles GET parameters
 using a WiFi shield.

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 * WiFi shield attached (or MKR1000)
 * Your circuit

 created 11 August 2016
 by ab@outofpluto.com
 inspired by WifiWebServer bu dlf (Metodo2 srl) and Tom Igoe (31 May 2012)

 ### TO USE THIS FILE, UPDATE handle(params) ###
 This function is at the end of the file
 String handle(String params[][2])
	`params` are the GET parameters as an array of array of String:
 		[["param1", "value1"], ["param2", "value2"], ...]
 	returns a String that will be the content of the response
 */

#include <SPI.h>
#include <WiFi101.h>

char ssid[] = "yourssid";
char pass[] = "yourpassword";

int status = WL_IDLE_STATUS;

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

// EDIT ME!
String handle(String params[][2], int nb) {
  String concat;
  for (int cnt = 0; cnt < nb; cnt++) {
    concat.concat(params[cnt][0]);
    concat.concat(String(" = "));
    concat.concat(params[cnt][1]);
    concat.concat("<br />");
  }
  return concat;
} 