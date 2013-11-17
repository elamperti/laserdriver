class Laserpoint {
  boolean laser;
  int x, y;
  
  void Laserpoint(boolean tempLaser, int tempX, int tempY) {
    this.laser = tempLaser;
    this.x = tempX;
    this.y = tempY;
  }
  
  @Override
  public String toString() {
    return "{" + (this.laser ? "1" : "0") + "," + this.x + "," + this.y + "},";
  }
}

ArrayList<Laserpoint> points;
boolean locked;
int previousX = 0;
int previousY = 0;

void setup() {
  PShape backdrop;
  
  size(800, 800);
  background(0);
  
  backdrop = loadShape("Hackaday_Logo.svg");
  shape(backdrop, 0,0, 800, 800);
  
  points = new ArrayList<Laserpoint>();
  points.add(new Laserpoint());
  noLoop();
}

void draw() 
{ 
  // Nothing to do here.
}

void mousePressed() {
  Laserpoint foo = new Laserpoint();
  if (mouseButton == LEFT) {
    stroke(255,0,0);
    foo.laser = true;
  } else if (mouseButton == RIGHT) {
    stroke(120,120,120);
    foo.laser = false;    
  }
  line(previousX, previousY, mouseX, mouseY);
  foo.x = mouseX;
  foo.y = mouseY;
  previousX = mouseX;
  previousY = mouseY;
  points.add(foo);
  point(mouseX, mouseY);
  redraw();
}

void keyPressed() {
  switch (keyCode) {
    case 127: // delete (removes last Laserpoint)
      if (points.size() > 1) {
        // index of Laserpoint to be removed
        int corpse = points.size() - 1;
        
        // draw black line over the last line (undo)
        Laserpoint[] eraseBack;
        eraseBack = new Laserpoint[2];
        eraseBack[0] = points.get(corpse - 1);
        eraseBack[1] = points.get(corpse);
        previousX = eraseBack[0].x;
        previousY = eraseBack[0].y;
        stroke(0);
        strokeWeight(2); 
        line(eraseBack[0].x, eraseBack[0].y, eraseBack[1].x, eraseBack[1].y);
        strokeWeight(1);
        redraw();
        
        // remove Laserpoint
        points.remove(corpse);
      }
      break;
    
    case 10: // enter (done with this)
      selectOutput("Select a file to write to:", "fileSelected");
      break;

  }

}

void fileSelected(File selection) { // called when enter key is pressed
  if (selection != null) {
    PrintWriter output;
    output = createWriter(selection.getAbsolutePath());
    output.print("{");
    for (Laserpoint item : points) {
      output.print(item.toString());
    }
    output.print("}");
    output.flush();
    output.close();
  }

}
