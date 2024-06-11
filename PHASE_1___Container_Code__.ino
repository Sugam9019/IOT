
#include <Wire.h>
#include <MPU6050_light.h>
MPU6050 mpu(Wire);
unsigned long timer = 0;



float gyroXerror = 0.07;
float gyroYerror = 0.03;
float gyroZerror = 0.01;
unsigned long lastTime = 0;

float gyroX, gyroY, gyroZ;

#include<WiFi.h>
#include <ESPAsyncWebServer.h>
#include<WebSocketsServer.h>
#include<SPIFFS.h>


#include<ArduinoJson.h>

StaticJsonDocument<200> doc_tx;
StaticJsonDocument<200> doc_rx;



#include <LiquidCrystal.h>
LiquidCrystal lcd(19, 23, 18, 17, 16, 15);

#include "ESP32_MailClient.h"

#include <WiFi.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);
WebSocketsServer socket=WebSocketsServer(81);


int interval=1;



const char* ssid     = "ECE_Lab_2_4GHz";
const char* password = "global123";


IPAddress local_IP(172, 16, 0, 91);
IPAddress gateway(172, 16, 0, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   
IPAddress secondaryDNS(8, 8, 4, 4); 

#define emailSenderAccount    "sugamn1ga20cs184@gmail.com"
#define emailSenderPassword   "rnrb bxoh jbov tcvk"
#define smtpServer            "smtp.gmail.com"
#define smtpServerPort        587
#define emailSubject          "--- OTP ---"

SMTPData smtpData;

String OTP;
boolean box_locked=true;


const char index_html[] PROGMEM = R"rawliteral(


<!DOCTYPE html>
<html lang="en">
  <head>
    <style>
     
     @import url("https://fonts.googleapis.com/css2?family=Poppins:wght@200;300;400;500;600;700&display=swap");
      * {
        margin: 0;
        padding: 0;
        box-sizing: border-box;
        font-family: "Poppins", sans-serif;
      }
      body {
        min-height: 100vh;
        display: flex;
        align-items: center;
        justify-content: center;
        padding: 20px;
        /* background: rgb(130, 106, 251); */
        /* background-color: #0000FF; */
      }
       .container {
        position: relative;
        max-width: 700px;
        width: 100%;
        background: #fff;
        padding: 25px;
        border: 1px solid black;
        border-radius: 8px;
        box-shadow: 0 0 20px rgba(0, 0, 0, 0.1);
      }
      .container header {
        font-size: 1.7rem;
        color: #333;
        font-weight: 500;
        text-align: center;

      }
      .container .form {
        margin-top: 30px;
      }
      .form .input-box {
        width: 100%;
        margin-top: 20px;
        display: flex;
        flex-direction: column;
        align-items: center;
      }
      .input-box label {
        color: #333;
      }
      .form :where(.input-box input, .select-box) {
        position: relative;
        height: 50px;
        width: 100%;
        outline: none;
        font-size: 1rem;
        color: #707070;
        margin-top: 8px;
        border: 1px solid #ddd;
        border-radius: 6px;
        padding: 0 15px;
      }
      .input-box input:focus {
        box-shadow: 0 1px 0 rgba(0, 0, 0, 0.1);
      }
      .form .column {
        display: flex;
        column-gap: 15px;
      }
      .form #submit {
        height: 55px;
        width: 100%;
        color: #fff;
        font-size: 1.1rem;
        font-weight: 400;
        margin-top: 30px;
        border: none;
        cursor: pointer;
        transition: all 0.2s ease;
        background: rgb(130, 106, 251);
      }
      .form #submit:hover {
        background: rgb(88, 56, 250);
      }
      /*Responsive*/
      @media screen and (max-width: 500px) {
        .form .column {
          flex-wrap: wrap;
        }
      
      }
  
  </style>
  </head>
  <body>
   
        <section class="container">
          <header>Authentication Form</header>
          <form action="/get" class="form">

            <br><br>

            <div class="input-box">
              <label style="font-size: larger; font-weight: 500;">OTP</label>
              <input style="text-align: center;" name="OTP" type="text" placeholder="Enter OTP"  required />
            </div>

            <br><br>
           
            <input type="submit" id="submit">

          </form>
        </section>
 
  </body>
</html>



)rawliteral";



String recipient_email="";
boolean received_email=false;

unsigned long previous=0;
int ctr=3;

void home_disp(){
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("E-Product");
  lcd.setCursor(9, 1);
  lcd.print("Shield");
}


void counter(){
  
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("container locks");
    

  while(ctr>=0){
    
    
  unsigned long cur=millis();
    if(cur - previous >= 3000){
      
        Serial.println(ctr);
        lcd.setCursor(4, 1);
        lcd.print(String("in ") + String(ctr) + String(" Sec.."));
        previous=cur;
        ctr--;
    }  
  }
 
  digitalWrite(14,HIGH);
  home_disp();
  digitalWrite(14,HIGH);
  return;
}


bool send_otp=false;


void sendCallback(SendStatus msg) {
  
  Serial.println(msg.info());
  return;

}


void send_OTP(){
   
Serial.println("Sending email...");
  
smtpData.setLogin(smtpServer, smtpServerPort, emailSenderAccount, emailSenderPassword);
  
 smtpData.setSender("ESP32-CAM", emailSenderAccount);
  
 smtpData.setPriority("High");

smtpData.setSubject(emailSubject);
    
smtpData.setMessage(String(OTP), true);
 

//smtpData.addRecipient(recipient_email);
smtpData.addRecipient("mytechno062001@gmail.com");


 
smtpData.setSendCallback(sendCallback);
  
if (!MailClient.sendMail(smtpData))
    Serial.println("Error sending Email, " + MailClient.smtpErrorReason());

smtpData.empty();
}


void send_impact_alert(){
  
Serial.println("Sending email...");
  
smtpData.setLogin(smtpServer, smtpServerPort, emailSenderAccount, emailSenderPassword);
  
 smtpData.setSender("ESP32-CAM", emailSenderAccount);
  
 smtpData.setPriority("High");

smtpData.setSubject("Severe Impact Detected");
    
smtpData.setMessage("Product might have been damaged due to crash/collision", true);
 

//smtpData.addRecipient(recipient_email);
smtpData.addRecipient("mytechno062001@gmail.com");


 
smtpData.setSendCallback(sendCallback);
  
if (!MailClient.sendMail(smtpData))
    Serial.println("Error sending Email, " + MailClient.smtpErrorReason());

smtpData.empty();
return;

}


void validate_OTP(String otp){

  

  if(otp==OTP){
   digitalWrite(14,LOW);
    Serial.println("OTP matched");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("OTP Verified..");
    delay(500);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Container");
    lcd.setCursor(5,1);
    lcd.print("Unlocked ..");
    delay(500);
    counter();
  }else{
       
        digitalWrite(14,HIGH);
        Serial.println("OTP not matched");
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("..OTP Failed..");
        lcd.setCursor(3, 1);
        lcd.print(".. Retry ..");
        delay(1500);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(WiFi.localIP());
        lcd.setCursor(12, 0);
        lcd.print("/otp");
        lcd.setCursor(0, 1);
        lcd.print(".. OTP Mailed ..");
    }

}



/*---------------------- RECEIVE RECIPIENT EMAIL ---------------------*/


const String url = "http://172.16.0.92/recipient_email";


void get_recipient(){
  
  Serial.println("Fetching " + url );

  HTTPClient http;
  http.begin(url);

  int httpResponseCode = http.GET();
  if (httpResponseCode > 0 && httpResponseCode<=300) {
    Serial.print("HTTP ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println();
    recipient_email=payload;
    Serial.println(recipient_email);
    received_email=true;
    digitalWrite(13,LOW);
    digitalWrite(14,HIGH);

  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
    Serial.println(":-");
    received_email=false;
    digitalWrite(14,LOW);
    digitalWrite(13,HIGH);
    delay(100);
    digitalWrite(13,LOW);
    
  }
  return;
  
}



/*---------------------- IR ACTIVATE  ---------------------*/


const String url2 = "http://172.16.0.92/ir_active";


void ir_active(){
  
  Serial.println("Fetching " + url2 );

  HTTPClient http;
  http.begin(url2);

  int httpResponseCode = http.GET();
  if (httpResponseCode > 0 && httpResponseCode<=300) {
      digitalWrite(13,HIGH);
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Sending");
      lcd.setCursor(5, 1);
      lcd.print("Photo..");
      home_disp();
    Serial.print("HTTP ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
    
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
    Serial.println(":-");
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Sending");
      lcd.setCursor(5, 1);
      lcd.print("Failed ..");
      home_disp();
  }
  return;
  
}



void setup() {

Serial.begin(115200);
lcd.begin(16, 2);

pinMode(12,INPUT);
pinMode(14,OUTPUT); //IR Sensor
pinMode(4,INPUT);
pinMode(13,OUTPUT);

  
if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("*-----*");
    digitalWrite(13,HIGH);
    
  } else {
    Serial.println("STA connected .!.!.!");
    digitalWrite(13,LOW);
    digitalWrite(14,HIGH);
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("........");  
  }

  
  Serial.print("Connecting to ");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(13,HIGH);
  }
  

  digitalWrite(13,LOW);

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  
  Wire.begin();
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while (status != 0) {
    Serial.println("finding MPU6050..!");
    delay(100);
    digitalWrite(13,HIGH);
  }

    digitalWrite(13,HIGH);
  
    
  Serial.println(" MPU6050 found..");
  
  mpu.calcOffsets(); // gyro and accelero
  Serial.println("Calculated offsets..\n");

  while(!SPIFFS.begin()){
  Serial.println("mounting sSPIFFS..!");
  }

  Serial.println("SPIFFS MOUNTED SUCCESSFULLY..");
  


  home_disp();
  
  
  String v1=String(random(10));
  String v2=String(random(10));
  String v3=String(random(10));
  String v4=String(random(10));
  OTP=v1+v2+v3+v4;


  server.on("/",HTTP_GET,[](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", "E-Product Shield"); 
  });


  server.serveStatic("/",SPIFFS,"/");


  server.on("/mpu",HTTP_GET,[](AsyncWebServerRequest *request){
    mpu.calcOffsets(); // gyro and accelero
    Serial.println("Calculated offsets..\n");
    request->send(SPIFFS,"/index.html","text/html");
  });



  server.on("/open", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send_P(200, "text/plain", "Chech Email for OTP");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(WiFi.localIP());
      lcd.setCursor(12, 0);
      lcd.print("/otp");
      send_otp=true;
      lcd.setCursor(0, 1);
      lcd.print(".. OTP Mailed ..");
      Serial.println(OTP);
  });
  

  
  server.on("/otp", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send_P(200, "text/html", index_html);
  });
  

  
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
  
      if (request->hasParam("OTP")) {
        String inputMessage;
        inputMessage = request->getParam("OTP")->value();
        Serial.println(inputMessage);
        validate_OTP(inputMessage);
      }

      request->send(200, "text/plain", "OTP Sent successfully ..");
      
   });
  
  
  socket.begin(); 
  server.begin();
  
  digitalWrite(14,LOW);

   while(!received_email){
    get_recipient();
    Serial.print("..");
  }


}


void loop() {

digitalWrite(14,HIGH);
  
  socket.loop();
  mpu.update();
  
  if(send_otp){
    
    send_OTP();
    send_otp=false;
    
  }

  
  int piezo=analogRead(12);

    
  float gyroX_temp =  mpu.getAngleX();
    if(abs(gyroX_temp) > gyroXerror)  {
      gyroX = (gyroX_temp/75);
       gyroX = gyroX_temp/50.00;
      
  }
  
  
  float gyroY_temp = mpu.getAngleY() ;
    if(abs(gyroY_temp) > gyroYerror) {
      gyroY = (gyroY_temp/75);
      gyroY = gyroY_temp/70.00;
  }
  
  
  float gyroZ_temp = mpu.getAngleZ() ;
    if(abs(gyroZ_temp) > gyroZerror) {
      gyroZ = (gyroZ_temp/75);
      gyroZ = gyroZ_temp/90.00;
  }
  
  
  if ((millis() - lastTime) > 10) {
 
    String json_data="";
    JsonObject obj=doc_tx.to<JsonObject>();
    obj["Gyro_X"]=((int)(gyroX*100))/100.0;
    obj["Gyro_Y"]=((int)(gyroY*100))/100.0;
    obj["Gyro_Z"]=((int)(gyroZ*100))/100.0;
    
      serializeJson(doc_tx,json_data);
      socket.broadcastTXT(json_data);


    if(((mpu.getAccX()>=1.51) || (mpu.getAccX()<=-1.51)) || ((mpu.getAccY() >=1.51) || (mpu.getAccY() <=-1.51)))
    {
//      int pir=digitalRead(12);
//      Serial.println(pir);
      if(received_email){  
          digitalWrite(13,HIGH);
          send_impact_alert();
          delay(100);
          digitalWrite(13,LOW);
      }
    
  }
      
    lastTime = millis();
 
  }


  if(digitalRead(4)){
    delay(500);
    ir_active();     
  }
  digitalWrite(13,LOW);
  
  

}
