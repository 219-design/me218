# ME218 PCB Ruler Electrical Resources
This folder contains everything you need to order, build, and debug the board yourself.

Interested in a specific project? Check out the [examples folder](https://github.com/219-design/me218/tree/lib/pcb-ruler/firmware/ruler_lib/examples). 

### Electrical Projects
![Electrical Demos](https://github.com/219-design/me218/blob/main/pcb-ruler/photos/electrical.jpg)

Experiment with electronics by soldering on a variety of components to build your own custom projects.  

This board features footprints for:  
- Seeed Studio XIAO Series MCU module  
- Battery connector  
- Button D-pad  
- Neopixels  
- Screen  
- Bypass capacitors  
- Power LEDs  
- QWIIC connector  
- Surface-mount and through-hole protoboard areas  

#### Customization and Flexibility
The board includes **custom solder jumpers** for maximum flexibility.  
- Some jumpers are **normally open**, while others are **normally closed**.  
- These can be used to add series elements or to solder a wire directly to a connection point.  
- To use the **buttons** or **Neopixels**, simply solder the appropriate jumper to the desired I/O pin (D0–D10).


### Assembly
So you have a ME218 PCB Ruler and you want to make a cool electrical design. Where do you start?

The board is designed to be used with a [Seeed Studio XIAO Series MCU module](https://www.seeedstudio.com/xiao-series-page).
These modules are all pin compatible, so pick which one has the features you want for your project.
Mount the module to the U1 footprint. The footprint includes options to surface mount solder, TH solder, or TH socket the module. Choose what is best for your project.

Check out the BOM for part recommendations. Check the examples folder for demo specific BOMs.

#### Bypass caps
The board has four footprints (C1-C4) for bypass caps to provide some localized power.

#### Power LEDs
The board has two power LEDs: 5V (D1) and 3V3 (D2). Recommend installing these and their associated current limiting resistors (R1, R2).

#### Want to use use a LiPo battery?
Surface mount solder the MCU to U1 so the BAT- and BAT+ TH pins tought the pads on the bottom of the module.
Solder a connector to P17 and flow solder through the BAT- and BAT+ TH pads to connect the pads to the pads on the bottom of the MCU module.
Choose a battery that meets your project specs and make sure the pinout matches the connector.

#### Want to use buttons?
Solder buttons to as many of the D-pad button pads as you would like. Make sure to solder the center button first if you want to use it since it will be harder to solder it after installing the outer buttons.
The buttons aren't connected to any IO pins by default for flexibility. To hook up the buttons, solder jumper wire from the appropriate custom jumper (ex. CENTER_TP for the CENTER button) to the desired IO pin (header marked D0-D10). 
The buttons short to return on press. The circuit doesn't include a pull up or deboucing circuit so recommend implementing this in firmware.
Note that D4 and D5 are used for I2C so don't use those for the buttons if you are using any I2C devices in your project.

#### Want to use Neopixels?
Solder Neopixels to as many of the pads as you would like (U2, U3, U4, U5, U6). Note that Neopixels works on single wire communication and ours are connected in numerical order. So if you would like to use only one Neopixel, use U2. Otherwise you will need to short the input and output pins across the unused Neopixels in the chain before the one you want to use in order for the signal to reach a pad later in the chain.
The Neopixel line isn't connected to an IO pin by default for flexibility. To hook up the Neopixel chain, solder jumper wire from the Neopixel_TP custom jumper to your desired IO pin (header marked D0-D10).
Note that D4 and D5 are used for I2C so don't use those for the Neopixels if you are using any I2C devices in your project.

#### Want to use a Screen?
Ideally pick a screen with a pinout that matches the silk on the board. See the BOM for the one we used.
If you have a different I2C screen that you would like to use but it has a different pinout for it's 100mil header, you can reconfigure the pinout by cutting the custom jumpers under the pin. Then use the surface mount or through hole pads to wire up the correct pinout with some jumper wire.
Solder your screen into either P8 or P9. There are a few mounting holes that can be used if you mount it in P8, however it will overlap the neopixel in U6. If you would like to avoid overlaping the neopixel, use P9.

#### Want to use a QWIIC module?
Solder a QWIIC connector to J1 and plug in any QWIIC module. This footprint is already wired up to power and I2C so it is ready to go.

#### Have a 100mil header sensor module?
There are two 100mil header breakout board sections on the board. The one at the end is the easier of the two to use and comes with NC custom jumpers. Just solder your module to the last row of TH headers and then solder a wire from either side of the custom jumper to the desired power, return, or IO pins.
See our ToF demo for an example of this.

#### Have other parts?
Use one of the three breakout areas to solder TH or SMT components. Use jumper wire to connect the pins to the appropriate IO and power signals.
