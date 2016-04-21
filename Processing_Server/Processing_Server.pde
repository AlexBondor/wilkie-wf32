import processing.net.*;

int port = 44300;       
Server myServer;  
Client myClient;

String points[];

int xPos[];
int yPos[];   

void setup()
{
  size(1000, 400);
  frameRate(30);
  
  background(0);
  myServer = new Server(this, port);
  myClient = myServer.available();
  while (myClient == null) {
    myClient = myServer.available();
  }   
  println("We have a new client: " + myClient.ip()); 
}

void draw()
{
  stroke(0, 255, 0);
  line(0, height / 2, width, height / 2);
  line(width / 2, height, width / 2, 0);
  
  String whatClientSaid = myClient.readString();
  if (whatClientSaid != null) {
    whatClientSaid = trim(whatClientSaid);
    println(myClient.ip() + ": " + whatClientSaid + "|");
    
    myServer.write(1);
    
    stroke(255, 0, 0);
    points = split(whatClientSaid, ',');
    for (int i = 0; i < points.length - 1; i = i + 2) {
     point(width / 2 + Integer.parseInt(points[i]), height / 2 - Integer.parseInt(points[i + 1]));
    }
  }
}