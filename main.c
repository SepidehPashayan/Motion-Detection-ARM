/*
 * Motion Detection and Alarm System
 * Target: ARMCM3 (Generic Cortex-M3)
 * IDE: Keil MDK
 * Author: Sepideh Pashayan & Atieh Jafarjadeh
 * Course: Microprocessor and Assembly Language
 */

#include <stdint.h>
#include "ARMCM3.h"

/* ===== Register Addresses ===== */
#define GPIOA_BASE   0x40010800UL
#define GPIOC_BASE   0x40011000UL
#define RCC_BASE     0x40021000UL

#define RCC_APB2ENR  (*((volatile uint32_t *)(RCC_BASE   + 0x18)))
#define GPIOA_CRL    (*((volatile uint32_t *)(GPIOA_BASE + 0x00)))
#define GPIOA_IDR    (*((volatile uint32_t *)(GPIOA_BASE + 0x08)))
#define GPIOA_ODR    (*((volatile uint32_t *)(GPIOA_BASE + 0x0C)))
#define GPIOC_CRH    (*((volatile uint32_t *)(GPIOC_BASE + 0x04)))
#define GPIOC_ODR    (*((volatile uint32_t *)(GPIOC_BASE + 0x0C)))

/* ===== Pin Definitions ===== */
#define PIR_PIN         0    /* PA0 - input  */
#define LED_GREEN_PIN   1    /* PA1 - output */
#define BUZZER_PIN      2    /* PA2 - output */
#define LED_RED_PIN     13   /* PC13 - output */

/* ===== Constants ===== */
#define ALARM_DURATION_MS  3000
#define SYSTICK_1MS        8000   /* 8MHz / 1000 */

/* ===== Global Variables (shared with Assembly) ===== */
volatile uint8_t motion_detected = 0;
volatile uint8_t alarm_active    = 0;
volatile uint32_t alarm_timer    = 0;
volatile uint32_t sys_tick_ms    = 0;

/* ===== Assembly function declarations ===== */
extern uint32_t ASM_GPIO_ReadPin(uint32_t pin);
extern void     ASM_GPIO_WritePin(volatile uint32_t *odr_addr, uint32_t mask, uint32_t value);
extern void     ASM_Delay_Loop(uint32_t count);
extern uint32_t ASM_CheckMotion(void);

/* ===== Function Prototypes ===== */
void GPIO_Init(void);
void SysTick_Init(void);
void Activate_Alarm(void);
void Deactivate_Alarm(void);
void Delay_ms(uint32_t ms);

/* ===== Main Program ===== */
int main(void)
{
    GPIO_Init();
    SysTick_Init();

    /* System idle: green LED on */
    GPIOA_ODR |=  (1 << LED_GREEN_PIN);
    GPIOA_ODR &= ~(1 << BUZZER_PIN);
    GPIOC_ODR &= ~(1 << LED_RED_PIN);

    while (1)
    {
        /* Call Assembly function to check PIR */
        ASM_CheckMotion();

        if (motion_detected && !alarm_active)
            Activate_Alarm();

        if (alarm_active && alarm_timer == 0)
            Deactivate_Alarm();
    }
}

/* ===== Activate Alarm ===== */
void Activate_Alarm(void)
{
    alarm_active = 1;
    alarm_timer  = ALARM_DURATION_MS;

    GPIOC_ODR |=  (1 << LED_RED_PIN);    /* Red LED ON  */
    GPIOA_ODR &= ~(1 << LED_GREEN_PIN);  /* Green LED OFF */
    GPIOA_ODR |=  (1 << BUZZER_PIN);     /* Buzzer ON */
}

/* ===== Deactivate Alarm ===== */
void Deactivate_Alarm(void)
{
    alarm_active    = 0;
    motion_detected = 0;

    GPIOC_ODR &= ~(1 << LED_RED_PIN);   /* Red LED OFF */
    GPIOA_ODR &= ~(1 << BUZZER_PIN);    /* Buzzer OFF  */
    GPIOA_ODR |=  (1 << LED_GREEN_PIN); /* Green LED ON */
}

/* ===== GPIO Initialization ===== */
void GPIO_Init(void)
{
    /* Enable clocks: GPIOA, GPIOC */
    RCC_APB2ENR |= (1 << 2) | (1 << 4);

    /* PA0: Input floating (PIR) */
    GPIOA_CRL &= ~(0xF << 0);
    GPIOA_CRL |=  (0x4 << 0);

    /* PA1, PA2: Output push-pull 2MHz */
    GPIOA_CRL &= ~(0xFF << 4);
    GPIOA_CRL |=  (0x22 << 4);

    /* PC13: Output push-pull 2MHz */
    GPIOC_CRH &= ~(0xF << 20);
    GPIOC_CRH |=  (0x2 << 20);
}

/* ===== SysTick Initialization (1ms) ===== */
void SysTick_Init(void)
{
    SysTick->LOAD = SYSTICK_1MS - 1;
    SysTick->VAL  = 0;
    SysTick->CTRL = 0x07; /* Enable, interrupt, use processor clock */
}

/* ===== SysTick Handler ===== */
void SysTick_Handler(void)
{
    sys_tick_ms++;
    if (alarm_active && alarm_timer > 0)
        alarm_timer--;
}

/* ===== Simple Delay ===== */
void Delay_ms(uint32_t ms)
{
    uint32_t start = sys_tick_ms;
    while ((sys_tick_ms - start) < ms);
}
