/* 

API생성 사이트 : ThingSpeak: https://thingspeak.com/ 

*/

#include <ESP8266WiFi.h>                                                
#include <ESP8266HTTPClient.h>
#include <LiquidCrystal_I2C.h>
 
#include <Wire.h> // 1.6.5이전 버전

LiquidCrystal_I2C lcd(0x27, 16, 2); // lcd주소 정의
  
  //const char* ssid = "geekble_2G";          // WIFI이름                             
  //const char* password = "geekble71762";    // WIFI비밀번호
  const char* ssid = "iPhone";          // WIFI이름                             
  const char* password = "12345678";     // WIFI비밀번호
  
  const char* host = "api.thingspeak.com";  // 값을 받아오는 사이트(Host)
  const int httpPortRead = 80;                                           
                                                  
  const char* url1 = "/apps/thinghttp/send_request?api_key=EOJ8LVY4I65CNAPP"; // 전일대비 ex)+11.19
    
  String Data_Raw; // 데이터 저장 변수

  WiFiClient client; // 와이파이, http client 생성                                                     
  HTTPClient http; 

  int ENA = 14; /* GPIO14(D5) -> Motor-A Enable */
  int IN1 = 15;     /* GPIO2(D4) -> IN2   */
  int IN2 = 13;    /* GPIO0(D3) -> IN4   */
  int IN3 = 2;   /* GPIO15(D8) -> IN1  */  
  int IN4 = 0;  /* GPIO13(D7) -> IN3  */
  int ENB = 12;  /* GPIO12(D6) -> Motor-B Enable */
 
  
  int BUT = 3;
  int PHO = 16;

void setup() {
    Serial.begin(115200);
    WiFi.disconnect(); // 이전 와이파이 접속 해제 
    
    lcd.begin(); 
    lcd.setCursor(1, 0);
    lcd.print("WIFI CONNECTING");  

    delay(1000);                                                                  
    WiFi.begin(ssid, password);                                                   
    Serial.println("Connected to the WiFi network");   //Display feedback on the serial BUTitor                                        
    Serial.println(WiFi.localIP());
    
    

    pinMode(ENA, OUTPUT); // GPIO12(D6) -> Motor-B Enable
    pinMode(IN1, OUTPUT); // GPIO2(D4) -> IN2
    pinMode(IN2, OUTPUT); // GPIO0(D3) -> IN4
    pinMode(IN3, OUTPUT); // GPIO15(D8) -> IN1
    pinMode(IN4, OUTPUT); // GPIO13(D7) -> IN3
    pinMode(ENB, OUTPUT); // GPIO14(D5) -> Motor-A Enable
    

    pinMode(BUT, INPUT_PULLUP); // 스위치
    pinMode(PHO, INPUT); // 포토인터럽트
}

void loop() {

  if(digitalRead(PHO)){

      if( http.begin(host,httpPortRead,url1)) { // host와 url 연결  
        
          int httpCode = http.GET(); // 응답이 있다면 신호를 반환                                            
          if (httpCode > 0) {                                                             
            if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY){ 


                lcd.setCursor(1, 0);
                lcd.print("WIFI CONNECTED!");  
                
                delay(1000);
                lcd.clear();

                
                Data_Raw = http.getString();   // Here we store the raw data string
    
                Serial.print("Data_Raw[0] : "); // 테스트테스트 <<<<<<<<<<<<<<<<<<<<<<<
                Serial.println(Data_Raw[0]); // 테스트테스트 <<<<<<<<<<<<<<<<<<<<<<<
                         
            }
          }
          else //If we can't get data
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str()); 
          
          http.end();                                                                 
      } 
      else Serial.printf("[HTTP} Unable to connect\n"); // HTTP 접속 못했을때
            
      while (WiFi.status() != WL_CONNECTED) { // WIFI연결 끊기면                                        
        WiFi.disconnect();                                                        
        delay(1000);                                                             
        
        WiFi.begin(ssid, password);                                              
        Serial.println("Reconnecting to WiFi..");       
        delay(10000);                                                             
      }

      if (digitalRead(BUT) == LOW) { // 버튼 눌리면 값 임의로 바꾸기
        if(Data_Raw[0]=='-'){
          Data_Raw[0] = '+';
        }
        else {
          Data_Raw[0] = '-';
        }
      }
      
      
      lcd.setCursor(0, 0);
      lcd.print("Stock Change:");
      lcd.setCursor(0, 1);
      lcd.print(Data_Raw);


      digitalWrite(IN1,HIGH); // 모터 ON
      digitalWrite(IN2,LOW);
      analogWrite(ENA,150);

      delay(1000);

      digitalWrite(IN1,HIGH); // 모터 OFF
      digitalWrite(IN2,LOW);
      digitalWrite(ENA,LOW);

      delay(500);

    
      if(Data_Raw[0]=='-'){ // '주가변동가'가 '-' 일때
        digitalWrite(IN1,HIGH); // 모터 ON
        digitalWrite(IN2,LOW);
        analogWrite(ENA,150);

        delay(2000);

        digitalWrite(IN1,HIGH); // 모터 OFF
        digitalWrite(IN2,LOW);
        digitalWrite(ENA,LOW);

        digitalWrite(IN3,HIGH); // 파쇄 모터 ON
        digitalWrite(IN4,LOW);
        digitalWrite(ENB,HIGH);

        delay(5000);

        digitalWrite(IN3,HIGH); // 파쇄 모터 ON
        digitalWrite(IN4,LOW);
        digitalWrite(ENB,LOW);
      }
      else{ // '주가변동가'가 '+' 일때
        digitalWrite(IN1,LOW); // 모터 ON
        digitalWrite(IN2,HIGH);
        digitalWrite(ENA,HIGH);

        delay(1500);

        digitalWrite(IN1,LOW); // C모터 OFF
        digitalWrite(IN2,HIGH);
        digitalWrite(ENA,LOW);

        
      }
  }

}
