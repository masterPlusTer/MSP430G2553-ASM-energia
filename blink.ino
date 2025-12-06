#define P1DIR  (*(volatile unsigned char *)0x0021)
#define P1OUT  (*(volatile unsigned char *)0x0022)

void setup() {
  asm volatile (
    "bis.b #0x41, %0 \n\t"    // P1DIR |= 0x41 (BIT6+BIT0)
    "bic.b #0x41, %1 \n\t"    // P1OUT &= ~0x41 (ambos OFF)
    : "=m" (P1DIR), "=m" (P1OUT)
    :
    : "memory"
  );
}

void delayAsm() {
  asm volatile (
    "mov.w #400, r14\n\t"
    "outer: mov.w #4000, r15\n\t"
    "inner: dec.w r15\n\t"
    "jnz inner\n\t"
    "dec.w r14\n\t"
    "jnz outer\n\t"
    ::: "r14", "r15", "cc"
  );
}

void loop() {
  asm volatile (
    "bis.b #0x40, %0 \n\t"    // P1OUT |= BIT6 (GREEN ON)
    "bic.b #0x01, %0 \n\t"    // P1OUT &= ~BIT0 (RED OFF)
    : "=m" (P1OUT)
    :
    : "memory"
  );
  delayAsm();
  
  asm volatile (
    "bic.b #0x40, %0 \n\t"    // P1OUT &= ~BIT6 (GREEN OFF)
    "bis.b #0x01, %0 \n\t"    // P1OUT |= BIT0 (RED ON)
    : "=m" (P1OUT)
    :
    : "memory"
  );
  delayAsm();
}
