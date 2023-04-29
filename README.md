# open_source_insulin_pump
This project was originally created by the team at UC Bioengineering, University of Canterbury, Christchurch, New Zealand.

This project has been modified and adapted by students at New Jersey Institute of Technology, Newark, New Jersey, United States.

# Build and Installation Instructions
## Insulin Pump
In order to run the Insulin Pump + Artificial Pancreas System (APS) software system, you will need to physically construct the insulin pump using the instructions from https://www.ncbi.nlm.nih.gov/pmc/articles/PMC9679028/. As a side note, per our correspondence with the author, you will also need experience in using a pick and place machine to assemble the PCB as there is no CPL file. Then you will need to use a debugger/profiler device to upload our modified Insulin Pump build files, which can be located in Code File/Version2.1/MyProject2/Release. The paper recommends using an Atmel Ice debugger which can be found at https://www.microchip.com/DevelopmentTools/ProductDetails/ATATMEL-ICE.  You will then upload either the MyProject2.bin, MyProject2.hex, or MyProject2.elf. 
## Artificial Pancreas System
The Artificial Pancreas System (APS) has been optimized to run on an Arduino Nano 33 IoT, which can be found at https://store-usa.arduino.cc/products/arduino-nano-33-iot. Uploading the APS to the Arduino Nano 33 IoT is a simpler process. Using the Arduino IDE, you can load our aps.ino file located in APS Arduino/aps.ino/aps/aps.ino. You can use a USB to Micro USB cable to upload our Arduino file to the board. Note that care must be taken to specifically choose Aurduino Nano 33 IoT and not Arduino Nano or Arduino Nano BLE since there is a risk of wiping out the bootloader. Also note that you need to install the Arduino BLE through the library manager in the Arduino IDE.
## Final Steps
As the Insulin Pump is an isolated system, you need to connect it to the APS system through its various pins. The Insulin Pump chip has three pins that we have labelled.
1. PA25 = APS_BUTTON1
2. PA27 = APS_BUTTON2
3. PA28 = APS_BUTTON3

The Arduino Nano 33 IoT has three corresponding pins that need to be connected.
1. PA06 = APS_BUTTON1
2. PA07 = APS_BUTTON2
3. PA18 = APS_BUTTON3

We believe using jumper cables to connect the pins is the best solution for quickly making sure the connections work. 

# Running
To run the Insulin Pump + Artificial Pancreas System, you need to power on both systems independently. The APS system will then automatically begin sending signals to the insulin pump. By connecting the APS system to a computer with a USB to Micro USB cable, you can view output messages to verify its output. 

Our design document can be found at https://docs.google.com/document/d/1LCMe2zzDWu8Dm9aOntk7V3nquZGxD0mBM0FljLpEkNM/edit#.
