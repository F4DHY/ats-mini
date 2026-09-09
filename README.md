# ATS Mini - F4DHY Mod

Personal modification of the ATS Mini firmware, based on the official
[esp32-si4732/ats-mini](https://github.com/esp32-si4732/ats-mini) project.

## Battery gauge modification

This version improves the battery indicator behavior on the ATS Mini.

### Changes

- Continuous battery gauge instead of the original steps
- Linear indication between 3.30 V and 3.95 V
- 3.95 V considered as a full battery
- The last two pixels of the gauge are displayed in the low-battery color
- Warning zone hysteresis: enters at 3.368 V and clears above 3.385 V
- Low battery warning at 3.30 V
- 30 mV hysteresis to prevent the LOW state from oscillating
- Empty battery icon flashes every 500 ms when battery is low
- Battery voltage remains displayed in red during the LOW warning
- Original USB charging indication is preserved

The modification was developed and tested on an ATS Mini V4 using the
ESP32-S3 OSPI build.

A full discharge test showed approximately 8 hours of runtime, with no
noticeable battery-life degradation compared with the original firmware.


## Firmware

A precompiled OSPI firmware is available in the Releases section.

> This is an unofficial modification and is not affiliated with or supported
> by the original ATS Mini project.

---


# ATS Mini

![](docs/source/_static/esp32-si4732-ui-theme.jpg)

This firmware is for use on the SI4732 (ESP32-S3) Mini/Pocket Receiver

Based on the following sources:

* Volos Projects:    https://github.com/VolosR/TEmbedFMRadio
* PU2CLR, Ricardo:   https://github.com/pu2clr/SI4735
* Ralph Xavier:      https://github.com/ralphxavier/SI4735
* Goshante:          https://github.com/goshante/ats20_ats_ex
* G8PTN, Dave:       https://github.com/G8PTN/ATS_MINI

## Releases

Check out the [Releases](https://github.com/esp32-si4732/ats-mini/releases) page.

## Documentation

The hardware, software and flashing documentation is available at <https://esp32-si4732.github.io/ats-mini/>

## Discuss

* [GitHub Discussions](https://github.com/esp32-si4732/ats-mini/discussions) - the best place for feature requests, observations, sharing, etc.
* [TalkRadio Telegram Chat](https://t.me/talkradio/174172) - informal space to chat in Russian and English.
