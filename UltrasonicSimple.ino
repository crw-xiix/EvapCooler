#include <Wire.h>

/*
   Ultrasonic Simple
   Prints the distance read by an ultrasonic sensor in
   centimeters. They are supported to four pins ultrasound
   sensors (liek HC-SC04) and three pins (like PING)))
   and Seeed Studio sensors).

   The circuit:
 * * Module HR-SC04 (four pins) or PING))) (and other with
     three pins), attached to digital pins as follows:
   ---------------------    --------------------
   | HC-SC04 | Arduino |    | 3 pins | Arduino |
   ---------------------    --------------------
   |   Vcc   |   5V    |    |   Vcc  |   5V    |
   |   Trig  |   12    | OR |   SIG  |   13    |
   |   Echo  |   13    |    |   Gnd  |   GND   |
   |   Gnd   |   GND   |    --------------------
   ---------------------
   Note: You do not obligatorily need to use the pins defined above

   By default, the distance returned by the read()
   method is in centimeters. To get the distance in inches,
   pass INC as a parameter.
   Example: ultrasonic.read(INC)

   created 3 Apr 2014
   by Erick Simões (github: @ErickSimoes | twitter: @AloErickSimoes)
   modified 23 Jan 2017
   by Erick Simões (github: @ErickSimoes | twitter: @AloErickSimoes)
   modified 03 Mar 2017
   by Erick Simões (github: @ErickSimoes | twitter: @AloErickSimoes)
   modified 11 Jun 2018
   by Erick Simões (github: @ErickSimoes | twitter: @AloErickSimoes)

   This example code is released into the MIT License.
*/
#include <ESP8266WiFi.h>
#include "Ultrasonic.h"
#include "mcp.h"

//This draws the website, auto generated
#include "output.h"

#define TRIG 16
#define ECHO 14
#define ledPin 2

#ifndef _EMAIL_CREDS_
#define _EMAIL_CREDS_


#define CREDS_EMAILBASE64_LOGIN "dasdfsdasasdadfasfasdfa"
#define CREDS_EMAILBASE64_PASSWORD "asqwersdvasdfasdfafdas"
#define CREDS_FROM "asdfasdfaqwerwadfs@gmail.com"

#endif

const char *wifiHostName = "Chicken";
const char *wifiNetwork = "Wood";
const char *wifiPassword = "slipperywhendusty";


IPAddress ip(192, 168, 89, 100); // where xx is the desired IP Address
IPAddress gateway(192, 168, 89, 1); // set gateway to match your network
IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your

/*
   Pass as a parameter the trigger and echo pin, respectively,
   or only the signal pin (for sensors 3 pins), like:
   Ultrasonic ultrasonic(13);
*/
Ultrasonic ultrasonic(TRIG, ECHO);
int distance;

MCP9808 mcp = MCP9808();



WiFiServer server(80);


void setup() {

  int j[10];
  j[5] = 4;
  int p = 5[j];

  Serial.begin(9600);
  pinMode(TRIG, OUTPUT);
  pinMode(ledPin, OUTPUT);
  delay(100);
  Serial.print("SDA Pin:");
  Serial.println(SDA);
  Serial.print("SCL Pin:");
  Serial.println(SCL);

  if (!mcp.begin()) {
    Serial.println("Couldn't find MCP9808!");
  } else {
    Serial.println("Found MCP9808!");
  }


  WiFi.mode(WIFI_STA);

  WiFi.hostname(wifiHostName);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");

  WiFi.config(ip, gateway, subnet);
  WiFi.begin(wifiNetwork, wifiPassword);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(ledPin, LOW);
    delay(250);
    digitalWrite(ledPin, HIGH);
    delay(250);

  }

  // Start the server
  server.begin();
  Serial.println("Server started");

  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  /*
    Gsender *gsender = Gsender::Instance();    // Getting pointer to class instance
      String subject = "ESP8285 Water monitoring system";
      if(gsender->Subject(subject)->Send("charles@loneaspen.com", "The system has been restarted after a power down.")) {
          Serial.println("Message sent.");
      } else {
          Serial.print("Error sending message: ");
          Serial.println(gsender->getError());
      }
  */
}

int lastWaterLevel = 0;
float lastTemp = 0.0f;

void loop() {
  // Pass INC as a parameter to get the distance in inches


  /*
    pinMode(TRIG,OUTPUT);
    digitalWrite(TRIG,HIGH);
    delay(250);
    digitalWrite(TRIG,LOW);
  */


  mcp.readSensor(  );


  Serial.print("Temp:");
  Serial.println(mcp.getTemperature_F());
  lastTemp = mcp.getTemperature_F();



  distance = ultrasonic.read();

  Serial.print("Distance in CM: ");
  Serial.println(distance);
  lastWaterLevel = distance;

  //Serial.println("Waiting...");

  WiFiClient client = server.available();
  int lps = 0;

  while (client == NULL)
  {
    if (lps > 500) return;  //Skip wifi stuff.
    delay(1);
    client = server.available();
    lps++;
  }


  // Wait until the client sends some data
  Serial.println("new client");

  while (client.connected() && !client.available())
  {
    delay(1);
    lps++;
    if (lps > 500) return;  //Skip wifi stuff.
  }

  //Bail out, something failed.
  if (!client.connected()) return;

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Match the request
  int value = LOW;
  if (request.indexOf("/LED=ON") != -1) {
    digitalWrite(ledPin, HIGH);
    value = HIGH;
  }
  if (request.indexOf("/LED=OFF") != -1) {
    digitalWrite(ledPin, LOW);
    value = LOW;
  }
  if (request.indexOf("/Water") != -1) {
    client.print("Water: ");
    client.print(lastWaterLevel);
    client.println(" cm");
    Serial.println("Water Requested");
    return;
  }
  if (request.indexOf("/Temp") != -1) {
    client.print("Temp: ");
    client.print(lastTemp);
    client.println(" Fah");
    Serial.println("Temp Requested");
    return;
  }


  // Return the response

  client.println("<!doctype html>");

  client.println("<html lang=\"en\">");
  client.println("<head>");
  //<!-- Required meta tags -->
  client.println("<meta charset=\"utf-8\">");
  client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\">");

  //<!-- Bootstrap CSS -->
  client.println("<link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/css/bootstrap.min.css\" integrity=\"sha384-MCw98/SFnGE8fJT3GXwEOngsV7Zt27NXFoaoApmYm81iuXoPkFOJwJ8ERdknLPMO\" crossorigin=\"anonymous\">");

  client.println("<title>CRW System Monitor</title>");
  client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");
  client.println("<script src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.3/umd/popper.min.js\" integrity=\"sha384-ZMP7rVo3mIykV+2+9J3UJ46jBk0WLaUAdn689aCwoqbBJiSnjAK/l8WvCWPIPm49\" crossorigin=\"anonymous\"></script>");
  client.println("<script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/js/bootstrap.min.js\" integrity=\"sha384-ChfqqxuZUCnJSK3+MXmPNIyE6ZbWh2IMqE241rYiqJxyMiZ6OW/JmZQ5stwEULTy\" crossorigin=\"anonymous\"></script>");

  client.println("<script>");
  client.println("   $(document).ready(function(){ ");
  client.println("     $(\"#dWater\").load(\"/Water\");");
  //  $("#div1").load("demo_test.txt");
  client.println("     });");
  client.println("   setInterval(function() { ");
  client.println("     $(\"#dWater\").load(\"/Water\");");
  client.println("     $(\"#dTemp\" ).load(\"/Temp\");");
  client.println("   }, 5000);");

  client.println("</script>");
  client.println("</head>");
  client.println("<body>");
  {
    client.println("<div class=\"container\">");
    {
      client.println("<div class=\"jumbotron\">");
      client.println("<h1>CRW Water Monitoring</h1>");
      client.println("<div class=\"row\">");
      {
        client.println("<div class=\"col\" id=\"dTemp\"> Temp:  67 F</div>");
        client.print("<div class=\"col\" id=\"dWater\"></div>");
        client.println("<div class=\"col\">Lights: 54% </div>");
        client.println("</div>");
        client.println("</div>");
      }
      client.println("<br><br>");


      client.println("<div>Toggle the LED</div>");
      client.println("<a class=\"btn btn-success\" role=\"button\"  href=\"/Water\">Water</a>");
      client.println("<a class=\"btn btn-success\" role=\"button\"  href=\"/LED=ON\">On</a>");
      client.println("<a class=\"btn btn-dark\" role=\"button\" href=\"/LED=OFF\">Off</a>");

      client.println("</div>");
    }
    client.println("</body>");
  }
  client.println("</html>");


  delay(1000);
}
