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

char ssid[] = "yourssid";      // your network SSID (name)
char pass[] = "yourpassword";   // your network password

int status = WL_IDLE_STATUS;

WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
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
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
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
  // cut GET parameters from HTTP request
  // (like GET /?c=1&d=2 HTTP=1.1 ---> c=1&d=2)
  msg = msg.substring(msg.indexOf("?")+1);
  msg = msg.substring(0, msg.indexOf(" "));

  int eq = 0;
  int nb = -1;
  // compute the number of params (counting '=' chars) 
  do {
    eq = msg.indexOf("=", eq+1);
    nb += 1; 
  } while (eq != -1);

  Serial.print("found ");
  Serial.print(nb);
  Serial.println(" parameters");

  return nb;
}

void readGET(String msg, String params[][2], int nb) {
  // cut GET parameters from HTTP request
  // (like GET /?c=1&d=2 HTTP=1.1 ---> c=1&d=2)
  msg = msg.substring(msg.indexOf("?")+1);
  msg = msg.substring(0, msg.indexOf(" "));

   Serial.print("read GET: ");
  Serial.println(msg.c_str());

  int eq = 0;
  int es = -1;
  // assign parameters ---> [["c", "1"], ["d", "2"]]
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
    Serial.println("new client");
    boolean currentLineIsBlank = true;
    String msg;
    String resp;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
	msg += c;
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          sendResponse(client, resp);
	  resp = String();
	  break;
        }
        if (c == '\n') {
          // you're starting a new line
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
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      } else {
	// if an empty request is sent, break loop
	break;
      }
    }
    // give the web browser time to receive the data
    delay(1);

    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

// EDIT ME!
String handle(String params[][2], int nb) {
  String concat;
  for (int cnt = 0; cnt < nb; cnt++) {
    concat.concat(params[cnt][0]);
    concat.concat(String(" = "));
    concat.concat(params[cnt][1]);
    concat.concat("\n");
  }
  Serial.println(concat.c_str());
  concat.replace("\n", "<br />");
  return concat;
} 