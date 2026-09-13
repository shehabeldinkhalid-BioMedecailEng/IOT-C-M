#include <SPI.h>
#include <SD.h>
#include <DHT.h>
#include <math.h>
#include <string.h>

/*==================================================
                    PIN CONFIGURATION
==================================================*/

#define DHT_PIN       A4
#define DHT_TYPE      DHT21

#define SD_CS_PIN     10

  /*
    UART:
    Arduino TX = D1
    Arduino RX = D0

    هنستخدم TX فقط لإرسال البيانات إلى ATmega640.
  */

/*==================================================
                    SENSOR OBJECT
==================================================*/

DHT dht(DHT_PIN, DHT_TYPE);

/*==================================================
                    SD CONFIGURATION
==================================================*/

const char DATA_FILE_NAME[] = "AM2305B.TXT";

  /*==================================================
                  PROJECT THRESHOLDS
  ==================================================*/

  /*
    هذه القيم هي حدود المشروع الحالية،
    وليست حدودًا طبية معتمدة.
  */

const float TEMP_MIN_NORMAL = 18.0;
const float TEMP_MAX_NORMAL = 37.5;

const float HUM_MIN_NORMAL  = 30.0;
const float HUM_MAX_NORMAL  = 60.0;

/*==================================================
                    GLOBAL VARIABLES
==================================================*/

float oldTemperature = 0.0;
float oldHumidity    = 0.0;

float newTemperature = 0.0;
float newHumidity    = 0.0;

float temperatureDifference = 0.0;
float humidityDifference    = 0.0;

bool oldTemperatureValid = false;
bool oldHumidityValid    = false;

/*==================================================
                    STATUS ENUM
==================================================*/

enum SensorStatus
{
  STATUS_LOW,
  STATUS_NORMAL,
  STATUS_HIGH,
  STATUS_ERROR
};

/*==================================================
                  FUNCTION PROTOTYPES
==================================================*/

SensorStatus getTemperatureStatus(float value);
SensorStatus getHumidityStatus(float value);

const char* statusToText(SensorStatus status);

bool initializeSDCard();
bool readLastReadingFromSD();
bool saveNewReadingToSD(float temperature, float humidity);

bool readAM2305B(float &temperature, float &humidity);

void sendErrorPacket(const char* errorText);

void sendDataPacket(
  float oldTemp,
  float newTemp,
  float diffTemp,
  SensorStatus tempStatus,
  float oldHum,
  float newHum,
  float diffHum,
  SensorStatus humStatus,
  bool oldDataValid
);

  /*==================================================
                          SETUP
  ==================================================*/

void setup()
{
    /*
      UART communication with ATmega640
    */
  Serial.begin(9600);

    /*
      Start sensor
    */
  dht.begin();

    /*
      Initialize SD card
    */
  if (!initializeSDCard())
  {
    sendErrorPacket("SD_INIT_FAILED");
  }

    /*
      Read the last saved temperature and humidity
      from the SD card.
    */
  if (!readLastReadingFromSD())
  {
      /*
        No previous reading is available.
        This is not a fatal error.
      */

    oldTemperatureValid = false;
    oldHumidityValid    = false;
  }

  delay(2000);
}

/*==================================================
                          LOOP
==================================================*/

void loop()
{
    /*
      Read current AM2305B values
    */
  if (!readAM2305B(newTemperature, newHumidity))
  {
    sendErrorPacket("SENSOR_READ_FAILED");

    delay(5000);
    return;
  }

    /*
      Determine sensor status
    */
  SensorStatus temperatureStatus =
    getTemperatureStatus(newTemperature);

  SensorStatus humidityStatus =
    getHumidityStatus(newHumidity);

    /*
      First reading:
      There is no real old value yet.
    */
  if (!oldTemperatureValid || !oldHumidityValid)
  {
    temperatureDifference = 0.0;
    humidityDifference    = 0.0;

    sendDataPacket(
      0.0,
      newTemperature,
      temperatureDifference,
      temperatureStatus,

      0.0,
      newHumidity,
      humidityDifference,
      humidityStatus,

      false
    );

      /*
        Save first reading to SD
      */
    if (!saveNewReadingToSD(newTemperature, newHumidity))
    {
      sendErrorPacket("SD_SAVE_FAILED");
    }

      /*
        The first reading becomes the old reading
        for the next cycle.
      */
    oldTemperature = newTemperature;
    oldHumidity    = newHumidity;

    oldTemperatureValid = true;
    oldHumidityValid    = true;

    delay(5000);
    return;
  }

    /*
      Calculate differences

      Difference = New - Old
    */
  temperatureDifference =
    newTemperature - oldTemperature;

  humidityDifference =
    newHumidity - oldHumidity;

    /*
      Send complete data packet to ATmega640
    */
  sendDataPacket(
    oldTemperature,
    newTemperature,
    temperatureDifference,
    temperatureStatus,

    oldHumidity,
    newHumidity,
    humidityDifference,
    humidityStatus,

    true
  );

    /*
      Save current reading to SD
    */
  if (!saveNewReadingToSD(newTemperature, newHumidity))
  {
    sendErrorPacket("SD_SAVE_FAILED");
  }

    /*
      Update old values
    */
  oldTemperature = newTemperature;
  oldHumidity    = newHumidity;

  delay(5000);
}

/*==================================================
                READ AM2305B SENSOR
==================================================*/

bool readAM2305B(float &temperature, float &humidity)
{
    /*
      Read humidity
    */
  float measuredHumidity = dht.readHumidity();

    /*
      Read temperature in Celsius
    */
  float measuredTemperature =
    dht.readTemperature();

    /*
      Check for invalid readings
    */
  if (isnan(measuredHumidity) ||
      isnan(measuredTemperature))
  {
    return false;
  }

    /*
      Check reasonable software limits
      to reject obviously invalid values.
    */
  if (measuredHumidity < 0.0 ||
      measuredHumidity > 100.0)
  {
    return false;
  }

  if (measuredTemperature < -40.0 ||
      measuredTemperature > 80.0)
  {
    return false;
  }

    /*
      Store valid readings
    */
  humidity    = measuredHumidity;
  temperature = measuredTemperature;

  return true;
}

/*==================================================
              TEMPERATURE STATUS
==================================================*/

SensorStatus getTemperatureStatus(float value)
{
  if (value < TEMP_MIN_NORMAL)
  {
    return STATUS_LOW;
  }

  if (value > TEMP_MAX_NORMAL)
  {
    return STATUS_HIGH;
  }

  return STATUS_NORMAL;
}

/*==================================================
                HUMIDITY STATUS
==================================================*/

SensorStatus getHumidityStatus(float value)
{
  if (value < HUM_MIN_NORMAL)
  {
    return STATUS_LOW;
  }

  if (value > HUM_MAX_NORMAL)
  {
    return STATUS_HIGH;
  }

  return STATUS_NORMAL;
}

  /*==================================================
                  STATUS TO TEXT
  ==================================================*/

const char* statusToText(SensorStatus status)
{
  switch (status)
  {
    case STATUS_LOW:
      return "LOW";

    case STATUS_NORMAL:
      return "NORMAL";

    case STATUS_HIGH:
      return "HIGH";

    default:
      return "ERROR";
  }
}

/*==================================================
                INITIALIZE SD CARD
==================================================*/

bool initializeSDCard()
{
    /*
      SD.begin() initializes SPI and SD card.
    */
  if (!SD.begin(SD_CS_PIN))
  {
    return false;
  }

  return true;
}

/*==================================================
              READ LAST READING FROM SD
==================================================*/

  /*
    File format:

    Temperature,Humidity

    Example:

    25.40,55.20
    26.10,57.00
    27.20,58.50

    The last line is the old reading.
  */

bool readLastReadingFromSD()
{
  if (!SD.exists(DATA_FILE_NAME))
  {
    return false;
  }

  File dataFile =
    SD.open(DATA_FILE_NAME, FILE_READ);

  if (!dataFile)
  {
    return false;
  }

  char lastLine[40];

  lastLine[0] = '\0';

    /*
      Read file line by line.
      Every new line replaces the previous one.
      At the end, lastLine contains the last reading.
    */
  while (dataFile.available())
  {
    char line[40];

    size_t bytesRead =
      dataFile.readBytesUntil('\n', line, sizeof(line) - 1);

    line[bytesRead] = '\0';

      /*
        Remove carriage return if present
      */
    for (uint8_t i = 0; i < bytesRead; i++)
    {
      if (line[i] == '\r')
      {
        line[i] = '\0';
        break;
      }
    }

      /*
        Ignore empty lines
      */
    if (strlen(line) > 0)
    {
      strncpy(lastLine, line, sizeof(lastLine) - 1);

      lastLine[sizeof(lastLine) - 1] = '\0';
    }
  }

  dataFile.close();

    /*
      No valid line found
    */
  if (strlen(lastLine) == 0)
  {
    return false;
  }

    /*
      Parse:

      Temperature,Humidity
    */
  float savedTemperature;
  float savedHumidity;

  int parsedValues =
    sscanf(lastLine, "%f,%f",
          &savedTemperature,
          &savedHumidity);

  if (parsedValues != 2)
  {
    return false;
  }

    /*
      Validate saved values
    */
  if (savedHumidity < 0.0 ||
      savedHumidity > 100.0)
  {
    return false;
  }

  if (savedTemperature < -40.0 ||
      savedTemperature > 80.0)
  {
    return false;
  }

    /*
      Store old values
    */
  oldTemperature = savedTemperature;
  oldHumidity    = savedHumidity;

  oldTemperatureValid = true;
  oldHumidityValid    = true;

  return true;
}

/*==================================================
              SAVE NEW READING TO SD
==================================================*/

bool saveNewReadingToSD(
  float temperature,
  float humidity
)
{
    /*
      Open file in append mode.
      New readings are added at the end.
    */
  File dataFile =
    SD.open(DATA_FILE_NAME, FILE_WRITE);

  if (!dataFile)
  {
    return false;
  }

    /*
      Format:

      Temperature,Humidity
    */

  dataFile.print(temperature, 2);
  dataFile.print(",");
  dataFile.println(humidity, 2);

  dataFile.close();

  return true;
}

/*==================================================
                  SEND ERROR PACKET
==================================================*/

void sendErrorPacket(const char* errorText)
{
    /*
      Example:

      <AM,ERROR,SD_INIT_FAILED>
    */

  Serial.print("<AM,ERROR,");
  Serial.print(errorText);
  Serial.println(">");
}

/*==================================================
                  SEND DATA PACKET
==================================================*/

  /*
    Packet format:

    <AM,
    OldDataValid,
    OldTemperature,
    NewTemperature,
    TemperatureDifference,
    TemperatureStatus,
    OldHumidity,
    NewHumidity,
    HumidityDifference,
    HumidityStatus
    >

    Example with previous data:

    <AM,1,36.50,37.00,0.50,NORMAL,55.00,58.00,3.00,NORMAL>

    Example for first reading:

    <AM,0,0.00,25.40,0.00,NORMAL,0.00,60.00,0.00,NORMAL>
  */

void sendDataPacket(
  float oldTemp,
  float newTemp,
  float diffTemp,
  SensorStatus tempStatus,

  float oldHum,
  float newHum,
  float diffHum,
  SensorStatus humStatus,

  bool oldDataValid
)
{
  Serial.print("<AM,");

  /*
    Old data valid flag
    1 = old data exists
    0 = first reading
  */
  if (oldDataValid)
  {
    Serial.print("1");
  }
  else
  {
    Serial.print("0");
  }

  Serial.print(",");

  /*
    Temperature data
  */
  Serial.print(oldTemp, 2);
  Serial.print(",");

  Serial.print(newTemp, 2);
  Serial.print(",");

  Serial.print(diffTemp, 2);
  Serial.print(",");

  Serial.print(statusToText(tempStatus));
  Serial.print(",");

  /*
    Humidity data
  */
  Serial.print(oldHum, 2);
  Serial.print(",");

  Serial.print(newHum, 2);
  Serial.print(",");

  Serial.print(diffHum, 2);
  Serial.print(",");

  Serial.print(statusToText(humStatus));

  /*
    End of packet
  */
  Serial.println(">");
}