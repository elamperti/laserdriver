#include <Servo.h>
#include <Serial.h>

#define __LASER 0
#define __X 1
#define __Y 2

#define LASER_PIN   10       // Toggles the beam
#define SERVO_X_PIN 9        // Moves the X axis
#define SERVO_Y_PIN 8        // Moves the Y axis
#define KNOB_PIN    0        // Used to tune servos manually

#define MAX_SERIAL_INPUT_LENGTH 20

#define DEFAULT_STEP_DELAY 40 // ms to wait between quick sweeps
#define DEFAULT_WALK_DELAY 25 // ms to wait between line points
#define DEFAULT_WAIT_DELAY 25 // ms to wait on each point
#define DEFAULT_SERVO_MIN  700
#define DEFAULT_SERVO_MAX  2300

Servo servoX;
Servo servoY;

int servoXPos = 0;
int servoXMin = DEFAULT_SERVO_MIN;
int servoXMax = DEFAULT_SERVO_MAX;

int servoYPos = 0;
int servoYMin = DEFAULT_SERVO_MIN;
int servoYMax = DEFAULT_SERVO_MAX;

int stepDelay = DEFAULT_STEP_DELAY;
int walkDelay = DEFAULT_WALK_DELAY;
int waitDelay = DEFAULT_WAIT_DELAY;

boolean isDrawing = false;
boolean startDrawing = false;
boolean stopDrawing = false;
boolean serialInterpreterWorking = false;

char serialBuffer[MAX_SERIAL_INPUT_LENGTH];
byte serialBufferIndex = 0;

/* ToDo: each point can be stored in < 3 bytes */
int drawing[][3] = // { {__LASER, __X, __Y}, ...}
/* PASTE LASERPOINTS HERE */
{{0,0,0},{0,87,52},{1,96,49},{1,126,49},{1,160,60},{1,190,85},{1,203,109},{1,209,139},{1,208,160},{1,288,230},{1,270,241},{1,247,265},{1,235,284},{1,225,302},{1,222,313},{1,141,242},{1,117,247},{1,99,247},{1,67,234},{1,41,217},{1,15,174},{1,12,147},{1,15,127},{1,81,183},{1,147,108},{1,87,54},{0,515,229},{1,590,161},{1,590,133},{1,598,102},{1,614,78},{1,642,57},{1,665,50},{1,694,46},{1,714,49},{1,653,106},{1,722,183},{1,786,124},{1,786,154},{1,780,184},{1,758,216},{1,735,233},{1,698,246},{1,672,244},{1,657,241},{1,579,311},{1,557,274},{1,535,249},{1,515,231},{0,585,493},{1,573,523},{1,549,558},{1,535,574},{1,526,580},{1,592,638},{1,588,667},{1,603,705},{1,631,735},{1,663,749},{1,696,752},{1,716,748},{1,652,693},{1,723,616},{1,785,673},{1,786,636},{1,775,604},{1,754,580},{1,734,565},{1,711,559},{1,686,555},{1,669,556},{1,657,558},{1,586,495},{0,215,495},{1,215,495},{1,229,527},{1,250,557},{1,265,574},{1,272,580},{1,210,638},{1,211,653},{1,206,685},{1,190,713},{1,169,735},{1,133,749},{1,111,751},{1,85,748},{1,145,691},{1,79,616},{1,13,672},{1,13,642},{1,21,613},{1,35,591},{1,55,572},{1,81,558},{1,105,554},{1,129,556},{1,141,559},{1,214,492},{1,216,496},{0,228,488},{1,228,489},{1,216,438},{1,220,366},{1,235,323},{1,250,290},{1,283,249},{1,326,222},{1,366,209},{1,418,207},{1,464,218},{1,504,241},{1,540,278},{1,567,321},{1,580,363},{1,585,424},{1,578,466},{1,558,516},{1,529,555},{1,506,577},{1,509,588},{1,506,609},{1,498,624},{1,482,633},{1,463,631},{1,446,620},{1,441,606},{1,440,593},{1,436,593},{1,435,607},{1,427,619},{1,417,629},{1,394,634},{1,380,627},{1,368,613},{1,365,595},{1,365,593},{1,361,593},{1,361,606},{1,350,625},{1,333,632},{1,322,634},{1,303,625},{1,293,609},{1,289,595},{1,290,585},{1,295,577},{1,263,549},{1,241,512},{1,229,489},{0,296,467},{1,273,447},{1,267,422},{1,268,402},{1,285,375},{1,308,360},{1,341,363},{1,362,381},{1,370,393},{1,368,405},{1,355,419},{1,331,431},{1,310,441},{1,304,448},{1,303,458},{1,300,465},{1,296,468},{0,383,522},{1,384,503},{1,390,478},{1,398,465},{1,402,465},{1,410,480},{1,416,500},{1,417,514},{1,415,525},{1,413,529},{1,412,529},{1,406,510},{1,405,508},{1,401,505},{1,396,506},{1,391,517},{1,388,525},{1,386,528},{1,383,524},{0,462,427},{1,491,441},{1,495,452},{1,497,465},{1,504,467},{1,510,463},{1,523,451},{1,533,428},{1,533,408},{1,524,384},{1,505,368},{1,483,360},{1,459,362},{1,437,379},{1,430,392},{1,433,409},{1,447,421},{1,462,428},}
/* PASTE LASERPOINTS HERE */
;

int drawingSize;
unsigned int drawingBoundaries[2][2] = {{99999,0},{99999,0}}; // {Xmin, Xmax}, {Ymin, Ymax}

void setup() {   
    Serial.begin(9600);
    serialBuffer[0] = '\0';
    serialBuffer[MAX_SERIAL_INPUT_LENGTH - 1] = '\0';
    Serial.println(F("There you are."));
    
    // Quick setting of the default min/max values for debugging purposes
    /*
    servoXMin = 700;
    servoXMax = 1100;
    servoYMin = 700;
    servoYMax = 1000;
    // */
    
    setDrawingParameters();
    
    // Init servos
    servoX.attach(SERVO_X_PIN);
    servoY.attach(SERVO_Y_PIN);

    // Init laser
    pinMode(LASER_PIN, OUTPUT);
    digitalWrite(LASER_PIN, LOW);

    // Init position and move servos to drawing[0] (first point)
    //moveServos(drawing[0][__X], drawing[0][__Y]);
}

void loop() {
    if (startDrawing) {
        draw();
        startDrawing = false;
    }
    
    checkSerial();
}

void checkSerial() {
    char tmpChar;
    
    while (Serial.available() > 0) {
        tmpChar = Serial.read();
        
        if (tmpChar == 59) { // end of command (;)
            if (!serialInterpreterWorking) {
                serialInterpreterWorking = true;
                serialInterpreter();
            }
            
        } else {
            serialBuffer[serialBufferIndex] = tmpChar;
            serialBufferIndex++;
            serialBuffer[serialBufferIndex] = '\0';
        }
        
        if (serialBufferIndex > MAX_SERIAL_INPUT_LENGTH - 1) {
            Serial.println(F("No end of command detected. Must receive ';' after a command."));
            emptyBuffer();
        }
        
    }
    
}

void serialInterpreter() {
    if (strcmp(serialBuffer, "ping") == 0) {
        Serial.println(F("pong!"));
        
    } else {
        if (isDrawing) {
            if (strcmp(serialBuffer, "stop") == 0) {
                stopDrawing = true;
            }
            
        } else {
            if (strcmp(serialBuffer, "go") == 0) {
                // dirty async call to draw()
                startDrawing = true;
                
            } else {
                if (serialBufferIndex > 14) {
                           if (strncmp(serialBuffer, "servo x min", 11) == 0) {
                        servoXMin = sanitizeServoValue(servoXMin, __X);
                        
                    } else if (strncmp(serialBuffer, "servo x max", 11) == 0) {
                        servoXMax = sanitizeServoValue(servoXMax, __X);
                        
                    } else if (strncmp(serialBuffer, "servo y min", 11) == 0) {
                        servoYMin = sanitizeServoValue(servoYMin, __Y);
                        
                    } else if (strncmp(serialBuffer, "servo y max", 11) == 0) {
                        servoYMax = sanitizeServoValue(servoYMax, __Y);
                        
                } else if (serialBufferIndex > 11) {  
                    } else if (strncmp(&(serialBuffer[5]), "delay", 5) == 0) {
                        int newDelay = 0;
                        newDelay = atoi(&(serialBuffer[11]));
                        
                        if (newDelay == 0) {
                            Serial.println(F("Invalid value."));
                            
                        } else if (strncmp(serialBuffer, "walk", 4) == 0) {
                            walkDelay = newDelay;
                            Serial.print(F("Walk delay set to "));
                            Serial.println(walkDelay);
                            
                        } else if (strncmp(serialBuffer, "wait", 4) == 0) {
                            waitDelay = newDelay;
                            Serial.print(F("Wait delay set to "));
                            Serial.println(waitDelay);
                            
                        } else if (strncmp(serialBuffer, "step", 4) == 0) {
                            stepDelay = newDelay;
                            Serial.print(F("Step delay set to "));
                            Serial.println(stepDelay);
                            
                        } else {
                            serialCommandUnrecognized(serialBuffer);
                        }
                        
                    } else {
                        serialCommandUnrecognized(serialBuffer);                        
                    } 
                    
                } else {
                    serialCommandUnrecognized(serialBuffer);                    
                }
                
            }
            
        }
        
    }
    
    emptyBuffer();
    serialInterpreterWorking = false;
}

void serialCommandUnrecognized(char *wat) {
    Serial.print(F("Unrecognized command: '"));
    Serial.print(wat);
    Serial.println(F("'"));
}

void emptyBuffer() {
    serialBuffer[0] = '\0';
    serialBufferIndex = 0;
    //Serial.println(F("Buffer is empty."));
}

int sanitizeServoValue(int defaultValue, byte axis) {
    int newVal = defaultValue;
    
    digitalWrite(LASER_PIN, HIGH);
    
    if (strcmp(&(serialBuffer[12]), "read") == 0) {
        boolean valueAcquired = false;
        emptyBuffer();
        
        Serial.println(F("Use the knob to calibrate the servo. Press space to finish, anything else to cancel."));
        
        while (!valueAcquired) {
            if (serialBufferIndex < 1) {
                newVal = analogRead(KNOB_PIN);
                newVal = map(newVal, 0, 1023, DEFAULT_SERVO_MIN, DEFAULT_SERVO_MAX);
                if (axis == __X) {
                    servoX.writeMicroseconds(newVal);
                    
                } else if (axis == __Y) {
                    servoY.writeMicroseconds(newVal);
                    
                }
                
                delay(40);
                checkSerial();
                
            } else {
                digitalWrite(LASER_PIN, LOW);
                valueAcquired = true;
                if (serialBuffer[0] == ' ') {
                    Serial.print(F("New value set to "));
                    Serial.println(newVal);
                    
                } else {
                    Serial.print(F("Canceled. Value remains "));
                    Serial.println(newVal);
                    newVal = defaultValue;
                    
                }
                
            }
            
        }
        
    } else {
        newVal = atoi(&(serialBuffer[12]));
        if (DEFAULT_SERVO_MIN > newVal || newVal > DEFAULT_SERVO_MAX) {
            newVal = defaultValue; // Failed to get a valid number
            Serial.print(F("Canceled. Value remains "));
            Serial.println(newVal);
            
        } else {
            Serial.print(F("New value set to "));
            Serial.println(newVal);
            
        }
        
    }
    
    if (axis == __X) {
        servoX.writeMicroseconds(newVal);
    } else if (axis == __Y) {
        servoY.writeMicroseconds(newVal);
    }
    
    delay(1000);
    digitalWrite(LASER_PIN, LOW);
    return newVal;
}

void setDrawingParameters() {
    
    drawingSize = sizeof(drawing) / 6; // int + int + int
    Serial.print(F("Points: "));
    Serial.println(drawingSize);

    for (int point = 1; point < drawingSize; point++) { // note: point starts on 1 because [0] is {0,0,0} always
        // X boundaries
        if (drawing[point][__X] < drawingBoundaries[0][0]) { // Xmin
            drawingBoundaries[0][0] = drawing[point][__X];
        }
        
        if (drawing[point][__X] > drawingBoundaries[0][1]) { // Xmax
            drawingBoundaries[0][1] = drawing[point][__X];
        }
        
        // Y boundaries
        if (drawing[point][__Y] < drawingBoundaries[1][0]) { // Ymin
            drawingBoundaries[1][0] = drawing[point][__Y];
        }
        
        if (drawing[point][__Y] > drawingBoundaries[1][1]) { // Ymax
            drawingBoundaries[1][1] = drawing[point][__Y];
        }
        
    }
    
    Serial.println(F("Boundaries:"));
    Serial.print(F("Xmin: ")); Serial.println(drawingBoundaries[0][0]); 
    Serial.print(F("Xmin: ")); Serial.println(drawingBoundaries[0][1]); 
    Serial.print(F("Ymin: ")); Serial.println(drawingBoundaries[1][0]); 
    Serial.print(F("Ymax: ")); Serial.println(drawingBoundaries[1][1]); 
    
}

void draw() {
    isDrawing = true;
    
    int X, Y;
    float ratioX, ratioY;
    
    Serial.print(F("Drawing"));
    
    ratioX = (float) (servoXMax - servoXMin) / (drawingBoundaries[0][1] - drawingBoundaries[0][0]);
    ratioY = (float) (servoYMax - servoYMin) / (drawingBoundaries[1][1] - drawingBoundaries[1][0]);
        
    for (int step = 0; step < drawingSize; step++) {
        
        if (Serial.available()) {
            checkSerial();
        }
        
        if (stopDrawing) {
            digitalWrite(LASER_PIN, LOW);
            stopDrawing = false;
            Serial.println(F("Interrumpted by command"));
            break;
        }
        
        // Sets the laser on/off before moving
        digitalWrite(LASER_PIN, drawing[step][__LASER]);
        delay(waitDelay);

        // Maps coordinates to the canvas space
        X = (int) servoXMin + (drawing[step][__X] * ratioX);
        Y = (int) servoYMin + (drawing[step][__Y] * ratioY);

        // Move to the new point
        if(drawing[step][__LASER] == LOW) {
            Serial.print(F(" "));
            moveServos(X, Y);
        } else {
            Serial.print(F("."));
            walkServos(X, Y);
        }

    }
    digitalWrite(LASER_PIN, LOW);
    Serial.println(F("Done."));
    isDrawing = false;
}

void moveServos(int x, int y) {
    servoXPos = x;
    servoYPos = y;
    // Move servos directly
    servoX.writeMicroseconds(servoXPos);
    servoY.writeMicroseconds(servoYPos);
    // Wait for the servos to reach their new point
    delay(stepDelay);
}

// Bresenham algorithm taken from here: http://members.chello.at/easyfilter/bresenham.html
void walkServos(int x, int y) {
    int dx =  abs(x - servoXPos), sx = servoXPos < x ? 1 : -1;
    int dy = -abs(servoYPos - y), sy = servoYPos < y ? 1 : -1; 
    int err = dx+dy, e2; /* error value e_xy */

    do {
        servoX.writeMicroseconds(servoXPos);
        servoY.writeMicroseconds(servoYPos);
        if (servoXPos == x && servoYPos == y) break;
        e2 = 2*err;                        
        if (e2 >= dy) {  /* e_xy+e_x > 0 */
            if (servoXPos == x) break;                       
            err += dy; 
            servoXPos += sx;                       
        }                                             
        if (e2 <= dx) {  /* e_xy+e_y < 0 */
            if (servoYPos == y) break;
            err += dx; 
            servoYPos += sy;
        }
        delay(walkDelay);
    } 
    while(1);
}

