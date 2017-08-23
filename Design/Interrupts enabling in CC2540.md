

#GENERAL-URPOSE I/O INTERRUPTS ENABLING FOR CC2540

>**Abstract**
>
+ Setup your GPIO as input, select the triggering edge and enable the IRQs for the given port.
+ Implement an Interrupt-handler for the vector of your port (see end of hal_key.c, copy the handler from hal_key.c for best results as the called macros are important).
+ If you want to process the IRQ-event in the main-task instead of the IRQ-handler raise an event by calling osal_set_event()
don't forget to clear the IRQ-pending flags in the handler.



The SFR registers used for interrupts  are summarized as follows:

| SFR Register | Description |
| ------------|------------|
|P0IEN | Port 0 interrupt-mask register (interrupt enables)|
|P1IEN | Port 1 interrupt-mask register (interrupt enables)|
|P2IEN | Port 2 interrupt-mask register (interrupt enables)|
|P0IFG | Port 0 interrupt-status flag register|
|P1IFG | Port 1 interrupt-status flag register|
|P2IFG | Port 2 interrupt-status flag register|
|PICTL | Interrupt edge register|
|IEN2.P1IE | IEN2 bit 4 General port 1 interrupt enable |
|IEN0.EA | IEN0 bit 7, Disables all interrupts |


---
##INTERRUPT ENABLING AND PORT CONFGURATION
**1. Clear interrupt flags.**

    PxIFG=0;   
    PxIF=0;    //  PxIF is Px interrupt flag
    //PxIFG has to be cleared before PxIF


**2. Set individual interrupt-enable bit in the peripherals SFR register, if any.**

    P1IEN |= PUSH1_BV | PUSH2_BV;  // Enable specific P1 bits for ints by bit mask.


**3. Set the corresponding individual interrupt-enable bit in the IEN0, IEN1, or IEN2 register to 1.**

    IEN2  |= 0x10;      // Enable general P1 interrupts.

**4. Enable global interrupt by setting the EA bit in IEN0 to 1.**

    IEN0 |= 0x80;       //Equal as EA |= 1.

**5. Configure triggerin by rising or falling edge.**

    PICTL |= 0x00;      // P1ICONL: Rising edge ints on pins 0-3 and port 1.

**5. Begin the interrupt service routine at the corresponding vector address of that interrupt. See Table 2-5for addresses.**



    HAL_ISR_FUNCTION( halKeyPort0Isr, P0INT_VECTOR )
    {
      HAL_ENTER_ISR();
    #if defined ( CC2540_MINIDK )
      if ((HAL_KEY_SW_1_PXIFG & HAL_KEY_SW_1_BIT) || (HAL_KEY_SW_2_PXIFG & HAL_KEY_SW_2_BIT))
    #else
      if (HAL_KEY_SW_6_PXIFG & HAL_KEY_SW_6_BIT)
    #endif
      {
        halProcessKeyInterrupt();
      }
      /*
        Clear the CPU interrupt flag for Port_0
        PxIFG has to be cleared before PxIF
      */
      HAL_KEY_SW_6_PXIFG = 0;
      HAL_KEY_CPU_PORT_0_IF = 0;
      CLEAR_SLEEP_MODE();
      HAL_EXIT_ISR();
      return;
    }

   

