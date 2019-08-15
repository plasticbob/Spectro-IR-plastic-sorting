//    By Mej
//    2019/07/16
// 
//    Direct drive of stepper motor. Full step sequence. Both ways. fixe delay between subteps.


//Pin definition
int LED = 3;
int LED_minus = 2;
int A_p = A0;
int A_m = A1;
int B_p = A2;
int B_m = A3;

//timing and scan range
unsigned int D = 1000; //Delay between sub-steps in µs NB: 1 step = 4 sub-steps
                      //Start missing under steps at 800µs/sub-step
int long loop_count = 0;
int steps_per_cycle=200; //steps of a full forward/backward cycle. Motor swich direction at steps_per_cycle/2
                         //full mechanical range: 10mm ~150steps = 600 sub-steps  = 3s      @ 5000 µs/sub-step
                         //                                                       = 600ms   @ 1000 µs/sub-step

void step_none()
{
  digitalWrite(A_p, LOW);
  digitalWrite(A_m, LOW);
  digitalWrite(B_p, LOW);
  digitalWrite(B_m, LOW);
}

void wait(unsigned int d)
{
  delayMicroseconds(d);
//  delayMicroseconds(d/2);
//  step_none();
//  delayMicroseconds(d/2);
}
  
void step_forward(unsigned int d)
{
  //step 11 cf http://www.edaboard.com/thread217270.html
  digitalWrite(A_p, HIGH);
  digitalWrite(A_m, LOW);
  digitalWrite(B_p, HIGH);
  digitalWrite(B_m, LOW);
  wait(d);
  //step 10
  digitalWrite(A_p, HIGH);
  digitalWrite(A_m, LOW);
  digitalWrite(B_p, LOW);
  digitalWrite(B_m, HIGH);
  wait(d);
  //step 00
  digitalWrite(A_p, LOW);
  digitalWrite(A_m, HIGH);
  digitalWrite(B_p, LOW);
  digitalWrite(B_m, HIGH);
  wait(d);
  //step 01
  digitalWrite(A_p, LOW);
  digitalWrite(A_m, HIGH);
  digitalWrite(B_p, HIGH);
  digitalWrite(B_m, LOW);
  wait(d);
}

void step_backward(unsigned int d)
{
  //step 01
  digitalWrite(A_p, LOW);
  digitalWrite(A_m, HIGH);
  digitalWrite(B_p, HIGH);
  digitalWrite(B_m, LOW);
  wait(d);
  //step 00
  digitalWrite(A_p, LOW);
  digitalWrite(A_m, HIGH);
  digitalWrite(B_p, LOW);
  digitalWrite(B_m, HIGH);
  wait(d);
  //step 10
  digitalWrite(A_p, HIGH);
  digitalWrite(A_m, LOW);
  digitalWrite(B_p, LOW);
  digitalWrite(B_m, HIGH);
  wait(d);
  //step 11
  digitalWrite(A_p, HIGH);
  digitalWrite(A_m, LOW);
  digitalWrite(B_p, HIGH);
  digitalWrite(B_m, LOW);
  wait(d);
}





void setup() {
// GPIO set up

 pinMode(LED, OUTPUT);
 pinMode(LED_minus, OUTPUT);
 pinMode(A_p, OUTPUT);
 pinMode(A_m, OUTPUT);
 pinMode(B_p, OUTPUT);
 pinMode(B_m, OUTPUT);
 
 digitalWrite(LED, LOW);
 digitalWrite(LED_minus, LOW);
 digitalWrite(A_p, LOW);
 digitalWrite(A_m, LOW);
 digitalWrite(B_p, LOW);
 digitalWrite(B_m, LOW);

// Mechanical set up

 for(int i=0; i<150; i++){
  step_forward(5000);}
step_none();
delay(200);
 for(int i=0; i<75; i++){
  step_backward(5000);}
step_none();
delay(200);
 for(int i=0; i<steps_per_cycle/4; i++){
  step_backward(5000);}
step_none();
delay(200);
  
}

void loop() {

  loop_count = loop_count + 1;

  //forward
  digitalWrite(LED, HIGH);
  digitalWrite(LED_minus, LOW);
  for(int i=0;i<steps_per_cycle/2;i++){
    step_forward(D);}

  step_none();
  delay(200);
  
  //reverse
  digitalWrite(LED, LOW);
  digitalWrite(LED_minus, HIGH);
  for(int i=0;i<steps_per_cycle/2;i++){
    step_backward(D);}

  step_none();
  delay(200);

}
