

#include "Adafruit_SHT4x.h"
#include <SPI.h>
#include <SD.h>
//#include <Wire.h>




Adafruit_SHT4x sht4 = Adafruit_SHT4x();
static const int SD_CS_PIN = 10;        //Select pin for MicroSD module

void setup( )
{
  Serial.begin( 9600 );
  //Wire.begin();


  // Initialize the SHT41 sensor 
  if (!sht4.begin())
  {
    Serial.println("Sensor initialization failed!");
    while (1) delay (1);
  }
  Serial.println("Found temp sensor");
  Serial.print(sht4.readSerial(), HEX);
  sht4.setPrecision(SHT4X_HIGH_PRECISION);
  sht4.setHeater(SHT4X_NO_HEATER);



  // Initialize & check SD card
  Serial.print("Initializing SD card...");
  if ( !SD.begin( SD_CS_PIN ) )
  {
    Serial.println("SD card initialization failed!"); 
    return;
  }
  Serial.println("SD card initialized.");

  // Write CSV header if file doesn't already exist
  if ( !SD.exists("LOG.csv") )
  {
    File dataFile = SD.open("LOG.csv", FILE_WRITE); //initalize data file 
    if ( dataFile )
    {
      dataFile.println("Time (ms),Temperature (C),Humidity (%)");
      dataFile.close();
    }
  }
}

static bool measure_environment( float *temperature, float *humidity )
{
  static unsigned long measurement_timestamp = millis( );

  
  if( millis( ) - measurement_timestamp > 3000ul ) //meaasurement time - rn it's every 4 seconds -> will be modified for final code
  {
   sensors_event_t hum_event, temp_event;
   sht4.getEvent(&hum_event, &temp_event);

   *temperature = temp_event.temperature;
   *humidity = hum_event.relative_humidity;
   measurement_timestamp = millis();
   return true;
  }

  return false;
}


// Log a temperature and humidity reading to the microSD card.
 
void log_to_sd( float temperature, float humidity )
{
  File dataFile = SD.open("LOG.csv", FILE_WRITE); 
  if ( dataFile )
  {
    dataFile.print( millis() );
    dataFile.print( "," );
    dataFile.print( temperature, 2 );
    dataFile.print( "," );
    dataFile.println( humidity, 2 );
    dataFile.close();   // Close file 
  }
  else
  {
    Serial.println("Error opening LOG.csv"); //check if it works 
  }
}


void loop( )
{
  float temperature;
  float humidity;


  if( measure_environment( &temperature, &humidity ) == true ) // measure temp and humidity 
  {
    Serial.print( "T = " );
    Serial.print( temperature, 2 );
    Serial.print( " deg. C, H = " );
    Serial.print( humidity, 2 );
    Serial.println( "%" );

    log_to_sd( temperature, humidity );   // log to microSD
  }
}