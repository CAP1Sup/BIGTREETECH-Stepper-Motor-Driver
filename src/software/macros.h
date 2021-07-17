// Only include once
#ifndef __MACROS_H__
#define __MACROS_H__

// Checking functions
#define IS_POWER_2(N) ((N) & ((N)-1)) // Return 0 if a number is a power of 2.

// Power of 2, N is positive integer
#define POWER_2(N) (1U << (N))

// Extend the motor steps range to the encoder increment range
#define MOTOR_STEP_TO_ENCODER_INCREMENT(steps, stepsPerRev, microstepDivisor) ( (steps) * ((POW_2_15) >> (REJECT_ENCODERS_LSB)) / ((stepsPerRev) * (microstepDivisor)) )

// Bitwise memory modification - ARM bitband
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))

// Low level GPIO configuration (for quicker manipulations than digitalWrites)
#define outputPin(GPIO_BASE, n)   BIT_ADDR((GPIO_BASE + 12), n) // ODR
#define inputPin(GPIO_BASE, n)    BIT_ADDR((GPIO_BASE + 8), n) // IDR

// Maybe an even faster digitalWrite, but only if really needed
#define digitalWriteFaster(pn, high) \
    if (high) { \
        WRITE_REG(get_GPIO_Port(STM_PORT(pn))->BSRR, (STM_LL_GPIO_PIN(pn) >> GPIO_PIN_MASK_POS) & 0x0000FFFFU); \
    } \
    else { \
        WRITE_REG(get_GPIO_Port(STM_PORT(pn))->BRR, (STM_LL_GPIO_PIN(pn) >> GPIO_PIN_MASK_POS) & 0x0000FFFFU); \
    }

#define digitalWriteFastest(pn, high) \
    if (high) { \
        get_GPIO_Port(STM_PORT(pn))->BSRR = STM_GPIO_PIN(pn); \
    } \
    else { \
        get_GPIO_Port(STM_PORT(pn))->BRR = STM_GPIO_PIN(pn); \
    }

// Direction (for reverse direction)
#define DIRECTION(x) ((x) > 0 ? 1 : (-1))  // Note: zero 'x' is not allowed

// Return angle in range [0..360[ (doesn't include 360)
#define TO360(A) ((A) - DIRECTION(A) * round(abs(A) / 360) * 360)

// Convert DPS(degrees per second) to RPM(rotations per minute)
#define DPS_TO_RPM(DPS) ((DPS) * 60 / 360)

// Table for GPIO_WRITE methods
#define GPIO_WRITE_DIGITAL_WRITE         0
#define GPIO_WRITE_DIGITAL_WRITE_FAST    1
#define GPIO_WRITE_DIGITAL_WRITE_FASTER  2
#define GPIO_WRITE_DIGITAL_WRITE_FASTEST 3
#define GPIO_WRITE_REGISTER_SET          4
#define GPIO_WRITE_HAL_FUNCTION          5


#endif // ! __MACROS_H__