#include "Common.h"
#include "Themes.h"
#include "Utils.h"

#define VBAT_MON  4                 // GPIO04 -- Battery Monitor PIN

#define BATT_ADC_READS          10  // ADC reads for average calculation
#define BATT_ADC_FACTOR      1.702  // ADC correction factor used for the battery monitor

// Custom battery gauge
#define BATT_FULL_VOLTAGE    3.950  // Gauge considered full
#define BATT_LOW_VOLTAGE     3.300  // Enter LOW state
#define BATT_LOW_RELEASE     3.330  // Leave LOW state (hysteresis)
#define BATT_BAR_WIDTH          24  // Maximum internal bar width in pixels
#define BATT_BLINK_MS          500  // Blink period
#define BATT_WARN_VOLTAGE 3.368
#define BATT_WARN_RELEASE 3.385

// Current battery voltage
static float batteryVolts = 4.0;

// LOW state with hysteresis
static bool batteryLow = false;

// hysteresis for Red progress bar pixels
static bool batteryWarn = false;

//
// Measure and return battery voltage
//
float batteryMonitor()
{
  int i, j;

  // Read ADC multiple times
  for(i = j = 0 ; i < BATT_ADC_READS ; i++)
    j += analogRead(VBAT_MON);

  // Calculate average voltage with correction factor
  batteryVolts = ((float)j / BATT_ADC_READS) * BATT_ADC_FACTOR / 1000;

  return batteryVolts;
}

//
// Show last measured battery voltage and status at given screen
// coordinates. Return true if voltage was drawn.
//
bool drawBattery(int x, int y)
{
  if(sleepOn()) return false;

  // Measure battery voltage
  batteryMonitor();

  spr.setTextDatum(TR_DATUM);
  spr.setTextColor(TH.batt_voltage);

  //
  // Theme editor test mode
  //
  if(switchThemeEditor())
  {
    // Alternate between representative states every 10 seconds
    uint8_t testState = (millis() % 50000u) / 10000u;

    switch(testState)
    {
      case 0:
        batteryVolts = 4.50;  // Charging
        break;

      case 1:
        batteryVolts = 3.95;  // Full
        break;

      case 2:
        batteryVolts = 3.70;  // Medium
        break;

      case 3:
        batteryVolts = 3.34;  // Nearly empty
        break;

      case 4:
      default:
        batteryVolts = 3.20;  // LOW
        break;
    }
  }

  //
  // LOW state hysteresis
  //
  // Enter LOW at or below 3.30V.
  // Once LOW, stay LOW until voltage reaches 3.33V.
  //
  if(batteryLow)
  {
    if(batteryVolts >= BATT_LOW_RELEASE)
      batteryLow = false;
  }
  else
  {
    if(batteryVolts <= BATT_LOW_VOLTAGE)
      batteryLow = true;
  }

if(batteryWarn) {
  if(batteryVolts >= BATT_WARN_RELEASE) batteryWarn = false;
} else {
  if(batteryVolts <= BATT_WARN_VOLTAGE) batteryWarn = true;
}

  //
  // USB / charging display
  //
  // The hardware has a load sharing circuit to allow simultaneous charge and power.
  // With USB(5V) connected the voltage reading will be approx.
  // VBUS - Diode Drop = 4.65V.
  //
  if(batteryVolts > 4.3)
  {
    // Normal battery outline
    spr.drawRoundRect(x, y + 1, 28, 14, 3, TH.batt_border);
    spr.drawLine(x + 29, y + 5, x + 29, y + 10, TH.batt_border);
    spr.drawLine(x + 30, y + 6, x + 30, y + 9, TH.batt_border);

    // Charging fill
    spr.fillRoundRect(x + 2, y + 3, 24, 10, 2, TH.batt_charge);

    // Lightning icon
    spr.drawLine(x + 9 + 8, y + 1, x + 9 + 6, y + 1 + 5, TH.bg);
    spr.drawLine(x + 9 + 6, y + 1 + 5, x + 9 + 10, y + 1 + 5, TH.bg);
    spr.drawLine(x + 9 + 11, y + 1 + 6, x + 9 + 4, y + 1 + 13, TH.bg);
    spr.drawLine(x + 9 + 2, y + 1 + 13, x + 9 + 4, y + 1 + 8, TH.bg);
    spr.drawLine(x + 9 + 4, y + 1 + 8, x + 9 + 0, y + 1 + 8, TH.bg);
    spr.drawLine(x + 9 - 1, y + 1 + 7, x + 9 + 6, y + 1 + 0, TH.bg);

    spr.fillTriangle(
      x + 9 + 7, y + 1,
      x + 9 + 4, y + 1 + 6,
      x + 9,     y + 1 + 7,
      TH.batt_icon
    );

    spr.fillTriangle(
      x + 9 + 5,  y + 1 + 6,
      x + 9 + 10, y + 1 + 6,
      x + 9 + 3,  y + 1 + 13,
      TH.batt_icon
    );

    spr.fillRect(x + 9 + 1, y + 1 + 6, 9, 2, TH.batt_icon);
    spr.drawPixel(x + 9 + 3, y + 1 + 12, TH.batt_icon);

    return false;
  }

  //
  // Voltage text
  //
  char voltage[8];
  sprintf(voltage, "%.02fV", batteryVolts);

  //
  // LOW BATTERY
  //
  if(batteryLow)
  {
    // Blink complete empty red battery symbol
    bool blinkOn = ((millis() / BATT_BLINK_MS) % 2) == 0;

    if(blinkOn)
    {
      spr.drawRoundRect(x, y + 1, 28, 14, 3, TH.batt_low);
      spr.drawLine(x + 29, y + 5, x + 29, y + 10, TH.batt_low);
      spr.drawLine(x + 30, y + 6, x + 30, y + 9, TH.batt_low);
    }
    else
    {
      // Erase battery symbol during blink-off phase
      spr.fillRect(x, y, 31, 16, TH.bg);
    }

    // Voltage remains visible and turns red
    spr.setTextDatum(TR_DATUM);
    spr.setTextColor(TH.batt_low);
    spr.drawString(voltage, x - 3, y, 2);

    return true;
  }

  //
  // NORMAL GAUGE
  // 3.30V -> 3.95V mapped linearly to 0 -> 24 pixels
  //
  int level;

  if(batteryVolts >= BATT_FULL_VOLTAGE)
  {
    level = BATT_BAR_WIDTH;
  }
  else
  {
    level = round(
      (batteryVolts - BATT_LOW_VOLTAGE) *
      BATT_BAR_WIDTH /
      (BATT_FULL_VOLTAGE - BATT_LOW_VOLTAGE)
    );

    // Safety bounds
    if(level < 0)
      level = 0;

    if(level > BATT_BAR_WIDTH)
      level = BATT_BAR_WIDTH;

  }

// Keep warning display stable around the 2/3 pixel threshold
if(batteryWarn && level > 2)
  level = 2;

  //
  // Battery outline
  //
  spr.drawRoundRect(x, y + 1, 28, 14, 3, TH.batt_border);
  spr.drawLine(x + 29, y + 5, x + 29, y + 10, TH.batt_border);
  spr.drawLine(x + 30, y + 6, x + 30, y + 9, TH.batt_border);

  //
  // Battery fill
  //
  if(level > 0)
  {
    uint16_t barColor;

    // Last two pixels are shown in the theme's low-battery color
    if(level <= 2)
      barColor = TH.batt_low;
    else
      barColor = TH.batt_full;

    spr.fillRoundRect(
      x + 2,
      y + 3,
      level,
      10,
      2,
      barColor
    );
  }

  //
  // Voltage
  //
  spr.setTextDatum(TR_DATUM);
  spr.setTextColor(TH.batt_voltage);
  spr.drawString(voltage, x - 3, y, 2);

  return true;
}

bool batteryLowActive()
{
  return batteryLow;
}