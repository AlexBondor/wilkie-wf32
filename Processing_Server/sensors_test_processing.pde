import processing.serial.*;

Serial myPort;
int x1Pos = height / 2;
int y1Pos = width / 2;
int x2Pos = height / 2;
int y2Pos = width / 2;
int x3Pos = height / 2;
int y3Pos = width / 2;

float oldHeartrateHeight = 0;

void setup () 
{
  noLoop();
  // set the window size:
  size(1000, 400);
  frameRate(30);
  
  // List available serial ports.
  println(Serial.list());
  
  // Setup which serial port to use.
  // This line might change for different computers.
  myPort = new Serial(this, Serial.list()[0], 9600);
  
  // set inital background:
  background(255);
}

void draw () 
{
    stroke(0, 255, 0);
    //line(0, height / 2, width, height / 2);
    //line(width/ 2, height, width / 2, 0);
    stroke(0, 0, 0);
    point(x1Pos, y1Pos);
    stroke(0, 0, 0);
    point(x2Pos, y2Pos);
    stroke(0, 0, 0);
    point(x3Pos, y3Pos);
}

void serialEvent (Serial myPort) 
{
  // read the string from the serial port.
  String line = myPort.readStringUntil('\n');
  if (line != null)
  {
    String[] list = split(line, ' ');
    String pos = list[0];
    String s1 = list[1];
    String s2 = list[2];
    String s3 = list[3];
    if (pos != null) 
    {
      // trim off any whitespace:
      println(pos);
      
      int p = int(pos) - 15;
      
      x1Pos = height / 2;
      y1Pos = width / 2;
      x2Pos = height / 2;
      y2Pos = width / 2;
      x3Pos = height / 2;
      y3Pos = width / 2;

      if (s1 != null)
      {
        x1Pos = int(float(s1) * cos(-(p - 90) * 0.0174533) * 3) + width / 2;
        y1Pos = int(float(s1) * sin(-(p - 90) * 0.0174533) * 3) + height / 2;
      }
      
      if (s2 != null)
      {
        x2Pos = int(float(s2) * cos(-p * 0.0174533) * 3) + width / 2;
        y2Pos = int(float(s2) * sin(-p * 0.0174533) * 3) + height / 2;
      }
      
      if (s3 != null)
      {
        x3Pos = int(float(s3) * cos(-(p + 90) * 0.0174533) * 3) + width / 2;
        y3Pos = int(float(s3) * sin(-(p + 90) * 0.0174533) * 3) + height / 2;
      }
      redraw();
    }
  }
}