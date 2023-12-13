// Include libraries
#include <Wire.h> 
#include <Servo.h>

// Initialize Servos
Servo servo_X;
Servo servo_Y;

// The ADXL345 sensor I2C address
int ADXL345 = 0x53; 

// Intialize variables for later use
float output_X, output_Y, output_Z;  
float angle_X, angle_Y;

// Creating a value for the amount of data in our future array
const int arrSize = 8;  

// Defining two arrays to store the angular values
int array_X[arrSize];
int array_Y[arrSize];  

// Variable to keep track of the current index in the array through iterations
int index = 0;

// Final variables for output
float sumY, sumX;


void setup() {

  // Attach the servos to pins
  servo_X.attach(2);
  servo_Y.attach(3);

  // Initiate serial communication for printing the results on the Serial monitor
  Serial.begin(9600); 

  // Initiate the Wire library
  Wire.begin(); 

  // Setup the wire communication
  Wire.beginTransmission(ADXL345); 
  Wire.write(0x2D); 
  Wire.write(8);
  Wire.endTransmission();

  // Reset the servos to default on startup
  updateServo(90, 90);

  delay(10);
}


// Main loop that runs all the functions
void loop() {
    read_angle();

    // Calculates the average Y angle
    sumY = 0;
    for(int i = 0; i < 8; i++) {
        sumY += array_Y[i];
    }


    // Calculates the average X angle
    sumX = 0;
    for(int i = 0; i < 8; i++) {
        sumX += array_X[i];
    }

  // Updates the Servo accordingly
  updateServo(sumX/8, sumY/8);
}

// Function to constantly update the average estimated angles
void read_angle() {
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32);
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true);
  output_X = ( Wire.read() | Wire.read() << 8); // X-axis value
  output_X = output_X / 256;
  output_Y = ( Wire.read() | Wire.read() << 8); // Y-axis value
  output_Y = output_Y / 256;
  output_Z = ( Wire.read() | Wire.read() << 8); // Z-axis value
  output_Z = output_Z / 256;


  // Calculate the angles based on the accel data
  angle_X = atan(output_X / output_Z) * (180 / 3.1415);
  angle_Y = atan(output_Y / output_Z) * (180 / 3.1415);

  // Add the angles to our arrays for futher calculations
  addToArray();
}

// Function to control the servo's rotations based on the current angle
void updateServo(int x, int y) {
  if (output_Z > 0){
    servo_X.write(x + 90);
    servo_Y.write(y + 90);
  }
}

// Function that calculates the average required angle
// Used to create a less wobbly product/more stable
void addToArray() {

  // Add the new value to the array
  array_Y[index] = angle_Y;
  array_X[index] = angle_X;

  // Move to the next index
  index = (index + 1) % arrSize;
}
