int inByte;
int var=0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
  inByte = Serial.read();
  var = 1;
  }
  delay(5);
  if(var == 1){
  Serial.write(inByte);
  var = 0;    
  }
}
