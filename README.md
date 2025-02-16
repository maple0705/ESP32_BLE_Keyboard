# ESP32 BLE Keyboard library

This source code allows you to make the ESP32 act as a Bluetooth Keyboard and control what it does.

<blockquote class="twitter-tweet"><p lang="ja" dir="ltr">した <a href="https://t.co/2cPE8O7Y7g">pic.twitter.com/2cPE8O7Y7g</a></p>&mdash; maple (@maple0705) <a href="https://twitter.com/maple0705/status/1891083527359201389?ref_src=twsrc%5Etfw">February 16, 2025</a></blockquote> <script async src="https://platform.twitter.com/widgets.js" charset="utf-8"></script>

## Original Repository:
- [ESP32-BLE-Keyboard](https://github.com/T-vK/ESP32-BLE-Keyboard)

## Changes from Original Repository
- add files BleKeyboard_Raw.h and BleKeyboard_Raw.cpp (files without '_Raw' are no longer used)
- add functions pressRaw() and releaseRaw() to send UsageID of HID DIRECTLY (press() and release() are no longer used)

## Hardware
- [かんたんUSBホスト](https://q61.org/blog/2021/06/09/easyusbhost/)  
  Only 'イベントモード' is  available in this code.
- [M5StickC](https://docs.m5stack.com/en/core/m5stickc)  
  Pin '5V' and 'GND' are used to supply voltage to かんたんUSBホスト.  
  Pin 'G36'(TX) is connected to RX of かんたんUSBホスト.
- [YMDK Air40](https://ymdkey.com/products/air40-rgb-hot-swap-cute-40-mechanical-keyboard-assembled-qmk-via-type-c-pcb-cnc-case-plate-kit)  
  USB-Keyboard to be wireless via BLE
