

Here's a code example that sets P0.2 high:
P0SEL &= ~BV(2);  // set P0.2 as GPIO pin
P0DIR |=  BV(2);  // set P0.2 as output
P0    |=  BV(2);  // set output high


P0.2 as input:
P0SEL &= ~BV(7);  // set P0.7 as GPIO pin 
P0DIR &= ~BV(7);  // set P0.7 as input 


Logical operators:
P0    |=  BV(2); => OR logical operation, to set to logic '1' only the bit designed by BV(X)
P0SEL &= ~BV(2); => NAND logical operation, to set to logic '0' only the bit designed by BV(X)