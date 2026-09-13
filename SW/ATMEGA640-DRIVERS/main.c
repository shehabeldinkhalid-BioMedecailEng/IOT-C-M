#include "DataTypes.h"
#include "ATmega640_REGS.h"
#include "DRIVERS/SPI/SPI.h"
#include "DRIVERS/UART/UART.h"
#include "DRIVERS/EEPROM/EEPROM.h"
#include "DRIVERS/I2C/I2C.h"
#include "HAL/AFE4490-INTERFACE/AFE4490.h"
#include "HAL/OLED/OLED.h"
#include "HAL/CONTROL/Control.h"

/* =========================
 * Project wiring
 * =========================
 * SPI0: AFE4490
 * UART1: sensor board 1
 * UART2: respiratory/MMR920 board
 * UART0: Wi-Fi module
 *
 * PB0..PB3 are already owned by SPI.c.
 * Control uses the remaining PB4..PB6 here.
 */
#define SENSOR1_UART       UART1
#define RESP_UART          UART2
#define WIFI_UART          UART0

#define GREEN_LED_PIN      PB4
#define RED_LED_PIN        PB5
#define BUZZER_PIN         PB6

#define PACKET_MAX         96
#define SENSOR1_TIMEOUT_SAMPLES 1500U
#define RESP_TIMEOUT_SAMPLES   1500U
#define EEPROM_MAGIC0      0x49
#define EEPROM_MAGIC1      0x4F
#define EEPROM_BASE        0x0000U
#define EEPROM_SENSOR1     0x0004U
#define EEPROM_RESP_OLD    0x0044U
#define EEPROM_RESP_NEW    0x0046U
#define EEPROM_RESP_DIFF   0x0048U
#define EEPROM_RESP_STATUS 0x004AU
#define EEPROM_HR          0x004BU
#define EEPROM_SPO2_X10    0x004DU
#define EEPROM_RECORD_SEQ  0x004FU

#define SPO2_MIN_X10       700U
#define SPO2_MAX_X10       1000U
#define HR_MIN_BPM         30U
#define HR_MAX_BPM         220U

/* The common 110 - 25R approximation is only a project prototype
 * calibration. Replace these coefficients with calibration derived from
 * the actual RED/IR optical path before clinical use. */
#define SPO2_A_X10         1100L
#define SPO2_B_X10         250L

typedef struct
{
    DT_uint16_t rr_old_x10;
    DT_uint16_t rr_new_x10;
    DT_int16_t  rr_diff_x10;
    DT_uint8_t  rr_status;
    DT_uint8_t  resp_valid;
} RespState_t;

typedef struct
{
    DT_uint16_t hr;
    DT_uint16_t spo2_x10;
    DT_uint8_t  valid;
} VitalState_t;

typedef struct
{
    char buffer[PACKET_MAX];
    DT_uint8_t index;
} PacketParser_t;

static RespState_t g_resp;
static VitalState_t g_vitals;
static PacketParser_t g_sensor1_parser;
static PacketParser_t g_resp_parser;
static DT_uint8_t g_sensor1_valid = DT_FALSE;
static DT_uint16_t g_sensor1_age = 0;
static DT_uint16_t g_resp_age = 0;
static DT_uint16_t g_record_sequence = 0;
static DT_uint8_t g_record_dirty = DT_FALSE;
static char g_sensor1_packet[61];
static DT_uint32_t g_sample_ticks = 0;
static DT_uint32_t g_total_samples = 0;

static DT_int32_t abs32(DT_int32_t x)
{
    return (x < 0) ? -x : x;
}

static DT_uint16_t parse_u16_x10(const char* s)
{
    DT_uint32_t whole = 0;
    DT_uint32_t frac = 0;
    DT_uint8_t have_frac = 0;
    while (*s >= '0' && *s <= '9')
    {
        whole = whole * 10UL + (DT_uint32_t)(*s - '0');
        s++;
    }
    if (*s == '.')
    {
        s++;
        if (*s >= '0' && *s <= '9')
        {
            frac = (DT_uint32_t)(*s - '0');
            have_frac = 1;
        }
    }
    return (DT_uint16_t)(whole * 10UL + (have_frac ? frac : 0UL));
}

static DT_int16_t parse_i16_x10(const char* s)
{
    DT_uint8_t neg = DT_FALSE;
    DT_uint16_t v;
    if (*s == '-')
    {
        neg = DT_TRUE;
        s++;
    }
    v = parse_u16_x10(s);
    return neg ? -(DT_int16_t)v : (DT_int16_t)v;
}

static char* next_field(char** p)
{
    char* start = *p;
    char* q = start;
    while (*q != '\0' && *q != ',')
        q++;
    if (*q == ',')
    {
        *q = '\0';
        *p = q + 1;
    }
    else
    {
        *p = q;
    }
    return start;
}

static void copy_sensor1_packet(const char* text)
{
    DT_uint8_t i;
    for (i = 0; i < 60U && text[i] != '\0'; i++)
        g_sensor1_packet[i] = text[i];
    g_sensor1_packet[i] = '\0';
}

static void process_sensor1_packet(char* packet)
{
    /* Preserve the complete packet. The first sensor-board flow is not
     * present in the ATmega640 ZIP, so no unsupported field meaning is invented. */
    copy_sensor1_packet(packet);
    g_sensor1_valid = DT_TRUE;
    g_sensor1_age = 0;
    g_record_dirty = DT_TRUE;
}

static void process_resp_packet(char* packet)
{
    char* p;
    char* f;

    if (packet[0] != '<')
        return;

    packet[0] = 'R';
    if (packet[1] != 'S')
        return;

    p = packet + 3;
    f = next_field(&p);
    if (*f == '>') return;
    g_resp.rr_old_x10 = parse_u16_x10(f);

    f = next_field(&p);
    g_resp.rr_new_x10 = parse_u16_x10(f);

    f = next_field(&p);
    g_resp.rr_diff_x10 = parse_i16_x10(f);

    f = next_field(&p);
    g_resp.rr_status = (DT_uint8_t)f[0];
    g_resp.resp_valid = DT_TRUE;
    g_resp_age = 0;
    g_record_dirty = DT_TRUE;
}

static void parser_feed(PacketParser_t* parser, DT_uint8_t byte, DT_uint8_t source)
{
    if (byte == '<')
    {
        parser->index = 0;
        parser->buffer[parser->index++] = '<';
        return;
    }

    if (parser->index == 0)
        return;

    if (byte == '>')
    {
        if (parser->index < PACKET_MAX - 1U)
            parser->buffer[parser->index++] = '>';
        parser->buffer[parser->index] = '\0';

        if (source == 1U)
            process_sensor1_packet(parser->buffer);
        else
            process_resp_packet(parser->buffer);

        parser->index = 0;
        return;
    }

    if (parser->index < PACKET_MAX - 1U)
        parser->buffer[parser->index++] = (char)byte;
    else
        parser->index = 0;
}

static void poll_uart_packets(void)
{
    while (UART_IsDataAvailable(SENSOR1_UART))
        parser_feed(&g_sensor1_parser, UART_ReceiveByte(SENSOR1_UART), 1U);

    while (UART_IsDataAvailable(RESP_UART))
        parser_feed(&g_resp_parser, UART_ReceiveByte(RESP_UART), 2U);
}

static DT_uint16_t calculate_hr_from_ir(DT_int32_t ir_ac)
{
    static DT_int32_t filtered = 0;
    static DT_int32_t previous = 0;
    static DT_uint16_t samples_since_peak = 0;
    static DT_uint8_t rising = DT_FALSE;
    static DT_uint8_t have_peak = DT_FALSE;
    static DT_uint16_t last_interval = 0;
    static DT_uint16_t result = 0;
    static DT_uint16_t peak_holdoff = 0;

    filtered += (ir_ac - filtered) / 8;
    samples_since_peak++;
    if (peak_holdoff > 0)
        peak_holdoff--;

    if (filtered > previous)
        rising = DT_TRUE;
    else if (rising && filtered < previous)
    {
        rising = DT_FALSE;
        if (peak_holdoff == 0 && abs32(filtered) > 1000)
        {
            if (have_peak && samples_since_peak >= 30U && samples_since_peak <= 200U)
            {
                last_interval = samples_since_peak;
                result = (DT_uint16_t)(60U * AFE4490_SAMPLE_RATE_HZ / last_interval);
                if (result >= HR_MIN_BPM && result <= HR_MAX_BPM)
                    g_vitals.hr = result;
            }
            have_peak = DT_TRUE;
            samples_since_peak = 0;
            peak_holdoff = 25U;
        }
    }
    previous = filtered;
    return result;
}

static DT_uint16_t calculate_spo2(AFE4490_Sample_t* s)
{
    static DT_int32_t ir_min = 2147483647L;
    static DT_int32_t ir_max = -2147483647L;
    static DT_int32_t red_min = 2147483647L;
    static DT_int32_t red_max = -2147483647L;
    static DT_uint16_t sample_count = 0;
    DT_int32_t ir_dc;
    DT_int32_t red_dc;
    DT_int32_t ir_ac;
    DT_int32_t red_ac;
    DT_int32_t r_x1000;
    DT_int32_t spo2_x10;

    if (s->ir_ambient < ir_min) ir_min = s->ir_ambient;
    if (s->ir_ambient > ir_max) ir_max = s->ir_ambient;
    if (s->red_ambient < red_min) red_min = s->red_ambient;
    if (s->red_ambient > red_max) red_max = s->red_ambient;

    sample_count++;
    if (sample_count < (AFE4490_SAMPLE_RATE_HZ * 3U))
        return g_vitals.spo2_x10;

    ir_dc = abs32(s->ir_ambient);
    red_dc = abs32(s->red_ambient);
    ir_ac = ir_max - ir_min;
    red_ac = red_max - red_min;

    if (ir_dc < 1000 || red_dc < 1000 || ir_ac < 10 || red_ac < 10)
    {
        sample_count = 0;
        ir_min = 2147483647L; ir_max = -2147483647L;
        red_min = 2147483647L; red_max = -2147483647L;
        return g_vitals.spo2_x10;
    }

    r_x1000 = (DT_int32_t)(((DT_int64_t)red_ac * ir_dc * 1000LL) /
                           ((DT_int64_t)red_dc * ir_ac));

    spo2_x10 = SPO2_A_X10 - ((SPO2_B_X10 * r_x1000) / 1000L);
    if (spo2_x10 < 0) spo2_x10 = 0;
    if (spo2_x10 > 1000) spo2_x10 = 1000;

    g_vitals.spo2_x10 = (DT_uint16_t)spo2_x10;

    sample_count = 0;
    ir_min = 2147483647L; ir_max = -2147483647L;
    red_min = 2147483647L; red_max = -2147483647L;
    return g_vitals.spo2_x10;
}

static void process_afe(void)
{
    AFE4490_Sample_t s;

    if (!AFE4490_DataReady())
        return;

    if (!AFE4490_ReadSample(&s))
        return;

    calculate_hr_from_ir(s.ir_ac);
    calculate_spo2(&s);
    if (g_sensor1_age < 65535U) g_sensor1_age++;
    if (g_resp_age < 65535U) g_resp_age++;
    g_sample_ticks++;
    g_total_samples++;

    if (g_vitals.hr >= HR_MIN_BPM && g_vitals.hr <= HR_MAX_BPM &&
        g_vitals.spo2_x10 >= SPO2_MIN_X10 && g_vitals.spo2_x10 <= SPO2_MAX_X10)
        g_vitals.valid = DT_TRUE;
    else
        g_vitals.valid = DT_FALSE;
}

static void eeprom_save_record(void)
{
    if (!g_record_dirty || !g_vitals.valid || !g_sensor1_valid || !g_resp.resp_valid)
        return;

    EEPROM_UpdateByte(EEPROM_BASE + 0, EEPROM_MAGIC0);
    EEPROM_UpdateByte(EEPROM_BASE + 1, EEPROM_MAGIC1);

    {
        DT_uint8_t i;
        for (i = 0; i < 60U; i++)
            EEPROM_UpdateByte(EEPROM_SENSOR1 + i, (DT_uint8_t)g_sensor1_packet[i]);
    }

    g_record_sequence++;
    EEPROM_UpdateByte(EEPROM_RECORD_SEQ + 0, (DT_uint8_t)g_record_sequence);
    EEPROM_UpdateByte(EEPROM_RECORD_SEQ + 1, (DT_uint8_t)(g_record_sequence >> 8));

    EEPROM_UpdateByte(EEPROM_RESP_OLD + 0, (DT_uint8_t)g_resp.rr_old_x10);
    EEPROM_UpdateByte(EEPROM_RESP_OLD + 1, (DT_uint8_t)(g_resp.rr_old_x10 >> 8));
    EEPROM_UpdateByte(EEPROM_RESP_NEW + 0, (DT_uint8_t)g_resp.rr_new_x10);
    EEPROM_UpdateByte(EEPROM_RESP_NEW + 1, (DT_uint8_t)(g_resp.rr_new_x10 >> 8));
    EEPROM_UpdateByte(EEPROM_RESP_DIFF + 0, (DT_uint8_t)g_resp.rr_diff_x10);
    EEPROM_UpdateByte(EEPROM_RESP_DIFF + 1, (DT_uint8_t)(g_resp.rr_diff_x10 >> 8));
    EEPROM_UpdateByte(EEPROM_RESP_STATUS, g_resp.rr_status);

    EEPROM_UpdateByte(EEPROM_HR + 0, (DT_uint8_t)g_vitals.hr);
    EEPROM_UpdateByte(EEPROM_HR + 1, (DT_uint8_t)(g_vitals.hr >> 8));
    EEPROM_UpdateByte(EEPROM_SPO2_X10 + 0, (DT_uint8_t)g_vitals.spo2_x10);
    EEPROM_UpdateByte(EEPROM_SPO2_X10 + 1, (DT_uint8_t)(g_vitals.spo2_x10 >> 8));

    g_record_dirty = DT_FALSE;
}

static void wifi_send_number(DT_uint32_t value)
{
    char digits[11];
    DT_uint8_t i = 0;
    DT_uint8_t j;

    if (value == 0)
    {
        UART_SendByte(WIFI_UART, '0');
        return;
    }

    while (value > 0 && i < sizeof(digits))
    {
        digits[i++] = (char)('0' + (value % 10UL));
        value /= 10UL;
    }
    for (j = i; j > 0; j--)
        UART_SendByte(WIFI_UART, (DT_uint8_t)digits[j - 1]);
}

static void wifi_publish(void)
{
    /* ESP-AT transport layer. The actual cloud URL/token must be supplied
     * for the deployed Wi-Fi module/cloud service. */
    UART_SendString(WIFI_UART, "<DATA,HR=");
    wifi_send_number(g_vitals.hr);
    UART_SendString(WIFI_UART, ",SPO2X10=");
    wifi_send_number(g_vitals.spo2_x10);
    UART_SendString(WIFI_UART, ",RRNEWX10=");
    wifi_send_number(g_resp.rr_new_x10);
    UART_SendString(WIFI_UART, ",RDIFFX10=");
    if (g_resp.rr_diff_x10 < 0) UART_SendByte(WIFI_UART, '-');
    wifi_send_number((DT_uint32_t)abs32(g_resp.rr_diff_x10));
    UART_SendString(WIFI_UART, ",STATUS=");
    UART_SendByte(WIFI_UART, g_resp.rr_status);
    UART_SendString(WIFI_UART, ">\r\n");
}

static void oled_write_u16(DT_uint16_t value)
{
    char digits[6];
    DT_uint8_t i = 0;
    if (value == 0) { OLED_WriteChar('0'); return; }
    while (value > 0)
    {
        digits[i++] = (char)('0' + (value % 10U));
        value /= 10U;
    }
    while (i > 0) OLED_WriteChar(digits[--i]);
}

static void oled_update(void)
{
    OLED_Clear();
    OLED_SetCursor(0, 0);
    OLED_WriteString("HR:");
    oled_write_u16(g_vitals.hr);
    OLED_WriteString(" BPM");

    OLED_SetCursor(1, 0);
    OLED_WriteString("SpO2:");
    oled_write_u16(g_vitals.spo2_x10 / 10U);
    OLED_WriteString(".");
    OLED_WriteChar((char)('0' + (g_vitals.spo2_x10 % 10U)));
    OLED_WriteString("%");

    OLED_SetCursor(2, 0);
    OLED_WriteString("RR:");
    oled_write_u16(g_resp.rr_new_x10 / 10U);
    OLED_WriteString("/min");

    OLED_SetCursor(3, 0);
    if (g_sensor1_valid) OLED_WriteString("S1 OK ");
    else OLED_WriteString("S1 -- ");
    if (g_resp.resp_valid) OLED_WriteString("RR OK");
    else OLED_WriteString("RR --");

    OLED_SetCursor(4, 0);
    if (g_vitals.valid) OLED_WriteString("AFE OK");
    else OLED_WriteString("AFE WAIT");
}

static void update_alerts(void)
{
    DT_uint8_t all_data = (g_vitals.valid && g_sensor1_valid && g_resp.resp_valid);
    DT_uint8_t recent = (g_sensor1_age < SENSOR1_TIMEOUT_SAMPLES &&
                         g_resp_age < RESP_TIMEOUT_SAMPLES);

    if (all_data && recent)
    {
        LED_On(GREEN_LED_PIN);
        LED_Off(RED_LED_PIN);
        Buzzer_Off(BUZZER_PIN);
    }
    else
    {
        LED_Off(GREEN_LED_PIN);
        LED_On(RED_LED_PIN);
        Buzzer_On(BUZZER_PIN);
    }
}

int main(void)
{

    SPI_InitMaster(SPI0, SPI_MODE0, SPI_DIV16);
    UART_Init(SENSOR1_UART, 115200UL);
    UART_Init(RESP_UART, 115200UL);
    UART_Init(WIFI_UART, 115200UL);
    I2C_Init(I2C0, 400000UL);

    Buzzer_Init(BUZZER_PIN);
    LED_Init(GREEN_LED_PIN);
    LED_Init(RED_LED_PIN);

    OLED_Init();
    OLED_Clear();
    OLED_SetCursor(0, 0);
    OLED_WriteString("SMART DEVICE");

    AFE4490_Init();

    while (1)
    {
        process_afe();
        poll_uart_packets();

        if (g_sample_ticks >= 100UL)
        {
            g_sample_ticks = 0;
            oled_update();
            update_alerts();
        }

        if ((g_vitals.valid && g_resp.resp_valid) &&
            (g_record_dirty == DT_TRUE) &&
            ((g_total_samples % 100U) == 0U))
        {
            eeprom_save_record();
        }

        if ((g_total_samples % 1000UL) == 0UL)
        {
            if (g_vitals.valid && g_resp.resp_valid)
                wifi_publish();
        }
    }

    return 0;
}
