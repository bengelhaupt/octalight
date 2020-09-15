/*
   OctaLight

   A octagon-shaped light made from LED strips.
   Features multiple lighting modes and effects.
   Based on the ESP8266 platform.

   A project by Ben-Noah Engelhaupt (code@bengelhaupt.com) Github: bengelhaupt
   Published under GNU General Public License v3.0

   http://bengelhaupt.com/projects/octalight
*/

#include "funcs.h"
#include "values.h"

#include <NeoPixelBus.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Schedule.h>
#include <GoogleAssistantSmarthome.h>

//// SETUP

// WiFi
#define WIFI_SSID "..."
#define WIFI_PASSWORD "..."

// LED strip, set pixel count in values.h
#define PIXEL_PIN 5
NeoPixelBus<NeoGrbwFeature, NeoEsp8266Dma800KbpsMethod> strip(PixelCount, PIXEL_PIN);

#define DEFAULT_WHITE RgbwColor(192, 128, 64, 255)

ESP8266WebServer server(80);
GoogleAssistantSmarthome smarthome(&server);

////

//// Smarthome

#define AUTH_ROUTE "/auth"
#define TOKEN_ROUTE "/token"
#define FULFILLMENT_ROUTE "/"
#define CLIENT_ID "..."
#define CLIENT_SECRET "..."
#define PROJECT_ID "..."
#define AUTH_TOKEN "..."
#define REFRESH_TOKEN "..."
#define ACCESS_TOKEN "..."

String getOnState() {
  RgbwColor current = getAverage(&strip);
  String isOn = "false";
  if (current.R + current.G + current.B + current.W > 0) {
    isOn = "true";
  }
  return isOn;
}

String getBrightnessState() {
  RgbwColor current = getAverage(&strip);
  float brightness;
  if (current.R + current.G + current.B + current.W > 0) {
    brightness = max(current.R, max(current.G, max(current.B, current.W))) / 255.0f;
  } else {
    brightness = 0.0f;
  }
  return String((int) (100 * brightness));
}

int getColorState() {
  RgbwColor current = getAverage(&strip);
  return 65536 * current.R + 256 * current.G + current.B;
}

String getState(String onState = getOnState(), String brightnessState = getBrightnessState(), int colorState = getColorState()) {
  return "\"on\":" + onState + ",\"brightness\":" + brightnessState + ",\"color\":{\"spectrumRrb\":" + colorState + "},\"online\":true";
}

////

//// Server

void sendPage(String message, bool page = true, RgbwColor avg = getAverage(&strip)) {
  bool requestedPage = true;
  if (server.arg("page") == "false")
    requestedPage = false;

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  if (requestedPage && page) {
    if (message != "")
      message += "<br><br>";
    server.send(200, "text/html", "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, maximum-scale=1\"> <script>var endpoint=window.location.origin;var staticWebsite=false;var req;function zeroFill(number,width){width-=number.toString().length;if(width>0){return new Array(width+(/\\./.test(number)?2:1)).join('0')+number;} return number+\"\";} var moving=false;var redC=255;var greenC=255;var blueC=255;var whiteC=255;var transition=100;function check(click=false){var r=parseInt(document.getElementById('redVal').value);var g=parseInt(document.getElementById('greenVal').value);var b=parseInt(document.getElementById('blueVal').value);var w=parseInt(document.getElementById('whiteVal').value);if((document.getElementById('instantChange').checked||click)&&(r!=redC||g!=greenC||b!=blueC||w!=whiteC)){if(!click) transition=100;else transition=document.getElementById('trans').value;oncolor(r,g,b,w,transition);redC=r;greenC=g;blueC=b;whiteC=w;} if(moving) setTimeout(check,120);} function request(url,page){if(!page){if(req){req.abort();}req=new XMLHttpRequest();req.open(\"GET\",endpoint+url+\"&page=\"+page);req.send();}else{window.location.href=url+\"&page=\"+page;}} function oncolor(r,g,b,w,t,page=false){var red=r.toString(16);var green=g.toString(16);var blue=b.toString(16);var white=w.toString(16);request(\"/color?transition=\"+t+\"&color=\"+zeroFill(red,2)+zeroFill(green,2)+zeroFill(blue,2)+zeroFill(white,2),page);} function set(r,g,b,w){document.getElementById('redVal').value=r;document.getElementById('greenVal').value=g;document.getElementById('blueVal').value=b;document.getElementById('whiteVal').value=w;} function start(){if(!moving){moving=true;check();}} function end(){started=false;moving=false;} function off(page=staticWebsite){request(\"/off?\",page);set(0,0,0,0);} function on(page=staticWebsite){request(\"/on?\",page);set(" + String(DEFAULT_WHITE.R) + "," + String(DEFAULT_WHITE.G) + "," + String(DEFAULT_WHITE.B) + "," + String(DEFAULT_WHITE.W) + ");} function alternate(page=staticWebsite){var colors=document.getElementById(\"altColors\").value;var speed=document.getElementById(\"altSpeed\").value;var inner=document.getElementById(\"altColI\").value;var red=inner.substring(1,3).toString(16);var green=inner.substring(3,5).toString(16);var blue=inner.substring(5,7).toString(16);var white=\"00\";request(\"/alternate?cycles=-1&colors=\"+colors+\"&duration=\"+speed+\"&inner=\"+zeroFill(red,2)+zeroFill(green,2)+zeroFill(blue,2)+zeroFill(white,2),page);} function altChange(){var col1=document.getElementById(\"altCol1\").value;var col2=document.getElementById(\"altCol2\").value;var red1=col1.substring(1,3).toString(16);var green1=col1.substring(3,5).toString(16);var blue1=col1.substring(5,7).toString(16);var white1=\"00\";var red2=col2.substring(1,3).toString(16);var green2=col2.substring(3,5).toString(16);var blue2=col2.substring(5,7).toString(16);var white2=\"00\";document.getElementById(\"altColors\").value=zeroFill(red1,2)+zeroFill(green1,2)+zeroFill(blue1,2)+zeroFill(white1,2)+zeroFill(red2,2)+zeroFill(green2,2)+zeroFill(blue2,2)+zeroFill(white2,2)} function spectrum(page=staticWebsite){var speed=document.getElementById(\"specSpeed\").value;request(\"/spectrum?cycles=-1&duration=\"+speed,page);} function stroboscope(page=staticWebsite){var colors=document.getElementById(\"strobColors\").value;var freq=document.getElementById(\"strobFreq\").value;var inter=document.getElementById(\"strobInt\").value;var mode=document.getElementById(\"strobMode\").value;request(\"/stroboscope?colors=\"+colors+\"&frequency=\"+freq+\"&blacks=\"+inter+\"&mode=\"+mode,page);} function strobChange(){var col1=document.getElementById(\"strobCol1\").value;var col2=document.getElementById(\"strobCol2\").value;var red1=col1.substring(1,3).toString(16);var green1=col1.substring(3,5).toString(16);var blue1=col1.substring(5,7).toString(16);var white1=\"00\";var red2=col2.substring(1,3).toString(16);var green2=col2.substring(3,5).toString(16);var blue2=col2.substring(5,7).toString(16);var white2=\"00\";document.getElementById(\"strobColors\").value=zeroFill(red1,2)+zeroFill(green1,2)+zeroFill(blue1,2)+zeroFill(white1,2)+zeroFill(red2,2)+zeroFill(green2,2)+zeroFill(blue2,2)+zeroFill(white2,2)} function ripple(page=staticWebsite){var colors=document.getElementById(\"ripColors\").value;var speed=document.getElementById(\"ripSpeed\").value;request(\"/ripple?colors=\"+colors+\"&duration=\"+speed,page);} function ripChange(){var col1=document.getElementById(\"ripCol1\").value;var col2=document.getElementById(\"ripCol2\").value;var red1=col1.substring(1,3).toString(16);var green1=col1.substring(3,5).toString(16);var blue1=col1.substring(5,7).toString(16);var white1=\"00\";var red2=col2.substring(1,3).toString(16);var green2=col2.substring(3,5).toString(16);var blue2=col2.substring(5,7).toString(16);var white2=\"00\";document.getElementById(\"ripColors\").value=zeroFill(red1,2)+zeroFill(green1,2)+zeroFill(blue1,2)+zeroFill(white1,2)+zeroFill(red2,2)+zeroFill(green2,2)+zeroFill(blue2,2)+zeroFill(white2,2)} function starlight(page=staticWebsite){var color=document.getElementById(\"starCol\").value;var frequency=document.getElementById(\"starFreq\").value;var max=document.getElementById(\"starMax\").value;request(\"/starlight?color=\"+color+\"&frequency=\"+frequency+\"&max=\"+max,page);} function random(page=staticWebsite){var frequency=document.getElementById(\"randFreq\").value;request(\"/randomness?frequency=\"+frequency,page);}</script> <style>html{font-family:Arial}.card{background-color:#FFF;border-radius:0.4em;border:1px solid #CCC;padding:1em;font-size:1em}.button{color:#444;background-color:rgba(0,0,0,0);border:none;cursor:pointer;font-weight:bold;font-size:1.1em}.slider{-webkit-appearance:none;appearance:none;width:100%;height:1em;margin:0.5em 0;border:1px solid #444;outline:none}.slider::-webkit-slider-thumb{-webkit-appearance:none;appearance:none;width:2em;height:2em;background:rgba(20,20,20,0.6);cursor:pointer}.slider::-moz-range-thumb{width:2em;height:2em;background:rgba(20,20,20, 0.6);cursor:pointer}</style></head><body style=\"background-color: #EEE; width: 100%; padding: 0; margin: 0;\" onload=\"set(" + String(avg.R) + "," + String(avg.G) + "," + String(avg.B) + "," + String(avg.W) + ");\"><div style=\"padding: 1em;\"><div style=\"float: right;\" class=\"card\"> <button class=\"button\" onclick=\"on()\" style=\"margin-right: 1em;\">ON</button> <button class=\"button\" onclick=\"off()\">OFF</button></div> <font style=\"font-size: 2em; line-height: 1.5em;\">OctaLight</font> <br> <br><div class=\"card\"> <input class=\"slider\" style=\"background: #FF6666;\" type=\"range\" min=\"0\" max=\"255\" value=\"255\" id=\"redVal\" ontouchstart=\"start()\" ontouchend=\"end()\" onmousedown=\"start()\" onmouseup=\"end()\"/> <br> <input class=\"slider\" style=\"background: #66FF66;\" type=\"range\" min=\"0\" max=\"255\" value=\"255\" id=\"greenVal\" ontouchstart=\"start()\" ontouchend=\"end()\" onmousedown=\"start()\" onmouseup=\"end()\"/> <br> <input class=\"slider\" style=\"background: #6666FF;\" type=\"range\" min=\"0\" max=\"255\" value=\"255\" id=\"blueVal\" ontouchstart=\"start()\" ontouchend=\"end()\" onmousedown=\"start()\" onmouseup=\"end()\"/> <br> <input class=\"slider\" style=\"background: #FFFFFF;\" type=\"range\" min=\"0\" max=\"255\" value=\"255\" id=\"whiteVal\" ontouchstart=\"start()\" ontouchend=\"end()\" onmousedown=\"start()\" onmouseup=\"end()\"/> <br> <br> <input type=\"checkbox\" id=\"instantChange\" checked/> &nbsp;<label for=\"instantChange\" style=\"margin-right: 1.5em;\">Instantly</label> <label for=\"trans\">Transition</label> &nbsp;<input type=\"number\" value=\"100\" id=\"trans\" style=\"width: 5em;\"/> <br> <input class=\"button\" type=\"button\" onclick=\"new function(){check(true);}\" value=\"CHANGE COLOR\" style=\"float: right;\"/> <br></div> <br><div class=\"card\"> <font style=\"font-size: 1.3em;\">Starlight</font> <br> <br> <label for=\"starCol\">Color</label> &nbsp;<input type=\"text\" value=\"00000010\" id=\"starCol\" style=\"margin-right: 1.5em; width: 7em;\"/> <label for=\"starFreq\">Frequency (per min.)</label> &nbsp;<input type=\"number\" id=\"starFreq\" value=\"20\" style=\"margin-right: 1.5em; width: 5em;\"/> <label for=\"starMax\">Maximum pixels</label> &nbsp;<input type=\"number\" id=\"starMax\" value=\"20\" style=\"width: 3em;\"/> <br> <input class=\"button\" type=\"button\" style=\"float: right;\" onclick=\"starlight()\" value=\"START\"/> <br></div> <br><div class=\"card\"> <font style=\"font-size: 1.3em;\">Spectrum</font> <br> <br> <label for=\"specSpeed\">Cycle duration (ms)</label> &nbsp;<input type=\"number\" id=\"specSpeed\" step=\"100\" value=\"10000\" style=\"width: 6em;\"/> <br> <input class=\"button\" type=\"button\" style=\"float: right;\" onclick=\"spectrum()\" value=\"START\"/> <br></div> <br><div class=\"card\"> <font style=\"font-size: 1.3em;\">Alternate</font> <br> <br> <label for=\"altCol1\">Color 1</label> &nbsp;<input type=\"color\" value=\"#FF0000\" id=\"altCol1\" style=\"margin-right: 1.5em;\" onchange=\"altChange()\"/> <label for=\"altCol2\">Color 2</label> &nbsp;<input type=\"color\" value=\"#FFFFFF\" id=\"altCol2\" style=\"margin-right: 1.5em;\" onchange=\"altChange()\"/> <label for=\"altColI\">Inner Color</label> &nbsp;<input type=\"color\" value=\"#000000\" id=\"altColI\" style=\"margin-right: 1.5em;\"/> <label for=\"altSpeed\">Cycle duration (ms)</label> &nbsp;<input type=\"number\" id=\"altSpeed\" value=\"1000\" style=\"width: 5em;\"/> <br> <br> <label for=\"altColors\">Manual Hex Colors</label> &nbsp;<input type=\"text\" id=\"altColors\" value=\"FF000000FFFFFF00\" style=\"width: 20em;\"/> <br> <input class=\"button\" type=\"button\" style=\"float: right;\" onclick=\"alternate()\" value=\"START\"/> <br></div> <br><div class=\"card\"> <font style=\"font-size: 1.3em;\">Stroboscope</font> <br> <br> <label for=\"strobCol1\">Color 1</label> &nbsp;<input type=\"color\" value=\"#FFFFFF\" id=\"strobCol1\" style=\"margin-right: 1.5em;\" onchange=\"strobChange()\"/> <label for=\"strobCol2\">Color 2</label> &nbsp;<input type=\"color\" value=\"#000000\" id=\"strobCol2\" style=\"margin-right: 1.5em;\" onchange=\"strobChange()\"/> <label for=\"strobFreq\">Frequency (per s.)</label> &nbsp;<input type=\"number\" id=\"strobFreq\" step=\"1\" value=\"20\" style=\"width: 5em; margin-right: 1.5em;\"/> <label for=\"strobInt\">Black phases</label> &nbsp;<input type=\"number\" id=\"strobInt\" value=\"1\" style=\"width: 5em; margin-right: 1.5em;\"/> <label for=\"strobMode\">Mode</label> &nbsp; <select id=\"strobMode\"><option value=\"order\">Order</option><option value=\"random\">Random</option><option value=\"random_spectrum\">Random in spectrum</option> </select> <br> <br> <label for=\"strobColors\">Manual Hex Colors</label> &nbsp;<input type=\"text\" id=\"strobColors\" value=\"FFFFFFFF00000000\" style=\"width: 20em;\"/> <br> <input class=\"button\" type=\"button\" style=\"float: right;\" onclick=\"stroboscope()\" value=\"START\"/> <br></div> <br><div class=\"card\"> <font style=\"font-size: 1.3em;\">Ripple</font> <br> <br> <label for=\"ripCol1\">Color 1</label> &nbsp;<input type=\"color\" value=\"#FFFFFF\" id=\"ripCol1\" style=\"margin-right: 1.5em;\" onchange=\"ripChange()\"/> <label for=\"ripCol2\">Color 2</label> &nbsp;<input type=\"color\" value=\"#000000\" id=\"ripCol2\" style=\"margin-right: 1.5em;\" onchange=\"ripChange()\"/> <label for=\"ripSpeed\">Cycle duration (ms)</label> &nbsp;<input type=\"number\" id=\"ripSpeed\" value=\"1000\" style=\"width: 5em; margin-right: 1.5em;\"/> <br> <br> <label for=\"ripColors\">Manual Hex Colors</label> &nbsp;<input type=\"text\" id=\"ripColors\" value=\"FFFFFFFF00000000\" style=\"width: 20em;\"/> <br> <input class=\"button\" type=\"button\" style=\"float: right;\" onclick=\"ripple()\" value=\"START\"/> <br></div> <br><div class=\"card\"> <font style=\"font-size: 1.3em;\">Random</font> <br> <br> <label for=\"randFreq\">Frequency (per s.)</label> &nbsp;<input type=\"number\" id=\"randFreq\" value=\"50\" style=\"width: 5em; margin-right: 1.5em;\"/> <br> <input class=\"button\" type=\"button\" style=\"float: right;\" onclick=\"random()\" value=\"START\"/> <br></div> <br> <br> <br> OctaLight by Ben-Noah Engelhaupt. Visit <a href=\"http://bengelhaupt.com/projects/octalight\">bengelhaupt.com</a> for more information.</div></body></html>");
  } else {
    server.send(200, "text/html", message);
  }
}

void handleNotFound() {
  if (server.method() == HTTP_OPTIONS) {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200);
  } else {
    sendPage("Command not found");
  }
}

boolean handlingClient = false;
bool interruptCheckingRoutine() {
  handlingClient = false;
  yield();
  server.handleClient();
  return handlingClient;
}

long lastWiFiCheck = millis();
void setup() {
  Serial.begin(115200);

  strip.Begin();
  setPixelsColor(&strip, interruptCheckingRoutine, 0, PixelCount - 1, DEFAULT_WHITE, 200);

  WiFi.mode(WIFI_STA);
  WiFi.hostname("OctaLight");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (true) {
    if (millis() - lastWiFiCheck > 600) {
      if (WiFi.status() == WL_CONNECTED)
        break;
      Serial.print(".");
      lastWiFiCheck = millis();
    }
    yield();
  }

  Serial.print("WiFi connected. Local IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, []() {
    sendPage("");
  });

  server.on("/get", HTTP_GET, []() {
    RgbwColor c = getAverage(&strip);
    char buf[8];
    sprintf(buf, "%02x%02x%02x%02x", c.R, c.G, c.B, c.W);
    sendPage(buf, false);
  });

  server.on("/on", HTTP_GET, []() {
    sendPage("turning on", true, DEFAULT_WHITE);

    handlingClient = true;
    schedule_function([]() {
      setPixelsColor(&strip, interruptCheckingRoutine, 0, PixelCount - 1, DEFAULT_WHITE, 100);
    });
  });

  server.on("/off", HTTP_GET, []() {
    sendPage("turning off", true, RgbwColor(0, 0, 0, 0));

    handlingClient = true;
    schedule_function([]() {
      setPixelsColor(&strip, interruptCheckingRoutine, 0, PixelCount - 1, RgbwColor(0, 0, 0, 0), 100);
    });
  });

  server.on("/color", HTTP_GET, []() {
    String message = "";
    if (server.arg("color") == "") {
      sendPage("no color (argument 'color', 'rrggbbww' (hex)) given or empty");
    } else {
      message += "showing color";

      RgbwColor c = getColor(server.arg("color"));

      sendPage(message, true, c);

      int transition = 0;
      if (server.arg("transition") != "")
        transition = atoi(server.arg("transition").c_str());

      String mode = "all";  //all|outer|inner
      if (server.arg("mode") != "")
        mode = server.arg("mode");

      handlingClient = true;
      if (mode == "outer") {
        schedule_function([c, transition]() {
          setPixelsColor(&strip, interruptCheckingRoutine, 0, OuterPixelCount - 1, c, transition);
        });
      } else if (mode == "inner") {
        schedule_function([c, transition]() {
          setPixelsColor(&strip, interruptCheckingRoutine, OuterPixelCount, PixelCount - 1, c, transition);
        });
      } else {
        schedule_function([c, transition]() {
          setPixelsColor(&strip, interruptCheckingRoutine, 0, PixelCount - 1, c, transition);
        });
      }
    }
  });

  server.on("/alternate", HTTP_GET, []() {
    String message = "";
    String colors = "";
    if (server.arg("colors") == "")
      sendPage("no colors (argument 'colors', '(rrggbbww)+' (hex)) given or empty");
    else
      colors = server.arg("colors");

    if (colors != "") {
      message += "showing alternating circle";
      sendPage(message);

      RgbwColors c = getColors(colors);

      int duration = 1000;
      if (server.arg("duration") != "")
        duration = atoi(server.arg("duration").c_str());

      String innerColor = "00000000";
      if (server.arg("inner") != "")
        innerColor = server.arg("inner");
      RgbwColor inner = getColor(innerColor);

      int cycles = -1;
      if (server.arg("cycles") != "")
        cycles = atoi(server.arg("cycles").c_str());

      handlingClient = true;
      schedule_function([c, inner, duration, cycles]() {
        alternatingCircle(&strip, interruptCheckingRoutine, c, inner, duration, cycles);
      });
    }
    else {
      sendPage(message);
    }
  });

  server.on("/ripple", []() {
    String message = "";
    String colors = "";
    if (server.arg("colors") == "")
      sendPage("no colors (argument 'colors', '(rrggbbww)+' (hex)) given or empty");
    else
      colors = server.arg("colors");

    if (colors != "") {
      message += "showing ripple";
      sendPage(message);

      RgbwColors c = getColors(colors);

      int duration = 1000;
      if (server.arg("duration") != "")
        duration = atoi(server.arg("duration").c_str());

      int cycles = -1;
      if (server.arg("cycles") != "")
        cycles = atoi(server.arg("cycles").c_str());

      handlingClient = true;
      schedule_function([c, duration, cycles]() {
        ripple(&strip, interruptCheckingRoutine, c, duration, cycles);
      });
    }
    else {
      sendPage(message);
    }
  });

  server.on("/spectrum", []() {
    String message = "";
    message += "showing spectrum";
    sendPage(message);

    int duration = 10000;
    if (server.arg("duration") != "")
      duration = atoi(server.arg("duration").c_str());

    int cycles = -1;
    if (server.arg("cycles") != "")
      cycles = atoi(server.arg("cycles").c_str());

    handlingClient = true;
    schedule_function([duration, cycles]() {
      spectrum(&strip, interruptCheckingRoutine, duration, cycles);
    });
  });

  server.on("/randomness", []() {
    String message = "";
    message += "showing randomness";
    sendPage(message);

    int frequency = 50;
    if (server.arg("frequency") != "")
      frequency = atoi(server.arg("frequency").c_str());

    handlingClient = true;
    schedule_function([frequency]() {
      randomness(&strip, interruptCheckingRoutine, frequency);
    });
  });

  server.on("/starlight", []() {
    String message = "";
    message += "showing starlight";
    sendPage(message);

    String starColor = "00000010";
    if (server.arg("color") != "")
      starColor = server.arg("color");
    RgbwColor star = getColor(starColor);

    int frequency = 20;
    if (server.arg("frequency") != "")
      frequency = atoi(server.arg("frequency").c_str());

    int max = PixelCount * 0.2;
    if (server.arg("max") != "")
      max = atoi(server.arg("max").c_str());

    handlingClient = true;
    schedule_function([star, frequency, max]() {
      starlight(&strip, interruptCheckingRoutine, star, frequency, max);
    });
  });

  server.on("/stroboscope", []() {
    String message = "";
    String colors = "";
    if (server.arg("colors") == "")
      sendPage("no colors (argument 'colors', '(rrggbbww)+' (hex)) given or empty\r\n");
    else
      colors = server.arg("colors");

    if (colors != "") {
      message += "showing stroboscope";
      sendPage(message);

      RgbwColors c = getColors(colors);

      float frequency = 20;
      if (server.arg("frequency") != "")
        frequency = atof(server.arg("frequency").c_str());

      int blacks = 1;
      if (server.arg("blacks") != "")
        blacks = atoi(server.arg("blacks").c_str());

      handlingClient = true;
      String mode = "order";  //order|random|random_spectrum
      if (server.arg("mode") != "")
        mode = server.arg("mode");
      if (mode == "random_spectrum")
        schedule_function([c, blacks, frequency]() {
        stroboscope(&strip, interruptCheckingRoutine, 0, PixelCount - 1, c, RANDOM_SPECTRUM, blacks, frequency);
      });
      else if (mode == "random")
        schedule_function([c, blacks, frequency]() {
        stroboscope(&strip, interruptCheckingRoutine, 0, PixelCount - 1, c, RANDOM, blacks, frequency);
      });
      else
        schedule_function([c, blacks, frequency]() {
        stroboscope(&strip, interruptCheckingRoutine, 0, PixelCount - 1, c, ORDER, blacks, frequency);
      });
    }
    else {
      sendPage(message);
    }
  });

  smarthome.setup(
    AUTH_ROUTE,
    TOKEN_ROUTE,
    FULFILLMENT_ROUTE,
    CLIENT_ID,
    CLIENT_SECRET,
    PROJECT_ID,
    AUTH_TOKEN,
    REFRESH_TOKEN,
    ACCESS_TOKEN,
  []() {
    return String("\"agentUserId\":\"octalight\",\"devices\":[{\"id\":\"octalight\",\"type\":\"action.devices.types.LIGHT\",\"traits\":[\"action.devices.traits.OnOff\",\"action.devices.traits.Brightness\",\"action.devices.traits.ColorSetting\",\"action.devices.traits.LightEffects\"],\"attributes\":{\"colorModel\":\"rgb\",\"supportedEffects\":[\"colorLoop\",\"sleep\",\"wake\"]},\"name\":{\"name\":\"OctaLight\"},\"willReportState\":false}]");
  },
  [](String id) {
    if (id == "octalight") {
      return getState();
    }
    return String();
  },
  [](String id, String command, JsonVariant params) {
    if (id == "octalight") {
      String setOn = getOnState();
      String setBrightness = getBrightnessState();
      int setColor = getColorState();

      if (command == "action.devices.commands.OnOff") {
        bool on = params["on"];

        handlingClient = true;
        if (on) {
          schedule_function([]() {
            setPixelsColor(&strip, interruptCheckingRoutine, 0, PixelCount - 1, DEFAULT_WHITE, 100);
          });
          setOn = "true";
        } else {
          schedule_function([]() {
            setPixelsColor(&strip, interruptCheckingRoutine, 0, PixelCount - 1, RgbwColor(0, 0, 0, 0), 100);
          });
          setOn = "false";
        }
      }

      if (command == "action.devices.commands.BrightnessAbsolute") {
        uint8_t brightness = params["brightness"];

        RgbwColor current = getAverage(&strip);
        float currentBrightness = max(current.R, max(current.G, max(current.B, current.W))) / 255.0f;
        float factor;
        if (currentBrightness > 0) {
          factor = brightness / currentBrightness / 100.0f;
        } else {
          factor = brightness / 100.0f;
          current = DEFAULT_WHITE;
        }
        RgbwColor newColor = RgbwColor(ceil(factor * current.R), ceil(factor * current.G), ceil(factor * current.B), ceil(factor * current.W));

        handlingClient = true;
        schedule_function([newColor]() {
          setPixelsColor(&strip, interruptCheckingRoutine, 0, PixelCount - 1, newColor, 100);
        });
        setBrightness = String(brightness);
      }

      if (command == "action.devices.commands.ColorAbsolute") {
        int color = params["color"]["spectrumRGB"];

        RgbwColor newColor = RgbwColor(color / 65536 % 256, color / 256 % 256, color % 256);

        handlingClient = true;
        schedule_function([newColor]() {
          setPixelsColor(&strip, interruptCheckingRoutine, 0, PixelCount - 1, newColor, 100);
        });
        setColor = color;
      }

      if (command == "action.devices.commands.ColorLoop") {
        int duration = params["duration"];
        int cycles = duration / 5;

        handlingClient = true;
        schedule_function([cycles]() {
          spectrum(&strip, interruptCheckingRoutine, 5000, cycles);
        });
      }

      if (command == "action.devices.commands.Sleep") {
        //int duration = params["duration"];

        handlingClient = true;
        schedule_function([]() {
          starlight(&strip, interruptCheckingRoutine, RgbwColor(0, 0, 0, 3), 20, 6);
        });
      }

      if (command == "action.devices.commands.Wake") {
        int duration = params["duration"];

        handlingClient = true;
        schedule_function([duration]() {
          setPixelsColor(&strip, interruptCheckingRoutine, 0, PixelCount - 1, DEFAULT_WHITE, 1000 * duration);
        });
      }

      if (command == "action.devices.commands.StopEffect") {
        handlingClient = true;
        schedule_function([]() {
          setPixelsColor(&strip, interruptCheckingRoutine, 0, PixelCount - 1, getAverage(&strip), 0);
        });
      }

      return getState(setOn, setBrightness, setColor);
    }

    return String();
  },
  []() {  }
  );

  server.onNotFound(handleNotFound);
  server.begin();
}

////

void loop() {
  if (!handlingClient) {
    server.handleClient();
  }
}
