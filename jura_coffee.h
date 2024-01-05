#include "esphome.h"

class JuraCoffee : public PollingComponent, public UARTDevice {
 Sensor *xsensor1 {nullptr};
 Sensor *xsensor2 {nullptr};
 Sensor *xsensor3 {nullptr};
 Sensor *xsensor4 {nullptr};
 Sensor *xsensor5 {nullptr};
 Sensor *xsensor6 {nullptr};
 Sensor *xsensor7 {nullptr};
 Sensor *xsensor8 {nullptr};
 Sensor *xsensor9 {nullptr};
 Sensor *xsensor10 {nullptr};
 Sensor *xsensor11 {nullptr};
 Sensor *xsensor12 {nullptr};
 Sensor *xsensor13 {nullptr};
 Sensor *xsensor14 {nullptr};
 Sensor *xsensor15 {nullptr};
 TextSensor *xsensor16 {nullptr};
 TextSensor *xsensor17 {nullptr};
 TextSensor *xsensor18 {nullptr};
 TextSensor *xsensor19 {nullptr};

 public:
  JuraCoffee(UARTComponent *parent, Sensor *sensor1, Sensor *sensor2, Sensor *sensor3, Sensor *sensor4, Sensor *sensor5, Sensor *sensor6, Sensor *sensor7, Sensor *sensor8, Sensor *sensor9, Sensor *sensor10, Sensor *sensor11, Sensor *sensor12, Sensor *sensor13, Sensor *sensor14, Sensor *sensor15, TextSensor *sensor16, TextSensor *sensor17, TextSensor *sensor18, TextSensor *sensor19) : UARTDevice(parent) , xsensor1(sensor1) , xsensor2(sensor2) , xsensor3(sensor3) , xsensor4(sensor4) , xsensor5(sensor5) , xsensor6(sensor6) , xsensor7(sensor7) , xsensor8(sensor8) , xsensor9(sensor9) , xsensor10(sensor10) , xsensor11(sensor11) , xsensor12(sensor12) , xsensor13(sensor13) , xsensor14(sensor14) , xsensor15(sensor15) , xsensor16(sensor16) , xsensor17(sensor17) , xsensor18(sensor18), xsensor19(sensor19) {}

  long num_single_espresso, num_double_espresso, num_coffee, num_double_coffee, num_grinded_coffee, num_ristretto, num_double_restretti, num_cappuccino, num_latte, num_milk, num_hot_water, num_filter, num_milk_clean, num_decalcify, num_clean;
  std::string tray_status, tank_status, num_all, status_all;

  // Jura communication function taken in entirety from cmd2jura.ino, found at https://github.com/hn/jura-coffee-machine
  String cmd2jura(String outbytes) {
    String inbytes;
    int w = 0;

    while (available()) {
      read();
    }

    outbytes += "\r\n";
    for (int i = 0; i < outbytes.length(); i++) {
      for (int s = 0; s < 8; s += 2) {
        char rawbyte = 255;
        bitWrite(rawbyte, 2, bitRead(outbytes.charAt(i), s + 0));
        bitWrite(rawbyte, 5, bitRead(outbytes.charAt(i), s + 1));
        write(rawbyte);
      }
      delay(8);
    }

    int s = 0;
    char inbyte;
    while (!inbytes.endsWith("\r\n")) {
      if (available()) {
        byte rawbyte = read();
        bitWrite(inbyte, s + 0, bitRead(rawbyte, 2));
        bitWrite(inbyte, s + 1, bitRead(rawbyte, 5));
        if ((s += 2) >= 8) {
          s = 0;
          inbytes += inbyte;
        }
      } else {
        delay(10);
      }
      if (w++ > 500) {
        return "";
      }
    }

    return inbytes.substring(0, inbytes.length() - 2);
  }

  void setup() override {
    this->set_update_interval(60000); // 600000 = 10 minutes // Now 60 seconds
  }

  void loop() override {
  }

  void update() override {
    String result, hexString, substring;
    byte hex_to_byte;
    int trayBit, tankBit;
    // For Testing
    // int read_bit0,read_bit1,read_bit2,read_bit3,read_bit4,read_bit5,read_bit6,read_bit7;

    // Fetch our line of EEPROM
    result = cmd2jura("RT:0000");
    num_all = result.c_str();

    // Get Single Espressos made
    substring = result.substring(3,7);
    num_single_espresso = strtol(substring.c_str(),NULL,16);

    // Ristretto made
    substring = result.substring(7,11);
    num_ristretto = strtol(substring.c_str(),NULL,16);

    // Coffees made
    substring = result.substring(11,15);
    num_coffee = strtol(substring.c_str(),NULL,16);

    // Double Coffees made
    substring = result.substring(15,19);
    num_double_coffee = strtol(substring.c_str(),NULL,16);

    // Cappuccino made
    substring = result.substring(19,23);
    num_cappuccino = strtol(substring.c_str(),NULL,16);

    // Latte made
    substring = result.substring(23,27);
    num_latte = strtol(substring.c_str(),NULL,16);

    // grinded Coffe made
    substring = result.substring(27,31);
    num_grinded_coffee = strtol(substring.c_str(),NULL,16);

    // Cleanings done
    substring = result.substring(35,39);
    num_clean = strtol(substring.c_str(),NULL,16);

    // Decalcify done
    substring = result.substring(39,43);
    num_decalcify = strtol(substring.c_str(),NULL,16);

    // Double Ristretti made
    substring = result.substring(51,55);
    num_double_restretti = strtol(substring.c_str(),NULL,16);

    // Double Espressos made
    substring = result.substring(59,63);
    num_double_espresso = strtol(substring.c_str(),NULL,16);

    // Tray & water tank status
    // Much gratitude to https://www.instructables.com/id/IoT-Enabled-Coffee-Machine/ for figuring out how these bits are stored
    result = cmd2jura("IC:");
    status_all = result.c_str();
    hexString = result.substring(3,5);
    hex_to_byte = strtol(hexString.c_str(),NULL,16);
    trayBit = bitRead(hex_to_byte, 4);
    tankBit = bitRead(hex_to_byte, 5);
    if (trayBit == 1) { tray_status = "Missing"; } else { tray_status = "Present"; }
    if (tankBit == 1) { tank_status = "Fill Tank"; } else { tank_status = "OK"; }

    // For Testing
    // read_bit0 = bitRead(hex_to_byte, 0);
    // read_bit1 = bitRead(hex_to_byte, 1);
    // read_bit2 = bitRead(hex_to_byte, 2);
    // read_bit3 = bitRead(hex_to_byte, 3);
    // read_bit4 = bitRead(hex_to_byte, 4);
    // read_bit5 = bitRead(hex_to_byte, 5);
    // read_bit6 = bitRead(hex_to_byte, 6);
    // read_bit7 = bitRead(hex_to_byte, 7);
    // ESP_LOGD("main", "Raw IC result: %s", result.c_str());
    // ESP_LOGD("main", "Substringed: %s", hexString.c_str());
    // ESP_LOGD("main", "Converted_To_Long: %li", hex_to_byte);
    // ESP_LOGD("main", "As Bits: %d%d%d%d%d%d%d%d", read_bit7,read_bit6,read_bit5,read_bit4,read_bit3,read_bit2,read_bit1,read_bit0);

    if (xsensor1 != nullptr)    xsensor1->publish_state(num_single_espresso);
    if (xsensor2 != nullptr)    xsensor2->publish_state(num_double_espresso);
    if (xsensor3 != nullptr)    xsensor3->publish_state(num_coffee);
    if (xsensor4 != nullptr)    xsensor4->publish_state(num_double_coffee);
    if (xsensor5 != nullptr)    xsensor5->publish_state(num_grinded_coffee);
    if (xsensor6 != nullptr)    xsensor6->publish_state(num_ristretto);
    if (xsensor7 != nullptr)    xsensor7->publish_state(num_double_restretti);
    if (xsensor8 != nullptr)    xsensor8->publish_state(num_cappuccino);
    if (xsensor9 != nullptr)    xsensor9->publish_state(num_latte);
    if (xsensor10 != nullptr)   xsensor10->publish_state(num_milk);
    if (xsensor11 != nullptr)   xsensor11->publish_state(num_hot_water);
    if (xsensor12 != nullptr)   xsensor12->publish_state(num_filter);
    if (xsensor13 != nullptr)   xsensor13->publish_state(num_milk_clean);
    if (xsensor14 != nullptr)   xsensor14->publish_state(num_decalcify);
 
    if (xsensor15 != nullptr)   xsensor15->publish_state(num_clean);
    if (xsensor16 != nullptr)   xsensor16->publish_state(tray_status);
    if (xsensor17 != nullptr)   xsensor17->publish_state(tank_status);
    if (xsensor18 != nullptr)   xsensor18->publish_state(num_all);
    if (xsensor19 != nullptr)   xsensor19->publish_state(status_all);

  }
};
