void setup() {
  Serial2.begin(9600);
}

void loop() {
  Serial2.println("OK");
  delay(300);
}
