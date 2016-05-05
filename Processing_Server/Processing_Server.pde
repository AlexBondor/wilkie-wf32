import processing.net.*;

static int BLOCKS_COUNT = 50;
static int BLOCK_LENGTH = 5;
static int THRESHOLD = 2;

int port = 44300;       
Server myServer;  
Client myClient;

String points[];

int xPos[];
int yPos[];   

int lastX = 0, lastY = 0, peak, peakX = 0, peakY = 0;
int dotX = 0, dotY = 0;
int dotCounter = 0;

int sensor = 1;

PrintWriter output;

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

Block[][] blockMap = new Block[BLOCKS_COUNT][BLOCKS_COUNT];

int STEP;

Point blockCoordinates(int pointX, int pointY)
{
  Point result = new Point();
  
  pointX = (int)map(pointX, -BLOCKS_COUNT * BLOCK_LENGTH / 2, BLOCKS_COUNT * BLOCK_LENGTH / 2, -BLOCKS_COUNT * STEP / 2, BLOCKS_COUNT * STEP / 2);
  pointY = (int)map(pointY, -BLOCKS_COUNT * BLOCK_LENGTH / 2, BLOCKS_COUNT * BLOCK_LENGTH / 2, -BLOCKS_COUNT * STEP / 2, BLOCKS_COUNT * STEP / 2);

  result.x = pointX - ((pointX % STEP) + STEP) % STEP;
  result.y = pointY - ((pointY % STEP) - STEP) % STEP;

  println("-> " + result.x + " " + result.y);
  int tx = result.x;
  int ty = result.y;
  result.y = tx/ STEP + BLOCKS_COUNT / 2;
  result.x = BLOCKS_COUNT / 2 - ty / STEP;
  
  println("<- " + result.x + " " + result.y);
  println(".......");
  
  return result;
}

Point transformPoints(int originX, int originY, int pointX, int pointY, int heading)
{
  Point result = new Point();
  
  println("##" + originX + " " + originY + " " + pointX + " " + pointY + " " + heading);
  
  result.x = pointX * (int)cos(radians((float)heading - 90)) - pointY * (int)sin(radians((float)heading - 90)) + originX;
  result.y = pointY * (int)cos(radians((float)heading - 90)) + pointX * (int)sin(radians((float)heading - 90)) + originY;
  
  println("##" + result.x + " " + result.y);
  
  return result;
}

void setup()
{
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
  output = createWriter("positions.txt"); 
  
  myServer = new Server(this, port);    
  myClient = myServer.available();      
  while (myClient == null) {      
    myClient = myServer.available();       
  }          
  println("We have a new client: " + myClient.ip());
  
  size(1400, 800);     
  frameRate(30);      
  smooth();      
  background(255);
}

void draw()
{
  //fill(255, 255, 255, 5);
  //stroke(255, 255, 255);
  //rect(0, 0, width, height); 
  //stroke(100, 100, 100);
  //line(0, height / 2, width, height / 2);
  //line(width / 2, height, width / 2, 0);
  
  String whatClientSaid = myClient.readString();
  if (whatClientSaid != null && !whatClientSaid.equals("")) 
  {
   whatClientSaid = trim(whatClientSaid);
   output.println(whatClientSaid);
   println(myClient.ip() + ": " + whatClientSaid + "|");
    
   myServer.write(1);
    
   points = split(whatClientSaid, ',');
   try
   {
     for (int i = 0; i < points.length - 1; i = i + 2) 
     {
       points[i] = trim(points[i]);
       points[i+1] = trim(points[i+1]);
       if (!points[i].equals("") && !points[i + 1].equals(""))
       {
         if (points[i].contains("p") || points[i + 1].contains("p"))
         {
           lastX = Integer.parseInt(points[i].substring(1));
           lastY = Integer.parseInt(points[i + 1].substring(1));
           //stroke(255, 0, 0);
           //point(lastX, lastY);
         }
         else
         {
           if (points[i].contains("h") || points[i + 1].contains("h"))
           {
             peak = Integer.parseInt(points[i].substring(1));
           }
           else
           {
             dotX = Integer.parseInt(points[i]);
             dotY = Integer.parseInt(points[i + 1]);
             if ((dotX != 0 || dotY != 0) && (dotX != lastX && dotY != lastY))
             {
              println("dot " + dotCounter++);
                
              Point transformedPoint = transformPoints(lastX, lastY, dotX, dotY, peak);
              Point blockPoint = blockCoordinates(transformedPoint.x, transformedPoint.y);
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
              sensor ++;
                
               //stroke(0, 0, 0);
               //strokeWeight(1);
               //pushMatrix();
               //translate(lastX, lastY);
               //rotate(-radians(peak - 90));
               //point(dotX, -dotY);
               //popMatrix();
               //strokeWeight(1);
             }
           }
         }
       }
     }
   }
   catch(Exception e) 
   {
     e.printStackTrace();
   }
  }
  else 
  {
   if (myClient == null) 
   {
     while (myClient == null) 
     {
       myClient = myServer.available();
     } 
   }
  }
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
      int y = - blockMap[i][j].position.y + height / 2;
      rect(x, y, STEP, STEP);
    }
  }
  stroke(0, 0, 255);
  int cx;
  int cy;
  cx = (int)map(lastX, -BLOCKS_COUNT * BLOCK_LENGTH / 2, BLOCKS_COUNT * BLOCK_LENGTH / 2, -BLOCKS_COUNT * STEP / 2, BLOCKS_COUNT * STEP / 2);
  cy = (int)map(-lastY, -BLOCKS_COUNT * BLOCK_LENGTH / 2, BLOCKS_COUNT * BLOCK_LENGTH / 2, -BLOCKS_COUNT * STEP / 2, BLOCKS_COUNT * STEP / 2);
  drawArrow(cx, cy, 15, -peak);
  fill(255, 255, 255);
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

void keyPressed() 
{
  save("rawMap.png");
  output.flush(); // Writes the remaining data to the file
  output.close(); // Finishes the file
  exit(); // Stops the program
}