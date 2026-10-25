
//Disclosures - Elgoo tutorial for temp sensor used to help formulate code. 

#include <dht_nonblocking.h>
#include <SPI.h>
#include <SD.h>
#define DHT_SENSOR_TYPE DHT_TYPE_11 //temp sensor -> to be replaced by SHT41 

static const int DHT_SENSOR_PIN = 9;
static const int SD_CS_PIN = 10;        // Chip Select pin for MicroSD module
DHT_nonblocking dht_sensor( DHT_SENSOR_PIN, DHT_SENSOR_TYPE ); //from Git opensource (https://github.com/olewolf/DHT_nonblocking) //will be changed with new sensor's


void setup( )
{
  Serial.begin( 9600 );

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
    if( dht_sensor.measure( temperature, humidity ) == true )
    {
      measurement_timestamp = millis( );
      return( true );
    }
  }

  return( false );
}


// Log a temperature and humidity reading to the microSD card.
 
void log_to_sd( float temperature, float humidity )
{
  File dataFile = SD.open("LOG.csv", FILE_WRITE); 
  if ( dataFile )
  {
    dataFile.print( millis() );
    dataFile.print( "," );
    dataFile.print( temperature, 1 );
    dataFile.print( "," );
    dataFile.println( humidity, 1 );
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
    Serial.print( temperature, 1 );
    Serial.print( " deg. C, H = " );
    Serial.print( humidity, 1 );
    Serial.println( "%" );

    log_to_sd( temperature, humidity );   // log to microSD
  }
}