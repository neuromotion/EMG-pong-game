// Graphing sketch


// This program takes ASCII-encoded strings
// from the serial port at 9600 baud and graphs them. It expects values in the
// range 0 to 1023, followed by a newline, or newline and carriage return

// Created 20 Apr 2005
// Updated 24 Nov 2015
// by Tom Igoe
// This example code is in the public domain.

import processing.serial.*;

Serial myPort;        // The serial port
float xPos = width/2;         // horizontal position of the graph
float inUser1 = 0;
float inUser2 = 0;
int scaleFactor = 10;
int currentState = 0;
int timeNow;
void setup () {
  // set the window size:
  size(1000, 400);

  // List all the available serial ports
  // if using Processing 2.1 or later, use Serial.printArray()
  // println(Serial.list());

  // I know that the first port in the serial list on my mac
  // is always my  Arduino, so I open Serial.list()[0].
  // Open whatever port is the one you're using.
  myPort = new Serial(this, Serial.list()[1], 9600);

  // don't generate a serialEvent() unless you get a newline character:
  myPort.bufferUntil('\n');

  // set inital background:
  background(0);
  xPos = width/2;
  timeNow = millis();
    textSize(32);
    textAlign(CENTER);
}

void draw () {
  switch(currentState) {
  case 0: // Ready? Screen 
  xPos = width/2;
    background(254, 204, 0);
    // Display start screen
    println("Current State Is:");
    println(currentState);
    text("Ready?", width/2, height/2);
    if ( millis() - timeNow > 2000) {
      currentState = 1;
      timeNow = millis();
    }
  break;
  case 1: // Go! Screen 
    background(204, 255, 0);
    text("Go!", width/2, height/2);
    
    if ( millis() - timeNow > 2000) {
      currentState = 2;
      timeNow = millis();
    }
  break;
  case 2: // The Game
  /*
  // draw the line:
  stroke(127, 34, 255);
  line(xPos, height, xPos, height - inUser1);
  
  // draw the line:
  stroke(255, 0, 0);
  line(xPos, height, xPos, height - inUser2);
  */
  background(0);
  ellipse(xPos, height/2, 100, 100);
  
  // Rescale user inputs to make the tug of war slower
  inUser1 = inUser1/scaleFactor;
  inUser2 = inUser2/scaleFactor;
  xPos = xPos + (inUser1 - inUser2);
  
  // at the edge of the screen, go back to the beginning:
  if (xPos >= width) {
    currentState = 3;
    timeNow = millis();
  }  else if (xPos <= 0) {
    currentState = 4;
    timeNow = millis();
  } else {
  }
    break;
    case 3: // Player 1 Wins!
    background(255, 0, 0);
    text("Player 1 Wins!", width/2, height/2);
    
    if ( millis() - timeNow > 2000) {
      currentState = 0;
      timeNow = millis();
    }
    break;
    case 4: // Player 2 Wins!
    background(0, 0, 255);
    text("Player 2 Wins!", width/2, height/2);
    
    if ( millis() - timeNow > 2000) {
      currentState = 0;
      timeNow = millis();
    }
    break;
  }
}


void serialEvent (Serial myPort) {
  // get the ASCII string:
  String inString = myPort.readStringUntil('\n');

  if (inString != null) {
    // trim off any whitespace:
    inString = trim(inString);
    // parse the string
    String[] inBytes = split(inString, ',');
    // convert to an int and map to the screen height:
    inUser1 = float(inBytes[0]);
    inUser2 = float(inBytes[1]);
    print(inUser1);
    print(',');
    println(inUser2);
    inUser1 = map(inUser1, 0, 50, 0, height); //1023
    inUser2 = map(inUser2, 0, 50, 0, height); //1023
  }
}

/* Max/MSP v5 patch for this example
 ----------begin_max5_patcher----------
1591.3oc0YszbaaCD9r7uBL5RalQUAO3CvdyS5zVenWZxs5NcfHgjPCIfJIT
RTxj+6AOHkoTDooroUs0AQPR73a+1cwtK3WtZxzEpOwqlB9YveAlL4KWMYh6
Q1GLo99ISKXeJMmU451zTUQAWpmNy+NM+SZ2y+sR1l02JuU9t0hJvFlNcMPy
dOuBv.U5Rgb0LPpRpYBooM3529latArTUVvzZdFPtsXAuDrrTU.f.sBffXxL
vGE50lIHkUVJXq3fRtdaoDvjYfbgjujaFJSCzq4.tLaN.bi1tJefWpqbO0uz
1IjIABoluxrJ1guxh2JfPO2B5zRNyBCLDFcqbwNvuv9fHCb8bvevyyEU2JKT
YhkBSWPAfq2TZ6YhqmuMUo0feUn+rYpY4YtY+cFw3lUJdCMYAapZqzwUHX8S
crjAd+SIOU6UBAwIygy.Q1+HAA1KH6EveWOFQlitUK92ehfal9kFhUxJ3tWc
sgpxadigWExbt1o7Ps5dk3yttivyg20W0VcSmg1G90qtx92rAZbH4ez.ruy1
nhmaDPidE07J+5n2sg6E6oKXxUSmc20o6E3SPRDbrkXnPGUYE.i5nCNB9TxQ
jG.G0kCTZtH88f07Rt0ZMMWUw8VvbKVAaTk6GyoraPdZff7rQTejBN54lgyv
HE0Ft7AvIvvgvIwO23jBdUkYOuSvIFSiNcjFhiSsUBwsUCh1AgfNSBAeNDBZ
DIDqY.f8.YjfjV1HAn9XDTxyNFYatVTkKx3kcK9GraZpI5jv7GOx+Z37Xh82
LSKHIDmDXaESoXRngIZQDKVkpxUkMCyXCQhcCK1z.G457gi3TzMz4RFD515F
G3bIQQwcP3SOF0zlkGhiCBQ1kOHHFFlXaEBQIQnCwv9QF1LxPZ.A4jR5cyQs
vbvHMJsLll01We+rE2LazX6zYmCraRrsPFwKg1ANBZFY.IAihr8Ox.aH0oAL
hB8nQVw0FSJiZeunOykbT6t3r.NP8.iL+bnwNiXuVMNJH9H9YCm89CFXPBER
bz422p8.O4dg6kRxdyjDqRwMIHTbT3QFLskxJ8tbmQK4tm0XGeZWF7wKKtYY
aTAF.XPNFaaQBinQMJ4QLF0aNHF0JtYuHSxoUZfZY6.UU2ejJTb8lQw8Fo5k
Rv6e2PI+fOM71o2ecY1VgTYdCSxxUqLokuYq9jYJi6lxPgD2NIPePLB0mwbG
YA9Rgxdiu1k5xiLlSU6JVnx6wzg3sYHwTesB8Z5D7RiGZpXyvDNJY.DQX3.H
hvmcUN4bP1yCkhpTle2P37jtBsKrLWcMScEmltOPv22ZfAqQAdKr9HzATQwZ
q18PrUGt6Tst2XMCRUfGuhXs6ccn23YloomMqcTiC5iMGPsHsHRWhWFlaenV
XcqwgCQiGGJzptyS2ZMODBz6fGza0bzmXBj7+DA94bvpR01MffAlueO7HwcI
pWCwmzJdvi9ILgflLAFmyXB6O7ML0YbD26lenmcGxjVsZUN+A6pUK7AtTrPg
M+eRYG0qD9j4I7eEbco8Xh6WcO.or9XDC6UCiewbXHkh6xm5LiPEkzpJDRTu
mEB44Fgz4NCtJvX.SM1vo2SlTCZGAe7GZu6ahdRyzFOhYZ+mbVVSYptBw.K1
tboIkatIA7c1cTKD1u.honLYV04VkluHsXe0szv9pQCE9Ro3jaVB1o15pz2X
zYoBvO5KXCAe0LCYJybE8ZODf4fV8t9qW0zYxq.YJfTosj1bv0xc.SaC0+AV
9V9L.KKyV3SyTcRtmzi6rO.O16USvts4B5xe9EymDvebK0eMfW6+NIsNlE2m
eqRyJ0utRq13+RjmqYKN1e.4d61jjdsauXe3.2p6jgi9hsNIv97CoyJ01xzl
c3ZhUCtSHx3UZgjoEJYqNY+hYs5zZQVFW19L3JDYaTlMLqAAt1G2yXlnFg9a
53L1FJVcv.cOX0dh7mCVGCLce7GFcQwDdH5Ta3nyAS0pQbHxegr+tGIZORgM
RnMj5vGl1Fs16drnk7Tf1XOLgv1n0d2iEsCxR.eQsNOZ4FGF7whofgfI3kES
1kCeOX5L2rifbdu0A9ae2X.V33B1Z+.Bj1FrP5iFrCYCG5EUWSG.hhunHJd.
HJ5hhnng3h9HPj4lud02.1bxGw.
-----------end_max5_patcher-----------

 */