#include "esp_camera.h"
//#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include<HTTPClient.h>
#define CAMERA_MODEL_AI_THINKER // Has PSRAM

#include "camera_pins.h"

//LiquidCrystal_I2C lcd(0x3F,16,4);

#define SECRET_SSID "jawhawk"
#define SECRET_PASSWORD "jawhawk160"
//#define I2C_SDA 14
//#define I2C_SCL 15

char ssid[] = SECRET_SSID;
char password[] = SECRET_PASSWORD;

int laststate = HIGH;
int currentstate;
bool isPosting = false;

const char* rootCACert =
"-----BEGIN CERTIFICATE-----\n"
"MIIEdDCCA1ygAwIBAgISAy+5gQJhwiUXLNqBcccmJw1ZMA0GCSqGSIb3DQEBCwUA\n"
"MDIxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MQswCQYDVQQD\n"
"EwJSMzAeFw0yNDA0MDYxMjUxNTRaFw0yNDA3MDUxMjUxNTNaMB8xHTAbBgNVBAMM\n"
"FCouYS5mcmVlLnBpbmdneS5saW5rMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAE\n"
"vgU7pqcuH4X2jHdF3hjTlC4uc8TWHcFt6D4mqGsXfbqVrpnU0zFjOEgYvgRrAsaZ\n"
"5josTRC6YR/7/LhVjpeAhaOCAmAwggJcMA4GA1UdDwEB/wQEAwIHgDAdBgNVHSUE\n"
"FjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwDAYDVR0TAQH/BAIwADAdBgNVHQ4EFgQU\n"
"k3cL9yqnpmylCSE+QojxeTrT9ygwHwYDVR0jBBgwFoAUFC6zF7dYVsuuUAlA5h+v\n"
"nYsUwsYwVQYIKwYBBQUHAQEESTBHMCEGCCsGAQUFBzABhhVodHRwOi8vcjMuby5s\n"
"ZW5jci5vcmcwIgYIKwYBBQUHMAKGFmh0dHA6Ly9yMy5pLmxlbmNyLm9yZy8waQYD\n"
"VR0RBGIwYIIUKi5hLmZyZWUucGluZ2d5LmxpbmuCDyouYS5waW5nZ3kubGlua4IU\n"
"Ki5mcmVlLmEucGluZ2d5LmxpbmuCEmEuZnJlZS5waW5nZ3kubGlua4INYS5waW5n\n"
"Z3kubGluazATBgNVHSAEDDAKMAgGBmeBDAECATCCAQQGCisGAQQB1nkCBAIEgfUE\n"
"gfIA8AB1ADtTd3U+LbmAToswWwb+QDtn2E/D9Me9AA0tcm/h+tQXAAABjrOuF8UA\n"
"AAQDAEYwRAIgch9wSLHrilSyjhy6VqmCvNiYB5QAmYXSCQ8MOc+sIr8CIEf1JugV\n"
"uUlZp82HYHKJi5WxPs4DrXwmOc8Rw1etcprGAHcA3+FW66oFr7WcD4ZxjajAMk6u\n"
"Vtlup/WlagHRwTu+UlwAAAGOs64YsAAABAMASDBGAiEAiTzz0n7d10yB0ezEFrs5\n"
"IhqDZG5SEIB90pDU2PJWvV4CIQCjTAjrghNAl5c8H/X3hKiNxBTm2j7WveBtvyGj\n"
"8DjWvjANBgkqhkiG9w0BAQsFAAOCAQEAtCSyawuitb4yzVHae/GAgpOZaH/6JkKW\n"
"llACYgXt+iLs1ndLjcn2hBWCBQEbCYTAQ7FMBH8epuIlNJNdgU28Mr1xSgoGEFEe\n"
"P49VbdkhkUjv2N6Mw9LzjD3H3nb/bUqBHu5RaFlOntkrNeeU+D0wBUlXapXziNvi\n"
"c4tkeK7mtaqckqj2bG0NmYBgXZVoGZQ4rkF+QnUrgDMG6d/WoOwdWetHiF6fRec6\n"
"Ir1lXBlqB0OCfkrbhXVbwGZlbnOdcA+rkExse9C2NmlVeylw9pQnEduNrAiZGMvU\n"
"PXR3lAd+pf1rsA76BJqbsR/QRreiCHI3uS0t+vrHJuiPqImPgvF/1g==\n"
"-----END CERTIFICATE-----";

//const char* serverUrl = "nova.mydomain.com";
const char* serverUrl = "sso.a.pinggy.link";
const char* host = "sso.a.pinggy.link";
const int Port = 443;
const int ledPin = 2; 
const int buttonPin = 4; // GPIO pin connected to the tactile switch button

bool ledState = LOW; 

int contentLength = 0;
const char* boundry = "dgbfhfh";

WiFiClientSecure client;
 
void sendPhotoToServer() {

  
   Serial.printf("Connecting to %s:%d... ", host,Port);
   client.setInsecure();
   if (!client.connect(host,Port))
   {      
      Serial.println("Failure in connection with the server");      
      return;
   }
  digitalWrite(ledPin, HIGH);
  isPosting = true;
  Serial.println("LED ON");
  delay(500);
   sendDataToServer();
   digitalWrite(ledPin, LOW);
   isPosting = false;
  Serial.println("LED OFF");
  delay(500);
}

void sendDataToServer() {
//      digitalWrite(LED_GPIO_NUM, HIGH);
//      delay(1000);
        String start_request = "";
        String end_request = "";

        start_request = start_request + "--" + boundry + "\r\n";

        start_request = start_request + "Content-Disposition: form-data; name=\"image\"; filename=\"CAM.jpg\"\r\n";
        start_request = start_request + "Content-Type: image/jpg\r\n";
        start_request = start_request + "\r\n";

        end_request = end_request + "\r\n";


        end_request = end_request + "--" + boundry + "--" + "\r\n";

        /************************************************/  
      
        camera_fb_t * fb = NULL;
        // Take Picture with Camera
        fb = esp_camera_fb_get();  
        if(!fb) 
         {
           Serial.println("Camera capture failed");
           return;
         }

        /************************************************/     
         int contentLength = (int)fb->len + start_request.length() + end_request.length();    

         String headers = String("POST /upload_reading_image/ HTTP/1.1\r\n");
         headers = headers + "Host: " + serverUrl + "\r\n";
         headers = headers + "User-Agent: ESP32" + "\r\n";
         headers = headers + "Accept: */*\r\n";
         headers = headers + "Content-Type: multipart/form-data; boundary=" + boundry + "\r\n";
         headers = headers + "Content-Length: " + contentLength + "\r\n";
         headers = headers + "\r\n";
         headers = headers + "\r\n";

         Serial.println();
         Serial.println("Sending data to Server...");        

         Serial.print(headers);        
         client.print(headers);
         client.flush();

         Serial.print(start_request);
         client.print(start_request);
         client.flush();

       /*****************************************************/
       int iteration = fb->len / 1024;
       for(int i=0; i<iteration; i++)
       {
         client.write(fb->buf,1024);
         fb->buf += 1024;
//         Serial.println(i);
//         client.write(fb->buf + i * 1024, 1024);
 //        client.flush();
       }
       size_t remain = fb->len % 1024;
       client.write(fb->buf,remain);
//       client.flush();
       /****************************************************/  

       Serial.print(end_request);
       client.print(end_request);
//       client.flush();

       esp_camera_fb_return(fb);
}
      
//void initLCD() {
//  Serial.println("lcd print");
//  lcd.init();
//  // Print a message to the LCD.
//  lcd.backlight();
//delay(200);
//  lcd.setCursor(0,0);
//  lcd.print("Starting");
//  Serial.println("done");
////  lcd.setCursor(2,1);
////  lcd.print("Ywrobot Arduino!");
////  lcd.setCursor(0,2);
////  lcd.print("Arduino LCM IIC 2004");
////  lcd.setCursor(2,3);
////  lcd.print("Power By Ec-yuan!");
//}

void setup() {
  Serial.begin(115200);
  
  Serial.setDebugOutput(true);
  pinMode(LED_GPIO_NUM, OUTPUT);
  pinMode(ledPin, OUTPUT);
//  pinMode(buttonPin, INPUT_PULLUP);/
  
//  initLCD();/

  delay(1000);

  camera_config_t config;
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
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 5*1000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG; // for streaming
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 4;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_XGA;
    config.jpeg_quality = 4;
    config.fb_count = 1;
  }

  Serial.println("Connecting to Wifi");
//  WiFi.begin(ssid, password);

//  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
//  // wait 10 seconds for connection:
//    Serial.println("Trying to connecting to WiFi..");
//    delay(10000);
//  }
//  Serial.println("Wifi connected");

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  } else {
    Serial.println("Camera init successful!");
  }
  
//  // wifi connection
  Serial.println("Connecting to wifi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
  // wait 10 seconds for connection:
  Serial.println("Connecting to WiFi..");
    delay(1000);
  }
  Serial.println("Wifi connected");

    sendPhotoToServer(); 
}

void loop() {
//  currentstate = digitalRead(buttonPin);
//  Serial.println(currentstate);
//  if (currentstate == HIGH) {
//    digitalWrite(ledPin, HIGH);
//    Serial.println("LED ON")
//  } else {
//    digitalWrite(ledPin, LOW);
//    Serial.println("LED OfF")
//  }
}
