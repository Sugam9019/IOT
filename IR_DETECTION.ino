

#include "esp_camera.h"
#include "SPI.h"
#include "driver/rtc_io.h"
#include "ESP32_MailClient.h"
#include <FS.h>
#include <SPIFFS.h>
#include <WiFi.h>

/*----------------------ASYNC WEB SERVER----------------*/

#include <ESPAsyncWebServer.h>
AsyncWebServer server(80);
AsyncEventSource events("/events");

/*------------------------------------------------------*/


/*--------------------- EMail To User HTML ---------------------------*/

const char mail_html[] PROGMEM = R"rawliteral(

  
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
      }
      .container {
        position: relative;
        max-width: 700px;
        width: 100%;
        background: #fff;
        padding: 25px;
        border-radius: 8px;
        box-shadow: 0 0 15px rgba(0, 0, 0, 0.1);
      }
      .container header {
        font-size: 1.5rem;
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
      
      .form :where(.gender-option, .gender) {
        display: flex;
        align-items: center;
        column-gap: 50px;
        flex-wrap: wrap;
      }
      
      .form :where(.gender input, .gender label) {
        cursor: pointer;
      }
      .gender label {
        color: #707070;
      }
      .address :where(input, .select-box) {
        margin-top: 15px;
      }
      .select-box select {
        height: 100%;
        width: 100%;
        outline: none;
        border: none;
        color: #707070;
        font-size: 1rem;
      }
      .form #submit {
        height: 55px;
        width: 100%;
        color: #fff;
        font-size: 1rem;
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
        .form :where(.gender-option, .gender) {
          row-gap: 15px;
        }
      }

  </style>
  </head>
  <body>
   
        
      <section class="container">
        <header>Add Customer</header>
        <form action="/get_email" class="form">
          <div class="input-box">
            <label>Full Name</label>
            <input type="text"  placeholder="Enter full name" required />
          </div>
  
          <div class="input-box">
            <label>Email Address</label>
            <input type="email" name="email" placeholder="Enter customer Email" required />
          </div>
  
          <div class="column">
            <div class="input-box">
              <label>Phone Number</label>
              <input type="number" placeholder="Enter phone number" required />
            </div>
            <div class="input-box">
              <label>Birth Date</label>
              <input type="date" placeholder="Enter birth date" required />
            </div>
          </div>
          
          <div class="input-box address">
            <label>Address</label>
            <input type="text" placeholder="Enter street address" required />
            <div class="column">
              <div class="select-box">
                <select>
                  <option hidden>Country</option>
                  <option>America</option>
                  <option>Japan</option>
                  <option>India</option>
                  <option>Nepal</option>
                </select>
              </div>
              <input type="number" placeholder="Enter postal code" required />
            </div>
  
          <input type="submit" id="submit">
        </form>
    </section>        

    <script>
    
        let IP_address;
           if (!!window.EventSource) {
                  
                     var source = new EventSource('/events');
                     
                     source.addEventListener('open', function(e) {
                      console.log("Events started ...");
                     }, false);
                     
                     source.addEventListener('error', function(e) {
                      if (e.target.readyState != EventSource.OPEN) {
                        console.log("Events Disconnected");
                      }
                     }, false);
           }
          
     </script>
 
  </body>
</html>

)rawliteral";

/*------------------------------------------------*/



/*----------------------  Track Ststus HTML -------------*/

const char track_html[] PROGMEM = R"rawliteral(

<html>

<head>

    <style>
        

        #title{
            display: flex;
            align-items: center;
            justify-content: center;
        }

        #h1{
            font-size: 3.3vw;
            font-family: sans-serif;
        }

        #motion_capture{
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
        }

        #box{
            display: flex;
            align-items: center;
            justify-content: center;
            height: 150px;
            width: 500px;
            background-color: rgb(68, 118, 218);
            border-radius:5px ;
            border: none;
        }

        #status{
            font-size: 25px;
            font-family: sans-serif;
            color: white;
        }

        #sensor1_stats{
            height: 400px;
            width: auto;
            box-shadow: -10px -10px 15px rgba(255, 255, 255, 0.5),10px 10px 15px rgba(70, 70, 70, 0.12),inset -7px -10px 15px rgba(255, 255, 255, 0.5),inset 7px 7px 15px rgba(70, 70, 70, 0.12);;            /* border-radius: 5px; */
            padding: 10px 15px;
            display: flex;
            align-items: center;
            justify-content: center;
            flex-direction: column;
            overflow: hidden;
            margin: 50px 10px ;
          
        }

        #sensor1_stats ul{
            overflow: scroll;
            height: auto;
            width:auto;
            
        }       

        #sensor2_stats{

            height: 400px;
            width: auto;
            box-shadow: -10px -10px 15px rgba(255, 255, 255, 0.5),10px 10px 15px rgba(70, 70, 70, 0.12),inset -7px -10px 15px rgba(255, 255, 255, 0.5),inset 7px 7px 15px rgba(70, 70, 70, 0.12);;            /* border-radius: 5px; */
           padding: 10px 15px;
            display: flex;
            align-items: center;
            justify-content: center;
            flex-direction: column;
            overflow: hidden;
            margin: 50px 10px ;
         
        }

       
        #sensor2_stats ul{
            overflow: scroll;
            height: auto;
            width:auto;
            
        }

        ::-webkit-scrollbar {
            display: none;
        }

        #sensor1_stats p,#sensor2_stats p{
            font-size: 2.2vw;
            font-family: sans-serif;
        }

        #sensor1_stats ul li,#sensor2_stats ul li{
            font-size: 1.2vw;
            font-family: sans-serif;
        }
        

    </style>

</head>

<body>
    
    <div id="title">
        <p id="h1">SENSOR STATISTICS</p>
    </div>
        
    <div id="motion_capture">
    
        <div id="box">
            <p id="status">No Movement</p>
        </div>
    
    </div>
    
    
    <div id="sensor1_stats">
        
        <p>Camera Events</p>
    
        <ul id="list1">
        </ul>
    
    </div>
    
    <div id="sensor2_stats">
    
        <p>Email Events</p>
        
        <ul id="list2">
        </ul>
        
    </div>

<script>


let l1=document.getElementById("list1");
let l2=document.getElementById("list2");


if (!!window.EventSource) {
 var source = new EventSource('/events');
 
 source.addEventListener('open', function(e) {
  console.log("Events started ...");
 }, false);
 
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);
 
 source.addEventListener('camera_events', function(e) {
  console.log( e.data);
   li=document.createElement("li");
   li.appendChild(document.createTextNode(e.data));
   l1.appendChild(li);
 }, false);


  source.addEventListener('gmail_events', function(e) {
  console.log( e.data);
  li=document.createElement("li");
   li.appendChild(document.createTextNode(e.data));
   l2.appendChild(li);
 }, false);


 source.addEventListener('pir_events', function(e) {
  console.log( e.data);
      if(e.data=="1"){
        
        let box=document.getElementById("box");
        box.style.backgroundColor="red";
        let status=document.getElementById("status");
        status.style.color="white";
        status.innerHTML="Movenent Detected ..";
      }
      else{
        let box=document.getElementById("box");
        box.style.backgroundColor="#036ffc";
        let status=document.getElementById("status");
        status.style.color="white";
        status.innerHTML="No Movenent ..";
      }
  }, false);
  

  source.addEventListener('camera_clear', function(e) {
  console.log( e.data);
  var parent = document.getElementById("list1");
  while (parent.firstChild) {
    parent.removeChild(parent.lastChild);
  }
 }, false);


   source.addEventListener('email_clear', function(e) {
  console.log( e.data);
  var parent = document.getElementById("list2");
  while (parent.firstChild) {
    parent.removeChild(parent.lastChild);
  }
 }, false);
  
 }
 

</script>

</body>

</html>

)rawliteral";


/*------------------------------------------------------*/


const char* ssid     = "ECE_Lab_2_4GHz";
const char* password = "global123";

IPAddress local_IP(172, 16, 0, 92);
IPAddress gateway(172, 16, 0, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   
IPAddress secondaryDNS(8, 8, 4, 4);

/*--------- Gmail Initialization -----------------------*/

#define emailSenderAccount    "sugamn1ga20cs184@gmail.com"
#define emailSenderPassword   "rnrb bxoh jbov tcvk"
#define smtpServer            "smtp.gmail.com"
#define smtpServerPort        587
#define emailSubject          "ESP32-CAM Photo Captured"


boolean add_recipient=false;
String recipient_Email="";
boolean ir_active=false;

/*---------- Default Recipient Email Address -----------------*/

String inputMessage = "mytechno062001@gmail.com";

#define CAMERA_MODEL_AI_THINKER

#if defined(CAMERA_MODEL_AI_THINKER)
  #define PWDN_GPIO_NUM     32
  #define RESET_GPIO_NUM    -1
  #define XCLK_GPIO_NUM      0
  #define SIOD_GPIO_NUM     26
  #define SIOC_GPIO_NUM     27
  
  #define Y9_GPIO_NUM       35
  #define Y8_GPIO_NUM       34
  #define Y7_GPIO_NUM       39
  #define Y6_GPIO_NUM       36
  #define Y5_GPIO_NUM       21
  #define Y4_GPIO_NUM       19
  #define Y3_GPIO_NUM       18
  #define Y2_GPIO_NUM        5
  #define VSYNC_GPIO_NUM    25
  #define HREF_GPIO_NUM     23
  #define PCLK_GPIO_NUM     22
#else
  #error "Camera model not selected"
#endif

/*---------- The Email Sending data object contains config and data to send -------------*/


/*-------------- Photo File Name to save in SPIFFS ---------------------*/
#define FILE_PHOTO "/photo.jpg"

String IP_address;
bool IP_send=false;


/*----------- Callback function to get the Email sending status -----------*/

void sendCallback(SendStatus msg) {
  /*--------------- Print the current status ----------*/
  Serial.println(msg.info());
  events.send(String(msg.info()).c_str(),"gmail_events");

}




/*------------------ Check if photo capture was successful -----------------*/

bool checkPhoto( fs::FS &fs ) {
  File f_pic = fs.open( FILE_PHOTO );
  unsigned int pic_sz = f_pic.size();
  return ( pic_sz > 100 );
}




/*--------------------- Capture Photo and Save it to SPIFFS ------------------*/

void capturePhotoSaveSpiffs( void ) {


    
  events.send("1","camera_clear");
  
  camera_fb_t * fb = NULL; // pointer
  bool ok = 0; // Boolean indicating if the picture has been taken correctly

  do {
    // Take a photo with the camera
    Serial.println("Taking a photo...");
    events.send("Taking a photo...","camera_events");

    fb = esp_camera_fb_get();
    esp_camera_fb_return(fb);
    fb=NULL;
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      events.send("Camera capture failed","camera_events");
      return;
    }

    /*----------------- Photo file name  ------------------*/
    Serial.printf("Picture file name: %s\n", FILE_PHOTO);
    events.send(String("Image file name : photo.jpg").c_str(),"camera_events");
    File file = SPIFFS.open(FILE_PHOTO, FILE_WRITE);

    /*------------ Insert the data in the photo file ------------*/
    if (!file) {
      Serial.println("Failed to open file in writing mode");
      events.send("Failed to open file in writing mode","camera_events");
    }
    else {
      file.write(fb->buf, fb->len); // payload (image), payload length
      Serial.print("The picture has been saved in ");
      Serial.print(FILE_PHOTO);
      events.send(String("The picture has been saved in /photo.jpg").c_str(),"camera_events");
      Serial.print(" - Size: ");
      Serial.print(file.size());
      Serial.println(" bytes");
      events.send(String("file Size : "+String(file.size())+" bytes").c_str(),"camera_events");
    }
    /*--------------- Close the file  ---------------*/
    file.close();
    esp_camera_fb_return(fb);

    /*----------- check if file has been correctly saved in SPIFFS --------------*/
    ok = checkPhoto(SPIFFS);
  } while ( !ok );

}




/*---------------------- Sending Captured Photo -----------------------*/

void sendPhoto( void ) {

  SMTPData smtpData;
 
  events.send("1","email_clear");
  
  
  /*------------ Preparing email ---------------*/
  Serial.println("Sending email...");
  events.send("Sending Email .","gmail_events");
  
  /*------ Set the SMTP Server Email host, port, account and password -----------*/
  smtpData.setLogin(smtpServer, smtpServerPort, emailSenderAccount, emailSenderPassword);
  
  /*-------- Set the sender name and Email -----------*/
  smtpData.setSender("E-Product Shield", emailSenderAccount);
  
  /*------- Set Email priority or importance High, Normal, Low or 1 to 5 (1 is highest)-----*/
  smtpData.setPriority("High");

  /*----- Set the subject --------*/
  smtpData.setSubject("E-Product Mistreatment");
    
  /*---------- Set the email message in HTML format ------------*/
  smtpData.setMessage("Photo captured", true);
  events.send("Photo captured with ESP32-CAM and attached in this email","gmail_events");
  
  
  /*--------- Add recipients, can add more than one recipient ---------*/
  smtpData.addRecipient(recipient_Email);
 

  /*--------- Add attach files from SPIFFS ------------*/
  smtpData.addAttachFile(FILE_PHOTO, "image/jpg");
  
  /*---------- Set the storage type to attach files in your email (SPIFFS) ---------*/
  smtpData.setFileStorageType(MailClientStorageType::SPIFFS);

  smtpData.setSendCallback(sendCallback);
  
  /*------- Start sending Email, can be set callback function to track the status ---------*/
  if (!MailClient.sendMail(smtpData)){
    Serial.println("Error sending Email, " + MailClient.smtpErrorReason());
    events.send(String("Error sending Email, " + MailClient.smtpErrorReason()).c_str(),"gmail_events");
  }

  /*--------- Clear all data from Email object to free memory ---------*/
  events.send("0","pir_events");
  smtpData.empty();
 
 
}




/*-------------------------- Sending Personal Email --------------------*/

void send_email_to_recipient(String recipient_Email){
  
   SMTPData smtpData;
   Serial.println("Sending IP Address ..");
    
  smtpData.setLogin(smtpServer, smtpServerPort, emailSenderAccount, emailSenderPassword);
    
  smtpData.setSender("E-Product Shield", emailSenderAccount);
    
  smtpData.setPriority("High");
  
  smtpData.setSubject(" Your Product Info ");
//  String message= " Track product position  : 172.16.0.91/mpu" + "\n" + "Open the product shield : 172.16.0.91/open"+ "\n" + "Enter OTP : 172.16.0.91/otp";   
  String message = "Track product position  : 172.16.0.91/mpu"
                   "\n Open the product shield : 172.16.0.91/open"
                   "\n Enter OTP : 172.16.0.91/otp";
  smtpData.setMessage(message, true);
   
    
  smtpData.addRecipient(recipient_Email);
   
  smtpData.setSendCallback(sendCallback);
    
  if (!MailClient.sendMail(smtpData))
      Serial.println("Error sending Email, " + MailClient.smtpErrorReason());
  
  smtpData.empty();

  
}




/*--------------------------- Sending Email to admin ------------------*/

void send_email_to_admin(String recipient_Email){

  SMTPData smtpData;
   Serial.println("Sending IP Address ..");
    
  smtpData.setLogin(smtpServer, smtpServerPort, emailSenderAccount, emailSenderPassword);
    
   smtpData.setSender("E-Product Shield", emailSenderAccount);
    
   smtpData.setPriority("High");
  
  smtpData.setSubject(" Add customer ");
 smtpData.setMessage("172.16.0.92/add_customer", true);
   
    
  smtpData.addRecipient(recipient_Email);
   
  smtpData.setSendCallback(sendCallback);
    
  if (!MailClient.sendMail(smtpData))
      Serial.println("Error sending Email, " + MailClient.smtpErrorReason());
  
  smtpData.empty();

  
}




//////////////////////////////////////////////////////////////



/*-------------------------  VOID SETUP  --------------------------*/



void setup() {
  
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector



  pinMode(13,INPUT);
//  pinMode(13,OUTPUT);
  pinMode(12,OUTPUT);
  
  Serial.begin(115200);
  Serial.println();


/*--------Connect to Wi-Fi ---------------*/

  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
    digitalWrite(12,HIGH);
  } else {  
    Serial.println("STA configured ");
    digitalWrite(12,LOW);
    
}
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(12,HIGH);
  }
  digitalWrite(12,LOW);
  Serial.println();


/*--------------  SPIFFS INITIALIZATION  -----------*/  

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    ESP.restart();
    digitalWrite(12,HIGH);
  }else{
    delay(500);
    Serial.println("SPIFFS mounted successfully");
    digitalWrite(12,LOW);
  }
  
  Serial.print("IP Address: http://");
  IP_address=WiFi.localIP().toString();
  Serial.println(WiFi.localIP());
  

/*--------------------  Configuring Parameters  ------------------------*/
 
  camera_config_t config;
  config.grab_mode =CAMERA_GRAB_LATEST;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }


/*--------------------Initialize camera-----------------------*/

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    digitalWrite(12,HIGH);
    return;
  }else{

    if(IP_address.length()>0){
    IP_send=true;
    send_email_to_admin("sugamn1ga20cs184@gmail.com");
    digitalWrite(12,LOW);
    delay(100);

    }

  }



/*--------------- ESP32 WEB SERVER Events Handler ----------------*/



events.onConnect([](AsyncEventSourceClient *client){
  IP_send=true;
  if(client->lastId()){
    Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
  }
  client->send("Events Server started successfully...", NULL, millis(), 10000);
  Serial.println("client connected");
  IP_send=false;

});


server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plian", "WELCOME TO PACKAGE INFO TRACKER");
});


server.on("/add_customer", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", mail_html);
});


server.on("/track", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", track_html);
});


server.on("/get_email", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;

    if (request->hasParam("email")) {
      recipient_Email = request->getParam("email")->value();
      if(recipient_Email){
        add_recipient=true;
      }
       request->send(200, "text/plain", "OK");
      Serial.println(recipient_Email);
    }
    else {
      inputMessage = "Error try again";
      inputParam = "none";
       request->send(200, "text/plain", "Error ..!! ");
    }
   

});


server.on("/recipient_email", HTTP_GET, [](AsyncWebServerRequest *request){
  
  if((recipient_Email.length())>5){
    request->send(200, "text/plian",String( recipient_Email).c_str());
    Serial.println(recipient_Email);
    Serial.println("recipient sent successfully ...!!!");
  } else {
    request->send(-1, "text/plian","0 users");  
    Serial.println("recipient sent Failed ...!!!");
  }
  
});


server.on("/ir_active", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plian","OK");
    ir_active=true;
});


server.addHandler(&events);
server.begin();
  

}


//////////////////////////////////////



/*---------------- VOID LOOP ----------------------*/


void loop() {


   if(add_recipient){
    
    if(recipient_Email){
          send_email_to_recipient(recipient_Email);
    }
    add_recipient=false;
  }


  if(ir_active){
    digitalWrite(12,HIGH);
    capturePhotoSaveSpiffs();
    delay(1000);
    sendPhoto();
    ir_active=false;
     digitalWrite(12,LOW);
  }
  

  

}


////////////////////////////////////
