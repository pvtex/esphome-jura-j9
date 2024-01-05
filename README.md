## ESPhome - Jura Impressa J9.3
This is an ESPhome custom component to communicate with a Jura Impressa J9.3 coffee machine.  It may also work with other serially-controlled Jura models with minimal adjustment.

It allows monitoring and control via HomeAssistant:

<img src="images/controlelements.png" alt="drawing" width=30%/><img src="images/sensors.png" alt="drawing" width=30%/>

***

Hardware is a Wemos D1 Mini connected to the 7-pin Jura service port via a 3.3V<->5V logic level converter.\
The D1 mini is powered from the Jura.

<img src="images/seven-pin-interface.jpg" alt="Jura 7-pin interface">

Above image taken from [here](https://community.home-assistant.io/t/control-your-jura-coffee-machine/26604).

<img src="images/connection-diagram.png" alt="Jura 7-pin interface">

If you have diffuculty, try swapping the TX/RX pins.

***

Commands for your machine can be generated using the provided script, `generate_esphome_jura_yaml.py`.  It requires the `bitarray` Python module to be installed.

      $ ./generate_esphome_jura_yaml.py AN:01

            - uart.write: [0xDF, 0xDB, 0xDB, 0xDF]  ## 'A'
            - delay: 8ms
            - uart.write: [0xFB, 0xFF, 0xDB, 0xDF]  ## 'N'
            - delay: 8ms
            - uart.write: [0xFB, 0xFB, 0xFF, 0xDB]  ## ':'
            - delay: 8ms
            - uart.write: [0xDB, 0xDB, 0xFF, 0xDB]  ## '0'
            - delay: 8ms
            - uart.write: [0xDF, 0xDB, 0xFF, 0xDB]  ## '1'
            - delay: 8ms
            - uart.write: [0xDF, 0xFF, 0xDB, 0xDB]  ## '\r'
            - delay: 8ms
            - uart.write: [0xFB, 0xFB, 0xDB, 0xDB]  ## '\n'

Particular commands seem to vary by model.\
These work on the Impressa J6, software `TY: PIM V01.01`, `TL: LOADER V3.0`.
Command | Action
--- | ---
AN:01 | Switch On   
AN:02 | Switch Off   
   
FA:01 | Switch off, including rinse    
FA:02 | Menu button    
FA:03 | Make 1 Espresso    
FA:04 | Make 1 Coffee    
FA:05 | Make 1 Ristretto    
FA:06 | Make 1 Cappuccino    
FA:07 | Make 1 Latte Macchiato    
FA:08 | Make Hot Water     
FA:09 |     
FA:0A |     
FA:0B | ?? Cup illumination light comes on. Also exits menu system.     
FA:0C |      
FA:0D | ?? Cycles through menu options [dial counter-clockwise]    
FA:OE | ?? Cycles through menu options clockwise [dial-clockwise]     
FA:0F | ?? Reads "B. Full", Freezes up     
     
DA:16 | ?? Shows the final two digits on the display, i.e. "16.."    


Response from RT Command:    
    
rt:00800001448B00000046001400062CB4005600208C7E00020000001A0000007D
0123456789012345678901234567890123456789012345678901234567890123456

|       |Position| DEVICE |STRING |   
| :---: | :---:  | :---:  | :---: |
|S Espresso|(3,7)|128|0080:128|  
|Ristretto|(7,11)|1|0001:1|   
|Kaffee|(11,15)|17547|448B:17547|
||(15,19)|0|0000:0|   
|Cappuccino|(19,23)|70|0046:70|    
|Latte|(23,27)|20|0014:20|    
|grinded Kaffee|(27,31)|6|0006:6|  
||(31,35)||2CB4:11444|
|Reinigung|(35,39)|86|0056:86|    
|Entkalkung|(39,43)|32|0020:32| 
||(43,47)||8C7E:35966|
||(47,51)||0002:2|
|d Ristretti|(51,55)||0000:0|     
||(55,59)||001A:26|     
|d Espresso|(59,63)|6|0006:6|    
||(63,67)||007D:125|
|||||    
|Heiswasser|()|15||     
|Milch|()|3||     
|Filter|()|4||     
|Milchreinigung|()|12||     
|2 Kaffee|()|45||    
|Total|()|17823||    
     
    
    
#### To-Do:
- Determine how to initiate a Force Rinse action on this model
- Status of "Fill Beans", "Need Cleaning", and "Need Flushing"
- Actual machine power state (currently we use an 'Optimistic', 'Assumed State' Template switch in ESPhome)
