1. Main folder contains code that is written on Chipkit WF32 - it is the control code of the robots' peripherals
2. Zybo-SDK contains the code written on the Zybo Zynq-7000 - it sends commands via I2C to WF32 board in order to perform the autonomous algorithm
3. .bit file is the hardware design of the Zybo that can be loaded directly on the board
4. .tcl file generates the Vivado project if you want to make any changes to the hardware layer