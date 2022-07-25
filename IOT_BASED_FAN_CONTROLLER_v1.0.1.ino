


#include <ESP8266WiFi.h>
#include<ESP8266WebServer.h>
#include <LiquidCrystal_I2C.h>

// setting our wifi name and password:
#define ssid "IOT FAN"
#define passcode "123456789"

// setting our ip adress for local,subnet and geteway:
IPAddress local_ip(192, 168, 10, 10);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// instantiating an esp8266webserver object with the value of 80:
ESP8266WebServer server(80);

/*
    declaring pin-out on nodemcu for the ultrasonic sensor and the pump and the pump_pin status:
*/
const int d0 = 16;
const int d1 = 5;
const int d2 = 4;
const int d3 = 0;
const int d4 = 2;
const int d5 = 14;
const int d6 = 12;
const int d7 = 13;
const int d8 = 15;

// initializing the fan pin, button pin, fan start and stop button,
// fan output pin, mains reader/checker pin,
// lcd button and the mains pin
int fanPin = d8; // this is the pin out for controling the relay of the fan.
int mainsPin = d5; // this is the pin used to check if the mains is available.
int lcdBtn = d7; // this is the pin connenected to the button to go to the next screen.
int fanBtn = d6; // this is the pin connected to the butoon to turn on and off the fan onsite.

// initializing the lcd screen number(lcdNumber) and its alt variable
// these variables are used to change the content
// on the lcd to the next screen depending on the lcdNumbner.
int lcdNumber = 1;
int altLcdNumber = 0;

// initailizing string variables with initail values, which will
// be sent to be sent to the browser upon initial request
String mainsState = "OFF";
String fanState = "OFF";
String mainsLightColor = "#767676";
String fanLightColor =  "#767676";
String message = "no message yet";
String Info = "Click Here To Turn Fan On";
String btnValue = "ON";
String btnColor = "#36c94c";
String data = "";
String green = "#36c94c";
String gray = "#767676";

// creating all global boolean variable
bool lcdBtnState = false;
bool fanBtnState = false;

// creating all global alt boolean variable are use to make the code run efficiently
// as it does not run the block of code if no diffirence between the variable and its alt variable
// example is <<<<<< myVar ,altMyVar if(myVar != altMyVar){execute me} >>>>>>
bool altLcdBtnState = false;
bool altFanBtnState = false;
bool isFan = false;
bool isMains = false;

// creating all global alt string variable.
String altMainsState = "";
String altFanState = "";

//creating an lcd object.
LiquidCrystal_I2C lcd(0x27, 16, 2);


// this is the data tto be sent upon the "/" request i.e the initial request.
const String sendText = "<!DOCTYPE html>\n <html lang=\"en\"> \n   <head> <meta charset=\"UTF-8\"> \n  <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"> \n  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> \n <title>FAN CONTROLLER</title>  \n <style> \n *{\n  margin: 0;\n  padding:0;\n   \n}  \n body{\n font-family: 'Gill Sans', 'Gill Sans MT', Calibri, 'Trebuchet MS', sans-serif; \n  background:rgba(22, 61, 90, 0.8); \n   } \n   header{\n    margin: 30px auto; \n   background: rgba(0,0,0,0.6); \n   border-bottom: rgba(255,0,0,0.6) 4px solid; \n   box-sizing: border-box; \n   width: 100%;  \n  min-height: 100px; \n   overflow: hidden;  \n  } \n   #header{ \n   text-transform: capitalize; \n   color: #fff;  \n  text-align: center;  \n  padding: 40px 20px;  \n  letter-spacing: 2px; \n   word-spacing: 5px; \n   } \n   .container{ \n       width:90%;  \n  padding: 10px; \n  margin:auto; \n   box-sizing: border-box; \n   overflow: hidden; \n   }  \n  .prop{ \n   color: #fff; \n   display: inline;   \n margin-left: 30px; \n   } \n   .value{ \n   color:#fff; \n   font-size: 20px; \n   margin-right: 2px; \n   } \n   .stats{ \n   position: relative; \n   text-align: center; \n   background: rgba(22, 61, 90, 0.4); \n       } \n   #mains-light{ \n       height:50px; \n   display: inline;  \n  border:solid 1px #fff;  \n  width: 100px; \n   border-radius: 50%; \n   background: #767676; \n   }  \n      #fan-light{ \n   height:50px; \n   display: inline; \n   border:solid 1px #fff; \n   width: 100px;  \n  border-radius: 50%;  \n  background: #767676;  \n }  \n  #status{ \n   position: absolute; \n   top: 2px;  \n  color: #fff; \n   font-weight: 1000px;  \n  font-size: 20px; \n   }  \n  label{ \n   color: #fff; \n   padding-top: 100px; \n   font-size: 20px; \n   } \n    .msg{ \n   color: #fff; \n   padding: 10px 10px; \n   }  \n  .control{ \n   text-align:center ; \n   background: rgba(22, 61, 90, 0.4); \n   }  \n  #btn{ \n outline:none; \n display: block; \n   margin: 20px auto; \n   padding:10px; \n   background-color: #36c94c; \n   color: #fff; \n   border: 2.5px solid ;  \n  border-radius: 15px;  \n  transition: all .2s; \n   transition-timing-function:ease-in-out; \n   } \n   .control div>input:active{ \n   transform: scale(0.8); \n   } \n   #msg{ \n   padding: 10px 0px 20px 0px; \n   font-size: 18px; \n   }  \n  </style> \n   </head> \n <body> \n   <header>   \n       <h1 id=\"header\"> \n iot based fan controller \n </h1> \n  </header> \n    <main>  \n  <!-- this is another section -->    \n <section class=\"container stats\"> \n   <h2 id=\"status\"> Status</h2> \n   <div style=\"padding-top:50px\" >  \n  <h3 class=\"prop\">Mains: </h3>  \n  <span class=\"value\" id=\"mains-state\">\n Off \n </span> \n       <div id=\"mains-light\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</div>   \n <h3 class=\"prop\">Fan:</h3>   \n  <span class=\"value\" id=\"fan-state\"> \n Off &nbsp; \n </span>   \n <div id=\"fan-light\"> \n &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; \n </div>   \n </div>  \n  </section>  \n  <!-- this is another section -->  \n  <section class=\"container msg\"> \n   <h2>Message:</h2>   \n <p id=\"msg\">no message yet</p> \n   </section>  \n  <!-- this is anoither section -->  \n  <section class=\"container control\"> \n   <div>  \n  <label id=\"info\"> \n Click Here To Turn Fan On \n </label>    \n    <input id=\"btn\" type=\"button\" value=\"ON\">      \n  </div>    </section>  \n <script>   \n let data = {   \n mainsState :\"Off\", \n   fanState:\"Off\",  \n  mainsLightColor:\"#767676\",  \n  fanLightColor:\"#767676\",  \n  message:\"Fan Currently Off\",\n    Info:\"Click Here To Turn Fan ON\",  \n  btnValue:\"ON\",  \n  btnColor:\"#36c94c\"  \n      }; \n const btn = document.querySelector(\"#btn\"); \n const msg = document.querySelector(\"#msg\"); \n const fanLight = document.querySelector(\"#fan-light\"); \n const mainsLight = document.querySelector(\"#mains-light\"); \n const mainsState = document.querySelector(\"#mains-state\"); \n const info = document.querySelector(\"#info\"); \n const fanState = document.querySelector(\"#fan-state\"); \n  btn.addEventListener(\"click\",updateServer); \n function updateServer(){\n        if(btn.value == \"ON\"){  \n      let onReq = new XMLHttpRequest();    \n    onReq.open(\"get\",\"/onfan\",true); \n       onReq.onload = ()=>{     \n            let response = this.responseText;   \n          response = JSON.parse(response);     \n    data = { \n       ...data,    ...response    };  \n       console.log(data);  \n  };  \n  onReq.send(); \n onReq.onerror = ()=>{\n console.log(\"onreq error\");  \n}   }\n    else{   \n let offReq = new    XMLHttpRequest();      \n      offReq.open(\"get\",\"/offfan\",true); \n     offReq.send(); \n offReq.onerror = ()=>{ \n console.log(\"offReq error\"); \n  } \n }  \n  }  \n const dataUpdate = ()=>{  \n      let getData = new XMLHttpRequest();    \n    getData.open(\"get\",\"/getdata\",true);   \n     getData.onload = ()=>{     \n    let response = getData.responseText;  \n  response = JSON.parse(response); \n  data = {    ...data,    ...response    };  \n  }; \n   getData.send();  \n  mainsState.textContent = data.mainsState;  \n  mainsLight.style.background = data.mainsLightColor;  \n  fanState.textContent = data.fanState;  \n  fanLight.style.background = data.fanLightColor;  \n  msg.textContent = data.message;  \n  info.textContent = data.Info; \n   btn.value = data.btnValue;  \n  btn.style.background = data.btnColor;  \n  };  \n  setInterval(()=>{   \n dataUpdate();\n   \n  },500); \n     </script>  \n  </body>  \n  </html>";

void setup() { // put your code here to run once:
  // setting up the pin modes
  pinMode(fanPin, OUTPUT);
  pinMode(fanBtn, INPUT);
  pinMode(lcdBtn, INPUT);
  pinMode(mainsPin, INPUT);

  // initializing the lcd object.
  lcd.init();
  // turning on the backlight
  lcd.backlight();
  lcd.print(" SMART IOT FAN");
  lcd.setCursor(0, 1);
  lcd.print(" CONTROL SYSTEM");
  delay(3000);
  lcd.clear();
  lcd.print("   STARTING...");
  delay(1500);

  Serial.begin(115200);
  WiFi.softAP(ssid, passcode);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  // setting the route and function to be
  // called to run upon  the /" request.
  server.on("/", handle_onconnect);

  // setting the route and function to be
  // called to run upon the "/onfan" request.
  server.on("/onfan", OnFan);

  // setting the route and function to be
  // called to run upon the "/offfan" request.
  server.on("/offfan", OffFan);

  // setting the route and function to be
  // called to run upon the "/getdata" request.
  server.on("/getdata", SendData);

  // setting the route and function to be
  // called to run upon the "/handle_notfound" request.
  server.onNotFound(handle_notfound);

  // stating the server.
  server.begin();

  Serial.println("HTTP WEB SERVER HAS STARTED");
  Serial.print("SSID:\t");
  Serial.println(ssid);
  Serial.print("PASSCODE:\t");
  Serial.println(passcode);

}


// this is the function the will be called upon the "/" request.
void handle_onconnect() {
  server.send(200, "text/html", sendText);
  Serial.println("home page");
}

// this is the function the will be called when an undefined request is made is made.
void handle_notfound() {
  Serial.println("invalid request: ");
  server.send(404, "text/html", "<h1>web page not available</h1>");
  Serial.println("not found");
}

// this is the function the will be called upon the "/onfan" request.
void OnFan() {
  isFan = true;
  btnColor = gray;
  Info = "Click Here To Turn Fan Off";
  btnValue = "OFF";
  Serial.println("fan on");
}

// this is the function the will be called upon the "/offfan" request.
void OffFan() {
  isFan = false;
  btnColor = green;
  Info = "Click Here To Turn Fan On";
  btnValue = "ON";
  Serial.println("fan off");
}

// this is the function the will be called upon the "/getdata" request.
void SendData() {

  //  mainsState
  data = "{";
  data += "\"mainsState\":";
  data += "\"";
  data += mainsState;
  data += "\",";
  //  fanState
  data += "\"fanState\":";
  data += "\"";
  data += fanState;
  data += "\",";
  //  mainsLightColor
  data += "\"mainsLightColor\":";
  data += "\"";
  data += mainsLightColor;
  data += "\",";

  //  fanLightColor
  data += "\"fanLightColor\":";
  data += "\"";
  data += fanLightColor;
  data += "\",";

  //  message = "no message yet";
  data += "\"message\":";
  data += "\"";
  data += message;
  data += "\",";

  //  Info = "Click Here To Turn Fan On";
  data += "\"Info\":";
  data += "\"";
  data += Info;
  data += "\",";

  //  btnValue = "ON";
  data += "\"btnValue\":";
  data += "\"";
  data += btnValue;
  data += "\",";

  //  btnColor = "#36c94c"
  data += "\"btnColor\":";
  data += "\"";
  data += btnColor;
  data += "\" }";

  server.send(200, "application/json", data);
  Serial.println("sent data");
}




void loop() { // put your code here to run repeatedly:
  server.handleClient();
  MainsChecker();
  FanControl();
  SetMessage();
  LcdBtn();
  FanBtn();
  LcdDisplay();
}



// helper functions.
void MainsChecker() {
  if (digitalRead(mainsPin) == LOW) {
    mainsState = "ON";
    isMains = true;
    mainsLightColor = green;

    //    mainsState = "OFF";
    //    isMains = false;
    //    mainsLightColor = gray;
  }
  else {
    mainsState = "OFF";
    isMains = false;
    mainsLightColor = gray;
    //    mainsState = "ON";
    //    isMains = true;
    //    mainsLightColor = green;
  }

  Serial.print(mainsState);
  Serial.println(mainsLightColor);
}

void FanControl() {
  if (isMains && isFan) {
    digitalWrite(fanPin, true);
    fanLightColor = green;
    fanState = "ON";
    btnColor = gray;
    Info = "Click Here To Turn Fan Off";
    btnValue = "OFF";
  }
  else {
    digitalWrite(fanPin, false);
    fanLightColor = gray;
    fanState = "OFF";
    btnColor = green;
    Info = "Click Here To Turn Fan On";
    btnValue = "ON";
  }
}

void SetMessage() {
  if (isFan == true && isMains == false)message = "mains currently off fan will turn on automatically when mains is available";

  else if (!isFan)message = "fan currently off";

  else if (isFan && isMains)message = "fan currently on";
}

void LcdBtn() {
  lcdBtnState = digitalRead(lcdBtn);
  if (lcdBtnState != altLcdBtnState) {
    altLcdBtnState = lcdBtnState;
    delay(50);
    if (!digitalRead(lcdBtn) && lcdNumber < 3) {
      lcdNumber++;
    }
    else if (!digitalRead(lcdBtn) && lcdNumber >= 3) {
      lcdNumber = 1;
    }
  }
}

void FanBtn() {
  fanBtnState = digitalRead(fanBtn);
  if (fanBtnState != altFanBtnState) {
    fanBtnState = altFanBtnState;
    delay(50);
    if (!digitalRead(fanBtn)) {
      isFan = !isFan;
    }
  }
}

void LcdDisplay() {

  if (lcdNumber == 1 && (altLcdNumber != lcdNumber)) {
    String ipAddress = "192.168.10.10";
    lcd.clear();
    lcd.print("IP ADDRESS: ");
    lcd.setCursor(0, 1);
    lcd.print(ipAddress);
    altLcdNumber = lcdNumber;
  }

  else if (lcdNumber == 2 && (altLcdNumber != lcdNumber)) {
    lcd.clear();
    lcd.print("Name:");
    lcd.print(ssid);
    lcd.setCursor(0, 1);
    lcd.print("Code: ");
    lcd.print(passcode);
    altLcdNumber = lcdNumber;
  }

  else if (lcdNumber == 3 && (altLcdNumber != lcdNumber || altMainsState != mainsState || altFanState != fanState)) {
    lcd.clear();
    lcd.print("Mains: ");
    lcd.print(mainsState);
    lcd.setCursor(0, 1);
    lcd.print("Fan: ");
    lcd.print(fanState);
    altLcdNumber = lcdNumber;
    altMainsState = mainsState;
    altFanState = fanState;
  }
}
