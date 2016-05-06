import processing.net.*;



// Server variables
int port = 44300;       
Server myServer;  
Client myClient;



// Variables to store the incoming data
// from the robot via WiFi connection
int positionX, positionY;
int heading;
int sensorX, sensorY;
String points[];



// Map of the robot is represented as a grid of blocks
// green block - no obstacle there
// red block   - sensors sensed some obstacle there
static int BLOCKS_COUNT = 20; // Size of the map in blocks
static int BLOCK_LENGTH = 5;
static int THRESHOLD = 10;
int STEP; // Used to scale map to frame size
int ROBOT_SIZE;

Block[][] blockMap = new Block[BLOCKS_COUNT][BLOCKS_COUNT];
int sensor = 1;



// This function gets called only once at the beginning
// of the process
void setup()
{
  println("Hello!"); 
  println("Waiting for client connection..");
  // Initialize server and wait for the first client
  // to connect
  myServer = new Server(this, port);
  myClient = myServer.available();
  while (myClient == null)
  {
   myClient = myServer.available();
  }   
  println("We have a new client: " + myClient.ip());
    
  println("Initializing map");
  // Initialize map
  STEP = width / BLOCKS_COUNT;
  Point currentPosition = new Point();
  currentPosition.x = (BLOCKS_COUNT / 2) * -STEP;
  currentPosition.y = (BLOCKS_COUNT / 2) * STEP;
  for (int i = 0; i < BLOCKS_COUNT; i++)
  {
   for (int j = 0; j < BLOCKS_COUNT; j++)
   {
     blockMap[i][j] = new Block(currentPosition.x, currentPosition.y);
     blockMap[i][j].leftEyePoints = 0;
     blockMap[i][j].frontEyePoints = 0;
     blockMap[i][j].rightEyePoints = 0;
     currentPosition.x += STEP;
   }
   currentPosition.x = (BLOCKS_COUNT / 2) * -STEP;
   currentPosition.y -= STEP;
  }
  
  
  ROBOT_SIZE = 25 * STEP / BLOCK_LENGTH;  
  
  
  println("Initialize frame");
  // Initialize frame
  size(1000, 1000); // Keep the size of the frame to a square
  frameRate(30);
  smooth();
  background(255);
}

// Draw function that gets called continously
void draw()
{
  println("reading");
  
  try
  {
    String whatClientSaid = myClient.readString();
    if (whatClientSaid != null && !whatClientSaid.equals("")) 
    {
      whatClientSaid = trim(whatClientSaid); // Get rid of possible spaces
      
      println("got: " + whatClientSaid + "|");
      
      myServer.write(1); // Ack that we received data
      
      points = split(whatClientSaid, ',');
      
      try
      {
        for (int i = 0; i < points.length - 1; i = i + 2)
        {
          // Time to parse the received data
          points[i] = trim(points[i]);
          points[i + 1] = trim(points[i + 1]);
          
          if (!points[i].equals("") && !points[i + 1].equals(""))
          {
            if (points[i].contains("p") || points[i + 1].contains("p"))
            {
              // We are parsing the position of the robot
              positionX = Integer.parseInt(points[i].substring(1));
              positionY = Integer.parseInt(points[i + 1].substring(1));
            }
            else
            {
              if (points[i].contains("h") || points[i + 1].contains("h"))
              {
                // We are parsing the heading of the robot
                heading = Integer.parseInt(points[i].substring(1));
              }
              else
              {
                if (points[i].contains("l") || points[i + 1].contains("l"))
                {
                  sensor = 1;
                }
                if (points[i].contains("f") || points[i + 1].contains("f"))
                {
                  sensor = 2;
                }
                if (points[i].contains("r") || points[i + 1].contains("r"))
                {
                  sensor = 3;
                }
                // We are parsing data from the sensors
                sensorX = Integer.parseInt(points[i].substring(1));
                sensorY = Integer.parseInt(points[i + 1].substring(1));
                if ((sensorX != 0 || sensorY != 0))
                {
                  Point transformedPoint = transformPoint(positionX, positionY, sensorX, sensorY, heading);
                  Point blockPoint = blockAt(transformedPoint.x, transformedPoint.y);
                  
                  if (blockPoint.x != -1 && blockPoint.y != -1)
                  {
                    switch(sensor)
                    {
                      case 1:
                      blockMap[blockPoint.x][blockPoint.y].leftEyePoints ++;
                      break;
                    case 2:
                      blockMap[blockPoint.x][blockPoint.y].frontEyePoints ++;
                      break;
                    case 3:
                      blockMap[blockPoint.x][blockPoint.y].rightEyePoints ++;
                      break;
                    default:
                      sensor = 1;
                      break;
                    }
                  }
                }
              } // end of else contains "h"
            } // end of else contains "p"
          } // end of if !points[i].equals("")
        } // end of for loop
      } // end of try clause
      catch (Exception e)
      {
        e.printStackTrace();
      }
    } // if whatClientSaid != null
    else
    {
      // Connection lost..
      if (myClient == null) 
      {
        println("Lost connection");
        while (myClient == null) 
        {
          myClient = myServer.available();
        } 
      }
    }
  }
  catch (Exception e)
  {
    e.printStackTrace();
  }
  // Do the drawing of the map now..
  stroke(0, 0, 0);
  for (int i = 0; i < BLOCKS_COUNT; i++)
  {
   for (int j = 0; j < BLOCKS_COUNT; j++)
   {
     if (blockMap[i][j].leftEyePoints + blockMap[i][j].frontEyePoints + blockMap[i][j].rightEyePoints > THRESHOLD)
     {
       fill(255, 0, 0);
     }
     else
     {
       fill(0, 255, 0);
     }
     int x = blockMap[i][j].position.x + width / 2;
     int y = -blockMap[i][j].position.y + height / 2;
     rect(x, y, STEP, STEP);
     fill(255, 255, 255);
     textSize(15);
     text(blockMap[i][j].leftEyePoints + blockMap[i][j].frontEyePoints + blockMap[i][j].rightEyePoints, x, y + STEP);
   }
  }
  Point scaledPoint = scaleCoord(positionX, positionY);
  if (scaledPoint.x != -1 && scaledPoint.y != -1)
  {
    drawRobot(scaledPoint.x + width / 2, height / 2 - scaledPoint.y, -heading);
    drawArrow(scaledPoint.x + width / 2, height / 2 - scaledPoint.y, 20, -heading);
  }
}

// Util classes
class Point
{
  public int x;
  public int y;
};

class Block
{
  public Point position = new Point();
  public int leftEyePoints = 0;
  public int frontEyePoints = 0;
  public int rightEyePoints = 0;
  
  public Block(int x, int y)
  {
    position.x = x;
    position.y = y;
  }
};

// Does a 2D transformation given the position of the robot and the coordinated
// of the point seen by the sensor
// The coordinates of the point seen by the sensor are relative to the center of the
// robot so it has to be translated with the actual position of the robot
Point transformPoint(int originX, int originY, float pointX, float pointY, int heading)
{
  Point result = new Point();
  
  //println("<transform>" + originX + " " + originY + " " + pointX + " " + pointY + " " + heading);
  
  result.x = (int)round(pointX * cos(radians((float)heading - 90)) - pointY * sin(radians((float)heading - 90))) + originX;
  result.y = (int)round(pointY * cos(radians((float)heading - 90)) + pointX * sin(radians((float)heading - 90))) + originY;
  
  //println("<transform>" + result.x + " " + result.y);
  
  return result;
}

Point blockAt(int pointX, int pointY)
{
  Point result = new Point();
  Point temp = scaleCoord(pointX, pointY);
  
  if (temp.x == -1 || temp.y == -1)
  {
    result.x = -1;
    result.y = -1;
    return result;
  }
  
  //println("<block:1>" + pointX + " " + pointY);

  result.x = temp.x - ((temp.x % STEP) + STEP) % STEP;
  result.y = temp.y - ((temp.y % STEP) - STEP) % STEP;
  
  //println("<block:2>" + result.x + " " + result.y);

  int tx = result.x;
  int ty = result.y;
  
  result.x = BLOCKS_COUNT / 2 - ty / STEP;
  result.y = tx/ STEP + BLOCKS_COUNT / 2;
  
  //println("<block:3>" + result.x + " " + result.y);
  
  return result;
}

Point scaleCoord(int pointX, int pointY)
{
  Point result = new Point();
  
  int min_reality = -BLOCKS_COUNT * BLOCK_LENGTH / 2;
  int max_reality = BLOCKS_COUNT * BLOCK_LENGTH / 2 - 1;
  
  int min_map = -BLOCKS_COUNT * STEP / 2;
  int max_map = BLOCKS_COUNT * STEP / 2 - 1;
  
  // Make sure the point doesn't get out of our range.
  result.x = pointX < min_reality ? -1 : pointX > max_reality ? -1 : pointX;
  result.y = pointY < min_reality ? -1 : pointY > max_reality ? -1 : pointY; 
  
  if (result.x == -1 || result.y == -1)
  {
    result.x = -1;
    result.y = -1;
    return result;
  }
  
  //println("<block:0>" + pointX + " " + pointY);
  
  result.x = (int)map(result.x, min_reality, max_reality, min_map, max_map);
  result.y = (int)map(result.y, min_reality, max_reality, min_map, max_map);
  
  return result;
}

void drawRobot(int cx, int cy, float angle)
{
  pushMatrix();
  translate(cx, cy);
  rotate(radians(angle));
  rect(-ROBOT_SIZE / 2, -ROBOT_SIZE / 2, ROBOT_SIZE, ROBOT_SIZE, 5);
  popMatrix();
  
}

void drawArrow(int cx, int cy, int len, float angle)
{
  pushMatrix();
  translate(cx, cy);
  rotate(radians(angle));
  line(0,0,len, 0);
  line(len, 0, len - 5, -5);
  line(len, 0, len - 5, 5);
  popMatrix();
}