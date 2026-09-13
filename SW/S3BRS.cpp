#include <SPI.h>
#include <SD.h>
#include "HX711.h"

/* =========================================================
 * ATmega328P - RS BOARD
 *
 * Sensor : MMR920
 * ADC    : HX711
 * Storage: SD Card
 * UART   : UART -> ATmega640 UART3
 *
 * Pin Mapping according to RS schematic:
 *
 * HX711 DT  -> PD8 / D8
 * HX711 SCK -> PD7 / D7
 *
 * SD:
 * CS       -> PD10 / D10
 * MOSI     -> PB3  / D11
 * MISO     -> PB4  / D12
 * SCK      -> PB5  / D13
 *
 * UART:
 * TX -> PD1
 * RX -> PD0
 * ========================================================= */


/* =========================================================
 * PIN DEFINITIONS
 * ========================================================= */

const uint8_t HX711_DOUT_PIN = 8;
const uint8_t HX711_SCK_PIN  = 7;
const uint8_t SD_CS_PIN      = 10;


/* =========================================================
 * OBJECTS
 * ========================================================= */

HX711 scale;
File logFile;


/* =========================================================
 * FILTER PARAMETERS
 * ========================================================= */

const float ALPHA = 0.08f;

float filteredValue = 0.0f;
bool firstRead = true;


/* =========================================================
 * RESPIRATORY SIGNAL / PEAK PARAMETERS
 * ========================================================= */

/*
 * IMPORTANT:
 * This threshold must be calibrated experimentally
 * with the real MMR920 + HX711 hardware.
 */

long threshold = 5000;

const unsigned long MIN_PEAK_INTERVAL = 1500UL;

unsigned long lastPeakTime = 0;

float previousFiltered = 0.0f;

bool isRising = false;


/* =========================================================
 * RR VARIABLES
 * ========================================================= */

float respiratoryRate = 0.0f;

float oldRR = -1.0f;
float newRR = -1.0f;
float difference = 0.0f;

bool hasOldRR = false;


/* =========================================================
 * FILE NAME
 * ========================================================= */

const char FILE_NAME[] = "respiration.csv";


/* =========================================================
 * IIR LOW PASS FILTER
 * ========================================================= */

float applyIIRFilter(long rawSignal)
{
    if (firstRead)
    {
        filteredValue = (float)rawSignal;
        firstRead = false;
    }
    else
    {
        filteredValue =
            (ALPHA * (float)rawSignal) +
            ((1.0f - ALPHA) * filteredValue);
    }

    return filteredValue;
}


/* =========================================================
 * UART SEND HELPERS
 * ========================================================= */

void sendFloat(float value, uint8_t decimals)
{
    Serial.print(value, decimals);
}


void sendTrend(const char* trend)
{
    Serial.print(trend);
}


/* =========================================================
 * DETERMINE TREND
 *
 * Project rule:
 *
 * New RR < Old RR -> IMPROVED
 * New RR > Old RR -> WORSE
 * New RR = Old RR -> NO_CHANGE
 *
 * This rule is a project classification rule and can be
 * changed later if the medical decision logic requires it.
 * ========================================================= */

const char* calculateTrend(float oldValue, float newValue)
{
    const float EPSILON = 0.05f;

    float diff = newValue - oldValue;

    if (diff > EPSILON)
    {
        return "WORSE";
    }
    else if (diff < -EPSILON)
    {
        return "IMPROVED";
    }
    else
    {
        return "NO_CHANGE";
    }
}


/* =========================================================
 * SEND COMPLETE RS PACKET
 *
 * Format:
 *
 * <RS,OLD_RR,NEW_RR,DIFF,TREND>
 *
 * Example:
 *
 * <RS,18.0,20.0,2.0,WORSE>
 * ========================================================= */

void sendRRPacket(float oldValue,
                  float newValue,
                  float diff,
                  const char* trend)
{
    Serial.print("<RS,");

    sendFloat(oldValue, 1);

    Serial.print(",");

    sendFloat(newValue, 1);

    Serial.print(",");

    sendFloat(diff, 1);

    Serial.print(",");

    sendTrend(trend);

    Serial.println(">");
}


/* =========================================================
 * READ OLD RR FROM SD
 *
 * CSV format:
 *
 * Time(ms),Raw,Filtered,RR(BPM)
 *
 * We search the entire file and keep the last valid RR.
 * ========================================================= */

bool readOldRRFromSD()
{
    if (!SD.exists(FILE_NAME))
    {
        return false;
    }

    logFile = SD.open(FILE_NAME, FILE_READ);

    if (!logFile)
    {
        return false;
    }

    bool found = false;

    /*
     * Skip header
     */
    logFile.readStringUntil('\n');

    while (logFile.available())
    {
        String line = logFile.readStringUntil('\n');

        if (line.length() == 0)
        {
            continue;
        }

        /*
         * Find:
         *
         * Time,Raw,Filtered,RR
         */

        int comma1 = line.indexOf(',');

        if (comma1 < 0)
        {
            continue;
        }

        int comma2 = line.indexOf(',', comma1 + 1);

        if (comma2 < 0)
        {
            continue;
        }

        int comma3 = line.indexOf(',', comma2 + 1);

        if (comma3 < 0)
        {
            continue;
        }

        String rrString =
            line.substring(comma3 + 1);

        float rrValue = rrString.toFloat();

        /*
         * Ignore zero / invalid RR
         */

        if (rrValue > 0.0f)
        {
            oldRR = rrValue;
            found = true;
        }
    }

    logFile.close();

    return found;
}


/* =========================================================
 * CREATE FILE IF REQUIRED
 * ========================================================= */

void initializeSDFile()
{
    if (!SD.exists(FILE_NAME))
    {
        logFile = SD.open(FILE_NAME, FILE_WRITE);

        if (logFile)
        {
            logFile.println(
                "Time(ms),Raw,Filtered,RR(BPM)"
            );

            logFile.close();
        }
    }
}


/* =========================================================
 * SAVE SAMPLE TO SD
 * ========================================================= */

void saveSample(unsigned long timeStamp,
                long rawValue,
                float filtered,
                float rr)
{
    logFile = SD.open(FILE_NAME, FILE_WRITE);

    if (!logFile)
    {
        Serial.println("ERR:SD_WRITE");
        return;
    }

    logFile.print(timeStamp);
    logFile.print(",");

    logFile.print(rawValue);
    logFile.print(",");

    logFile.print(filtered, 2);
    logFile.print(",");

    logFile.println(rr, 1);

    logFile.close();
}


/* =========================================================
 * PROCESS NEW RR
 * ========================================================= */

void processNewRR(float calculatedRR)
{
    newRR = calculatedRR;

    /*
     * First valid RR:
     *
     * There is no old RR yet.
     */

    if (!hasOldRR)
    {
        oldRR = newRR;
        hasOldRR = true;

        Serial.print("FIRST_RR:");
        Serial.println(newRR, 1);

        /*
         * No comparison on first measurement.
         *
         * Save it as the baseline.
         */

        return;
    }


    /*
     * Calculate difference
     */

    difference = newRR - oldRR;


    /*
     * Determine trend
     */

    const char* trend =
        calculateTrend(oldRR, newRR);


    /* ---------------------------------------------
     * Debug output
     * --------------------------------------------- */

    Serial.print("OLD_RR: ");
    Serial.print(oldRR, 1);

    Serial.print(" | NEW_RR: ");
    Serial.print(newRR, 1);

    Serial.print(" | DIFF: ");
    Serial.print(difference, 1);

    Serial.print(" | TREND: ");
    Serial.println(trend);


    /* ---------------------------------------------
     * Send packet to ATmega640
     * --------------------------------------------- */

    sendRRPacket(
        oldRR,
        newRR,
        difference,
        trend
    );


    /*
     * VERY IMPORTANT:
     *
     * The new RR becomes the old RR
     * for the next valid respiratory measurement.
     */

    oldRR = newRR;
}


/* =========================================================
 * SETUP
 * ========================================================= */

void setup()
{
    /*
     * UART
     *
     * ATmega328P PD1 TX -> ATmega640 UART3 RX
     */

    Serial.begin(115200);


    /* ---------------------------------------------
     * HX711
     * --------------------------------------------- */

    scale.begin(
        HX711_DOUT_PIN,
        HX711_SCK_PIN
    );


    if (scale.is_ready())
    {
        Serial.println("HX711:READY");
    }
    else
    {
        Serial.println("ERR:HX711_NOT_READY");
    }


    /* ---------------------------------------------
     * SD CARD
     * --------------------------------------------- */

    Serial.print("SD:INIT...");

    if (!SD.begin(SD_CS_PIN))
    {
        Serial.println("FAIL");
    }
    else
    {
        Serial.println("OK");


        /*
         * Create file if necessary
         */

        initializeSDFile();


        /*
         * Get previous RR
         */

        if (readOldRRFromSD())
        {
            hasOldRR = true;

            Serial.print("OLD_RR_FROM_SD:");
            Serial.println(oldRR, 1);
        }
        else
        {
            hasOldRR = false;

            Serial.println("NO_OLD_RR");
        }
    }


    /*
     * Initialize filter variables
     */

    firstRead = true;

    previousFiltered = 0.0f;

    isRising = false;

    lastPeakTime = 0;
}


/* =========================================================
 * MAIN LOOP
 * ========================================================= */

void loop()
{
    if (!scale.is_ready())
    {
        delay(20);
        return;
    }


    /* ---------------------------------------------
     * Read HX711
     * --------------------------------------------- */

    long rawValue = scale.read();


    /* ---------------------------------------------
     * IIR filtering
     * --------------------------------------------- */

    float currentFiltered =
        applyIIRFilter(rawValue);


    unsigned long currentTime =
        millis();


    /* ---------------------------------------------
     * PEAK DETECTION
     * --------------------------------------------- */

    if (currentFiltered > previousFiltered)
    {
        isRising = true;
    }
    else
    {
        if (isRising &&
            currentFiltered < previousFiltered)
        {
            /*
             * Falling edge after rising edge =
             * possible respiratory peak.
             */

            isRising = false;


            /*
             * Check threshold
             */

            if (previousFiltered > threshold)
            {
                /*
                 * Check minimum interval
                 */

                if ((lastPeakTime == 0) ||
                    ((currentTime - lastPeakTime)
                    >= MIN_PEAK_INTERVAL))
                {
                    /*
                     * Do not calculate RR on first peak.
                     */

                    if (lastPeakTime != 0)
                    {
                        unsigned long
                            timeBetweenPeaks =
                            currentTime - lastPeakTime;


                        /*
                         * Calculate RR:
                         *
                         * RR = 60000 / interval(ms)
                         */

                        respiratoryRate =
                            60000.0f /
                            (float)timeBetweenPeaks;


                        /*
                         * Process Old/New/Difference
                         */

                        processNewRR(
                            respiratoryRate
                        );
                    }


                    lastPeakTime = currentTime;
                }
            }
        }
    }


    previousFiltered = currentFiltered;


    /* ---------------------------------------------
     * Save waveform sample
     * --------------------------------------------- */

    saveSample(
        currentTime,
        rawValue,
        currentFiltered,
        respiratoryRate
    );


    delay(100);
}