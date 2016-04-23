import processing.net.*;

int port = 44300;       
Server myServer;  
Client myClient;

String points[];

int xPos[];
int yPos[];   

int lastX = 0, lastY = 0, peak, peakX = 0, peakY = 0;
int dotX = 0, dotY = 0;
int dotCounter = 0;

void setup()
{
  size(1400, 800);
  frameRate(30);
  smooth();
  background(255);
  myServer = new Server(this, port);
  myClient = myServer.available();
  while (myClient == null) {
    myClient = myServer.available();
  }   
  println("We have a new client: " + myClient.ip()); 
}

void draw()
{
  fill(255, 255, 255, 5);
  stroke(255, 255, 255);
  rect(0, 0, width, height); 
  stroke(100, 100, 100);
  line(0, height / 2, width, height / 2);
  line(width / 2, height, width / 2, 0);
  
  String whatClientSaid = myClient.readString();
  if (whatClientSaid != null && !whatClientSaid.equals("")) {
    whatClientSaid = trim(whatClientSaid);
    //println(myClient.ip() + ": " + whatClientSaid + "|");
    
    myServer.write(1);
    
    points = split(whatClientSaid, ',');
    for (int i = 0; i < points.length - 1; i = i + 2) {
      if (!points[i].equals("") && !points[i + 1].equals(""))
      {
        if (points[i].contains("p"))
        {
          lastX = width / 2 + Integer.parseInt(points[i].substring(1, points[i].length())) * 2;
          lastY = height / 2 - Integer.parseInt(points[i + 1].substring(1, points[i + 1].length())) * 2;
          //stroke(255, 0, 0);
          //point(lastX, lastY);
        }
        else
        {
          if (points[i].contains("h"))
          {
            peak = Integer.parseInt(points[i].substring(1, points[i].length()));
            peakX = lastX + (int)(cos(radians(peak)) * 7);
            peakY = lastY - (int)(sin(radians(peak)) * 7);
            stroke(255, 0, 0);
            drawArrow(lastX, lastY, 15, -peak);
          }
          else
          {
            dotX = Integer.parseInt(points[i]) * 2;
            dotY = Integer.parseInt(points[i + 1]) * 2;
            if (dotX != 0 || dotY != 0)
            {
              println("dot " + dotCounter++);
              stroke(0, 0, 0);
              strokeWeight(5);
              pushMatrix();
              translate(lastX, lastY);
              rotate(-radians(peak - 90));
              point(dotX, -dotY);
              popMatrix();
              strokeWeight(1);
            }
          }
        }
      }
    }
  }
}

void drawArrow(int cx, int cy, int len, float angle){
  pushMatrix();
  translate(cx, cy);
  rotate(radians(angle));
  line(0,0,len, 0);
  line(len, 0, len - 5, -5);
  line(len, 0, len - 5, 5);
  popMatrix();
}