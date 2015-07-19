## Nixius ##
A simple ARM powered Nixie clock.

![Nixius](http://r00li.com/downloads/Nixius/Nixi_small.jpg)

**Features**

 - It's a clock
 - User settable auto turn on/turn off time (can turn off at night)
 - Manual display on/off button
 - Temperature display

**I want it!**
Great! Build it! This project includes everything that you need. A complete PCB, a 3D printable case and firmware source code.

**Instructions**

 1. Download the project
 2. Manufacture the PCB at your favourite PCB fab (I recommend elecrow.com). I've already included the gerbers so you don't even have to use eagle to export them - they are exported for use with elecrow so you may need to export them again if you want to manufacture them somewhere else.
 3. Order the necessary parts listed in bill of materials (I recommend Farnell/Element 14). Nixies themselves can be bought at siberian-shop.com or somewhere else.
 4. Solder everything together
 5. Flash the firmware to the micro. You will need any STM development board (they can be bought at Farnell for less than 10€) with STLINK. You can compile the project using free EmBlocks IDE (now also called EmBitz). It's a GCC based IDE so it should work with other compilers/IDEs as well. If you don't want to compile the project then you can simply used the .hex file in the bin folder. It's configured for use without the temperature sensor. But compiling everything in EmBlocks should only take a few clicks so I recommend that. 
 6. 3D print the case. It's designed to be 3D printable on most popular home 3D printers. 
 7. Assemble everything and enjoy

I am happy to provide any assistance that you may need. 

**More information**
Hackaday.io: https://hackaday.io/project/5543-nixius
My website: http://www.r00li.com

**Tinkering**
I've used many online sources to build this thing. Especially the power supply part and how to drive the Nixies. Do you want to use some of the bits from my design? Go right ahead! Don't want to use IN-1 nixies? You will just need to replace 4 resistors in the center with the correct ones and you'll be fine. And of course use some wire to connect the nixies. 

**License**
Licensed under GNU GPL v3.
