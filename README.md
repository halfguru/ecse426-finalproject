## ECSE 426 Final Project Winter 2015

![Alt Text](https://media.giphy.com/media/1zRgQYRGi0NPul02Rd/giphy.gif)

This experiment involves the development of a 3D mechanical printer system which is controlled by a pair of STM Discovery F4 microcontroller boards, as well as physical moving parts. The desired final product of the experiment includes two main subsystems. The first subsystem essentially comprises the user interface which allows a user to determine and control what the machine will print next, as well as to enable printing itself. This subsystem relies on the STM32F429 board, which includes a built-in LCD screen and to which an alphanumeric keypad is connected. On the output end of the machine, another subsystem uses the STM32F4 board to which three HiTech HS422 stepper motors are connected. These motors are in turn attached to a set of custom designed parts which can hold a pen, pencil or marker. To communicate between the two subsystems, wireless communication is set up using two TI MSP430-CC2500 wireless chipsets. Once the system is fully operational, the user can use the keypad buttons to switch between two different drawing modes; one for drawing basic predetermined shapes and the other for a sequence of user-determined lines. The user’s inputs are displayed on the LCD, and once confirmed, control signals are sent wirelessly from one board to the other. The mechanical arms connected to the motors will then move in a 2D plane, as well as up and down, to draw the corresponding shapes.

### Prerequisites

[Keil MDK](https://www.keil.com/download/) is the complete software development environment used to program the ARM Cortex-M based microcontroller STM Discovery F4.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgements
* Xavier Sit
* Sidney Ng
* Meng Yin Tao
* Kai 
* Prof. [Zeljko Zilic](http://www.ece.mcgill.ca/~zzilic/) for his assistance throughout our ECSE 426 course.
