// Define number of ultrasonic sensors
const int NUM_SENSORS = 5;
int distance[NUM_SENSORS];
// Define Echo pins array
const int ECHO_PIN[NUM_SENSORS] = {A1, A2, A3, A4, A5};
// Define common Trigger pin for all ultrasonic sensors
#define latchpin 7
#define clockpin 6
#define datapin 8
// Define LED pins
#define LLED_PIN 2
#define RLED_PIN 3
#define RirLED_PIN 13
#define LirLED_PIN 5
// Define maximum distance limit for adaptive cruise control
#define MAX_DISTANCE 200
// Define motor control pins
#define ENA 9
#define ENB 10
#define IRR 11
#define IRL 12
// Define motor speed variable
int motorSpeed1 ;
int motorSpeed2 ;
int motorSpeed3 ;
int motorSpeed4 ;
long duration;
long distances;
// Define distance for the blind spot detection
const int BLIND_SPOT_DISTANCE = 20;
void setup() {
Serial.begin(9600);
// Initialize motor control pin as output
TCCR1B = TCCR1B & B1111000 | B00000100;
pinMode(ENA, OUTPUT);
pinMode(ENB, OUTPUT);
pinMode(ECHO_PIN[0],INPUT);
53
pinMode(ECHO_PIN[1],INPUT);
pinMode(ECHO_PIN[2],INPUT);
pinMode(ECHO_PIN[3],INPUT);
pinMode(ECHO_PIN[4],INPUT);
// Set shift register pins as outputs
pinMode(latchpin, OUTPUT);
pinMode(datapin, OUTPUT);
pinMode(clockpin, OUTPUT);
//Set IR pins for input
pinMode(IRR, INPUT);
pinMode(IRL, INPUT);
pinMode(RirLED_PIN,OUTPUT);
pinMode(LirLED_PIN,OUTPUT);
pinMode(RLED_PIN,OUTPUT);
pinMode(LLED_PIN,OUTPUT);
}
void loop() {
measuredistances();
BlindSpot();
if( distance[1]>20 && distance[2] > 20 && distance[3] > 20 && distance[4] > 20 ){
AdCrcontrol();
laneDepart();
}
else {
accidentPrevention();
laneDepart();
}
}
void measuredistances()
{
for (int i = 0; i < NUM_SENSORS; i++) {
// Trigger ultrasonic sensor
digitalWrite(latchpin, LOW); // Pull latch low to start sending data
shiftOut(datapin, clockpin, MSBFIRST, 0); // Send data to trigger the ith
ultrasonic sensor
digitalWrite(latchpin, HIGH);
delay(2);
digitalWrite(latchpin, LOW); // Pull latch low to start sending data
shiftOut(datapin, clockpin, MSBFIRST, 1 << i); // Send data to trigger the ith
ultrasonic sensor
digitalWrite(latchpin, HIGH);
delay(10);
digitalWrite(latchpin, LOW); // Pull latch low to start sending data
shiftOut(datapin, clockpin, MSBFIRST, 0); // Send data to trigger the ith
ultrasonic sensor
digitalWrite(latchpin, HIGH); /// Pull latch high to store data
54
// Delay to ensure sensor has time to respond
// Measure the echo duration
duration = pulseIn(ECHO_PIN[i], HIGH);
// Calculate distance based on the speed of sound
distance[i] = duration * 0.034 / 2;
// Print distance to serial monitor
Serial.print("Distance ");
Serial.print(i + 1);
Serial.print(": ");
Serial.print(distance[i]);
Serial.println(" cm");
motorSpeed1 = map(distance[0],10,100,70,255);
motorSpeed3 = map(distance[0],10,100,92,255);
delay(10);
}
}
// function for blind spot detection
void BlindSpot(){
// Check for blind spot and turn LEDs on/off accordingly
if (distance[3] < BLIND_SPOT_DISTANCE || distance[4] < BLIND_SPOT_DISTANCE) {
digitalWrite(LLED_PIN, HIGH); // Turn left back LED on
}
else {
digitalWrite(LLED_PIN, LOW); // Turn left back LED off
}
if (distance[2] < BLIND_SPOT_DISTANCE || distance[1] < BLIND_SPOT_DISTANCE) {
digitalWrite(RLED_PIN, HIGH); // Turn right back LED on
}
else {
digitalWrite(RLED_PIN, LOW); // Turn right back LED off
}
// Delay before next iteration
}
void laneDepart(){
int a = digitalRead(IRR);
int b = digitalRead(IRL);
if (a == HIGH) //blink right indicator when moving to right lane
{
Serial.println("car Leaving to right lane.");
digitalWrite(RirLED_PIN, HIGH);
}
else{
digitalWrite(RirLED_PIN, LOW);
}
55
if(b == HIGH) // blink left indicator when moving to left lane
{
Serial.println("car leaving to left lane.");
digitalWrite(LirLED_PIN, HIGH);
}
else{
digitalWrite(LirLED_PIN, LOW);
}
}
// function for Adaptive Cruise Control
void AdCrcontrol()
{
motorSpeed1 = map(distance[0],10,100,70,255);
motorSpeed3 = map(distance[0],10,100,92,255);
if (distance[0]> 100) // car runs at maximum permeasible speed limit when no
object in range
{
digitalWrite(ENA, HIGH);
digitalWrite(ENB, HIGH);
}
// Control motor speed using PWM
else if(distance[0]>=10 && distance[0]<=100){
analogWrite(ENA, motorSpeed1);
analogWrite(ENB, motorSpeed3);
}
else if(distance[0]<10 && distance[0]>5){
if(distance[1]<20 && distance[4]<20) //check for object in front blindspots
{
digitalWrite(ENA, LOW);
digitalWrite(ENB, LOW);
}
else if(distance[4]<30){
analogWrite(ENB, 175);
digitalWrite(ENA, LOW);
delay(600);
analogWrite(ENA, 150);
analogWrite(ENB, 175);
delay(900);
analogWrite(ENA, 150);
digitalWrite(ENB, LOW);
laneDepart();
delay(600);// turn vehicle left
}
else if (distance[1]<30 ){
digitalWrite(ENB, LOW);
analogWrite(ENA, 150);
delay(600);
analogWrite(ENA, 150);
analogWrite(ENB, 175);
56
delay(900);
digitalWrite(ENA, LOW);
analogWrite(ENB, 175);
laneDepart();
delay(600);//turn vehicle right
}
else{
digitalWrite(ENB, LOW);
analogWrite(ENA, 150);
delay(600);
analogWrite(ENA, 150);
analogWrite(ENB, 175);
delay(900);
digitalWrite(ENA, LOW);
analogWrite(ENB, 175);
laneDepart();
delay(600);// turn vehicle left
}
}
else if(distance[0]<5){
digitalWrite(ENA,LOW);
digitalWrite(ENB,LOW);
}
// Add a short delay for stability
delayMicroseconds(10);
}
//accident prevention function if a car comes too close in blind spot
void accidentPrevention()
{
if(distance[0] > 50.
)
{
if(distance[1] >= 20 && distance[4] >= 20)
{
if(distance[2] <= 20 || distance[3] <= 20)
{
if(distance[2]<distance[3]){
motorSpeed2 = map(distance[2],0,20,255,motorSpeed1);
motorSpeed4 = map(distance[2],0,20,255,motorSpeed3);
analogWrite(ENA, motorSpeed2);
analogWrite(ENB, motorSpeed4); //make car run faster when no object in
front
}
else if (distance[2]>distance[3]){
motorSpeed2 = map(distance[3],0,20,255,motorSpeed1);
motorSpeed4 = map(distance[3],0,20,255,motorSpeed3);
analogWrite(ENA, motorSpeed2);
57
analogWrite(ENB, motorSpeed4); //make car run faster when no object in
front
}
else if (distance[2]=distance[3])
{
motorSpeed2 = map(distance[2],0,20,255,motorSpeed1);
motorSpeed4 = map(distance[2],0,20,255,motorSpeed3);
analogWrite(ENA, motorSpeed2);
analogWrite(ENB, motorSpeed4); //make car run faster when no object in
front
}
}
else
{
loop();
}
}
else if (distance[1] <= 20)
{
digitalWrite(ENA, LOW);
digitalWrite(ENB, LOW); //Stop the vehicle
}
else if (distance[4] <= 20)
{
digitalWrite(ENA, LOW);
digitalWrite(ENB, LOW); //Stop the vehicle
}
}
else if(distance[0] < 50) // check for object in front
{
if (distance[1] > 20 && distance[4] > 20) //check for objects in front blinspots
{
if(distance[2] <= 20 && distance[3] <= 20)
{
digitalWrite(ENA, HIGH);
digitalWrite(ENB, HIGH);
}
else if(distance[3] <= 20)
{
digitalWrite(ENB, LOW);
analogWrite(ENA,150);
delay(600);
analogWrite(ENA, 150);
analogWrite(ENB, 175);
delay(900);
digitalWrite(ENA, LOW);
analogWrite(ENB, 175);
laneDepart();
delay(600);//turn vehicle right
}
58
else if(distance[2]<=20)
{
analogWrite(ENB, 175);
digitalWrite(ENA, LOW);
delay(600);
analogWrite(ENA, 150);
analogWrite(ENB, 175);
delay(900);
analogWrite(ENA, 150);
digitalWrite(ENB, LOW);
laneDepart();
delay(600);// turn vehicle left
}
}
else if (distance[1] <= 20)
{
digitalWrite(ENA, LOW);
digitalWrite(ENB, LOW); //Stop the vehicle
}
else if (distance[4] <= 20)
{
digitalWrite(ENA, LOW);
digitalWrite(ENB, LOW); //Stop the vehicle
}
else {
digitalWrite(ENA, LOW);
digitalWrite(ENB, LOW); //Stop the vehicle
}
}
}
