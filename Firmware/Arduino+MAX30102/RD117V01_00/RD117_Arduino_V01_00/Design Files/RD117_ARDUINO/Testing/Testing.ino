
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
}

void loop() {
  char buffer[30];
   char x[3] = "Hi";
   char *y;
   int z;
   sprintf(buffer, "with %%p:  x    = %p\n", x);
   Serial.print(buffer);
   sprintf(buffer, "with %%p: &x[0] = %p\n", &x[0]);
   Serial.print(buffer);

   y = &x[0]; // Same as writing y = x;
   z = (unsigned int)y;
   Serial.print("sizeof(unsigned int) = ");Serial.print(sizeof(unsigned int));
   Serial.print(", sizeof(char *) = "); Serial.println(sizeof(char *));
   Serial.print("(unsigned int)y     = 0x");Serial.println(uint16_t(y),HEX);
   Serial.print("(unsigned int)x     = 0x");Serial.println((unsigned int)x,HEX);
   Serial.print("(unsigned int)&x[0] = 0x");Serial.println((unsigned int)&x[0],HEX);
   Serial.print(" z = 0x");Serial.println(z,HEX);
   Serial.print("&z = 0x");Serial.println((unsigned int)&z, HEX);

   Serial.println();
   delay(10000);
}
