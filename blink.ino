// Define P1DIR (0x0021): Registro dirección Puerto 1 (1=config salida, 0=entrada)
#define P1DIR  (*(volatile unsigned char *)0x0021)
// Define P1OUT (0x0022): Registro salida Puerto 1 (1=alto/encendido, 0=bajo/apagado)
#define P1OUT  (*(volatile unsigned char *)0x0022)

void setup() {
  asm volatile (
    // P1DIR |= 0x41 → Configura P1.6(GREEN) y P1.0(RED) como SALIDAS
    "bis.b #0x41, %0 \n\t"    
    // P1OUT &= ~0x41 → Apaga ambos LEDs al inicio (BIT6+BIT0=0)
    "bic.b #0x41, %1 \n\t"    
    // %0=P1DIR (escribe), %1=P1OUT (escribe) - Extended ASM syntax
    : "=m" (P1DIR), "=m" (P1OUT)
    // Sin inputs
    :
    // Clobber: advierte que modifica memoria (evita optimización)
    : "memory"
  );
}

void delayAsm() {
  asm volatile (
    // Carga R14=400 → Contador bucle EXTERIOR (400 iteraciones)
    "mov.w #400, r14\n\t"
    // Etiqueta 'outer:' → Inicio bucle exterior
    "outer: mov.w #4000, r15\n\t" // Carga R15=4000 → Contador bucle INTERIOR
    // Etiqueta 'inner:' → Inicio bucle interior
    "inner: dec.w r15\n\t"         // R15-- (decrementa 1)
    "jnz inner\n\t"                // Si R15≠0, salta a 'inner:' (4000 veces)
    "dec.w r14\n\t"                // R14-- (al acabar interior)
    "jnz outer\n\t"                // Si R14≠0, salta a 'outer:' (400 veces)
    // Total: 400×4000×3ciclos = ~4.8M ciclos ≈ 4.8s @1MHz
    ::: "r14", "r15", "cc"  // Clobbers: uso R14,R15 y modifico flags
  );
}

void loop() {
  // BLOQUE 1: VERDE ON (P1.6=1), ROJO OFF (P1.0=0)
  asm volatile (
    "bis.b #0x40, %0 \n\t"    // P1OUT |= 0x40 → BIT6=1 (GREEN_LED ON)
    "bic.b #0x01, %0 \n\t"    // P1OUT &= ~0x01 → BIT0=0 (RED_LED OFF)
    : "=m" (P1OUT)            // %0=P1OUT (escribe memoria)
    :                         // Sin inputs
    : "memory"                // Modifica memoria
  );
  delayAsm();                   // Espera ~4.8s
  
  // BLOQUE 2: VERDE OFF (P1.6=0), ROJO ON (P1.0=1)
  asm volatile (
    "bic.b #0x40, %0 \n\t"    // P1OUT &= ~0x40 → BIT6=0 (GREEN_LED OFF)
    "bis.b #0x01, %0 \n\t"    // P1OUT |= 0x01 → BIT0=1 (RED_LED ON)
    : "=m" (P1OUT)            // %0=P1OUT (escribe memoria)
    :                         // Sin inputs
    : "memory"                // Modifica memoria
  );
  delayAsm();                   // Espera ~4.8s → Vuelve al inicio
}

