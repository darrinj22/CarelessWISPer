/************************************************************************************************************************************************  
 * Function: database 
 * Author:   DJ Hansen @ North Carolina State University  
 * Date: 11/29/21
 * From:  
 * About: connects to wifi, converts collected tag data to string, and sends an HTTP request to the apache web server setup using xampp. 
 * 
 **********************************************************************************************************************************************/
void database(){
  unsigned long curr_time_db = millis();
                                                                          // setup wifi connection 
  if(WiFi.status() != WL_CONNECTED){
    connect_to_wifi(); 
  }
    convert_epc_string(); 

    for(int h = 0; h < count_entries(); h++){
      queryString = "?epc=" + final_epc[h]; 
      http.begin(HOST_NAME + PATH_NAME + queryString); //HTTP
      httpCode = http.GET();
  
                                                                         
      if(httpCode > 0) {                                                   // httpCode will be negative on error
        if(httpCode == HTTP_CODE_OK) {
        payload = http.getString();
        Serial.println(payload);
        Serial.println(httpCode);
        Serial.println("here"); 
      } else {                                                            // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      }
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
  
      http.end();
      
    }
    
    clear_arrays_db();
    prev_time_db = curr_time_db;
    if(!first_flag){
      first_flag = 1; 
  }  
}


/************************************************************************************************************************************************  
 * Function: connect_to_wifi
 * Author:   DJ Hansen @ North Carolina State University  
 * Date: 11/29/21
 * From:  
 * About: sets up wifi connection
 * 
 **********************************************************************************************************************************************/
void connect_to_wifi(void){
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.println("Connecting");
    while(WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
}
/************************************************************************************************************************************************  
 * Function: convert_epc_string
 * Author:   DJ Hansen @ North Carolina State University  
 * Date: 11/29/21
 * From:  
 * About: simple integer array to string
 * 
 **********************************************************************************************************************************************/
void convert_epc_string(void){
  for(int i = 0; i < count_entries(); i++){
    for(int j = 0; j < 12; j++){
      final_epc[i] += String(EPC_recv[i][j]); 
    }
  }
}
/************************************************************************************************************************************************  
 * Function: clear_arrays_db
 * Author:   DJ Hansen @ North Carolina State University  
 * Date: 11/29/21
 * From:  
 * About: same as clear_arrays
 **********************************************************************************************************************************************/
void clear_arrays_db(void){
    for(int i = 0; i < EPC_COUNT; i++){
      for(int j = 0; j < 12; j++){
       EPC_recv[i][j] = 0; 
       final_epc[i] = ""; 
      } 
    }
}
