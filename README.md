# A Game of Tetris
I have been doing alot of interview prep lately, and decided that I wanted to build something fun to switch things up. I decided that to build a game of Tetris on my Arduino after seeing an article on the [Tetris MicroCard](http://www.tetrismicrocard.com/). My version isn't as fancy (I used two 8x8 LED matrices as opposed to an OLED for example), but it works, and I'm quite happy with it. 

## Game Play
A random shape is spawned at the top of the board. It gradually moves downwards until it hits the bottom of the board, or collides with a piece that has previously stopped. The idea is to move the shapes to complete full rows of blocks with no gaps. 

![Connections](/img/tetris_game.jpg)

Each complete row will disappear, giving you more room to as you continue playing. 

The Left, Right and Rotate buttons can be used to move the shapes. The Down button quick drops the pieces to the board.

The game gradually speeds up proportional to the number of rows cleared.

When the blocks pile up beyond the top of the playing area, the game is over. The number of rows cleared is shown on the game board.

![Connections](/img/gameover.jpg)

## Technical Bits

### Parts Required
* Arduino  - I used an Arduino-compatible Pro-micro, but it should work on any Arduino
* 4 Push Buttons - _Left_, _Right_, _Down_ and _Rotate_
* 2 8x8 LED Matrix Module - I picked up [these](http://tinkersphere.com/led-matrix-panels/626-serial-led-matrix-module-arduino-compatible.html) that come with the MAX7219 serial driver. Kinda wish I had bought the RGB ones though.
* BreadBoard
* Wires (8x male to male jumper wires for push buttons, and 10 Dupont wires for the led matrices)

### Connections

For the Arduino: 7 Digital I/O Pins are used
- 3 PINS are used for the LED Matrices. These are connected to the DIN, CS, CLK pins of the first led matrix module. The remaining VCC and GND Pins of the LED matrix should go to the VCC and GND Pins of the Arduino.  This will be the top module (new shapes spawn here). The second module is connected in a serial version from the first led matrix module.  
- 4 PINS are used for the Push Buttons. The Push Buttons should be connected in [INPUT_PULLUP pin mode](https://www.arduino.cc/en/Tutorial/InputPullupSerial). That is, a wire is used to connect from the digital pin of the Arduino to one leg of the push button, and a second wire is used to connect a second leg of the push button to ground.

Here are my connections for the Pro-Micro:
| Connection | Pin |
| --- | --- |
| CS | 14 |
| DIN | 15 |
| CLK | 16 |
| RotatePB | 6 |
| LeftPB | 7 |
| RightPB | 9 |
| DownPB | 8 |

![Connections](/img/connections.png)

### External Libraries used
[LedControl Library](https://github.com/wayoda/LedControl) for serial communications with the LED matrix

### Rendering shapes
I was looking around for a good way to represent tetris shapes. These typically fall into two categories
1) using an array of 1s and 0s - [example here](https://www.marginallyclever.com/2015/01/arduino-tetris-2/)
2) using 2 bytes to represent a shape - [example here](http://engineerish.com/post/149133126116/arduino-tetris-part-1)

I liked the second method more - the code is abit cleaner, and requires less space. To help in debugging, particularly when making sure that the tetris blobs don't rotate off the board, I created this table to help in debugging.

![Tetris shapes](/img/tetris_forms.png)

