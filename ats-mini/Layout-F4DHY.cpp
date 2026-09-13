#include "Common.h"
#include "Themes.h"
#include "Utils.h"
#include "Menu.h"
#include "Draw.h"
#include <string.h>

static void drawF4DHYScale(uint32_t freq)
{
  uint32_t centerFreq = freq;
  // Scale pointer
  spr.fillTriangle(156, 120, 160, 130, 164, 120, TH.scale_pointer);
  spr.drawLine(160, 130, 160, 169, TH.scale_pointer);

  spr.setTextDatum(MC_DATUM);
  spr.setTextColor(TH.scale_text);

  // Extra frequencies to draw outside the screen boundaries
  // (ensures frequency numbers don't disappear at the edges)
  int16_t slack = 3;

  // Scale offset
  int16_t offset = ((freq % 10) / 10.0 + slack) * 8;

  // Start drawing frequencies from the left
  freq = freq / 10 - 20 - slack;

  // Get band edges
  const Band *band = getCurrentBand();
  uint32_t minFreq = band->minimumFreq / 10;
  uint32_t maxFreq = band->maximumFreq / 10;

  for(int i=0 ; i<(slack + 41 + slack) ; i++, freq++)
  {
    int16_t x = i * 8 - offset;
    uint16_t tickFreq = freq * 10;
    uint16_t lineColor = TH.scale_line;

bool useBandColors =
  strcmp(TH.name, "eInk") != 0 &&
  strcmp(TH.name, "Pager") != 0 &&
  strcmp(TH.name, "Orange") != 0 &&
  strcmp(TH.name, "Phosphor") != 0 &&
  strcmp(TH.name, "Night") != 0 &&
  strcmp(TH.name, "Magenta") != 0;

BandUsage usage = BAND_NONE;

if(currentMode != FM)
  usage = getBandUsage(tickFreq);

if(useBandColors)
{
  switch(usage)
  {
    case BAND_CW:       lineColor = 0xFFE0; break;
    case BAND_DIGI:     lineColor = 0x07FF; break;
    case BAND_VOICE:    lineColor = 0x07E0; break;
    case BAND_ALL_MODE: lineColor = 0x03E0; break;
    case BAND_BEACON:   lineColor = 0xFD20; break;
    case BAND_SAT:      lineColor = 0xF81F; break;
    case BAND_CB_DX:    lineColor = 0x001F; break;
    default:            break;
  }
}

    if(freq >= minFreq && freq <= maxFreq)
    {
      if((i==20) && (!offset || (!(freq%5) && offset==1)))
      lineColor = TH.scale_pointer;

      if((freq % 10) == 0)
      {
        spr.drawLine(x, 169, x, 150, lineColor);
        spr.drawLine(x + 1, 169, x + 1, 150, lineColor);

        if(currentMode == FM)
          spr.drawFloat(freq / 10.0, 1, x, 140, 2);
        else if(freq >= 100)
          spr.drawFloat(freq / 100.0, 3, x, 140, 2);
        else
          spr.drawNumber(freq * 10, x, 140, 2);
      }
      else if((freq % 5) == 0 && (freq % 10) != 0)
      {
        spr.drawLine(x, 169, x, 155, lineColor);
        spr.drawLine(x + 1, 169, x + 1, 155, lineColor);
      }
      else
      {
        spr.drawLine(x, 169, x, 160, lineColor);
      }
    }
  }
  // Draw remarkable frequency markers

  
 if(currentMode != FM)
 {
   for(uint8_t i = 0; i < getNamedFrequencyCount(); i++)
   {
     uint16_t markerFreq = getNamedFrequencyFreq(i);
 
     int32_t deltaFreq = (int32_t)markerFreq - (int32_t)centerFreq;
     int16_t markerX = 160 + (deltaFreq * 8) / 10;

     if(markerX >= 0 && markerX <= 319)
     {
       spr.fillTriangle(
         markerX - 3, 152,
         markerX + 3, 152,
         markerX,     156,
         TH.scale_text
       );
     }
   }
 }
}

void drawLayoutF4DHY(const char *statusLine1, const char *statusLine2)
{
  // Draw preferences write request icon
  drawSaveIndicator(SAVE_OFFSET_X, SAVE_OFFSET_Y);

  // Draw BLE icon
  drawBleIndicator(BLE_OFFSET_X, BLE_OFFSET_Y);

  // Draw battery indicator & voltage
  bool has_voltage = drawBattery(BATT_OFFSET_X, BATT_OFFSET_Y);

  // Draw WiFi icon
  drawWiFiIndicator(has_voltage ? WIFI_OFFSET_X : BATT_OFFSET_X - 13, WIFI_OFFSET_Y);

  // Set font we are going to use
  spr.setFreeFont(&Orbitron_Light_24);

  // Draw band and mode
  drawBandAndMode(
    getCurrentBand()->bandName,
    bandModeDesc[currentMode],
    BAND_OFFSET_X, BAND_OFFSET_Y
  );

  if(switchThemeEditor())
  {
    spr.setTextDatum(TR_DATUM);
    spr.setTextColor(TH.text_warn);
    spr.drawString(TH.name, 319, BATT_OFFSET_Y + 17, 2);
  }

  // Draw frequency, units, and optionally highlight a digit
  drawFrequency(
    currentFrequency,
    FREQ_OFFSET_X, FREQ_OFFSET_Y,
    FUNIT_OFFSET_X, FUNIT_OFFSET_Y,
    currentCmd == CMD_FREQ ? getFreqInputPos() + (pushAndRotate ? 0x80 : 0) : 100
  );

  // Show station or channel name, if present
  if(*getStationName() == 0xFF)
    drawLongStationName(getStationName() + 1, MENU_OFFSET_X + 1 + 76 + MENU_DELTA_X + 2, RDS_OFFSET_Y);
  else if(*getStationName())
    drawStationName(getStationName(), RDS_OFFSET_X, RDS_OFFSET_Y);

  // Draw left-side menu/info bar
  // @@@ FIXME: Frequency display (above) intersects the side bar!
  drawSideBar(currentCmd, MENU_OFFSET_X, MENU_OFFSET_Y, MENU_DELTA_X);

  // Draw S-meter
  drawSMeter(getStrength(rssi), METER_OFFSET_X, METER_OFFSET_Y);

  // Indicate FM pilot detection (stereo indicator)
  drawStereoIndicator(METER_OFFSET_X, METER_OFFSET_Y, (currentMode==FM) && rx.getCurrentPilot());

  if(currentCmd == CMD_SCAN)
  {
    drawScanGraphs(isSSB()? (currentFrequency + currentBFO/1000) : currentFrequency);
  }
  else if(!drawWiFiStatus(statusLine1, statusLine2, STATUS_OFFSET_X, STATUS_OFFSET_Y))
  {
    // Show radio text if present, else show frequency scale
    if(*getRadioText() || *getProgramInfo())
      drawRadioText(STATUS_OFFSET_Y, STATUS_OFFSET_Y + 25);
    else
      drawF4DHYScale(isSSB()? (currentFrequency + currentBFO/1000) : currentFrequency);
  }
}
