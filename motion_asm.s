; ============================================================
; Motion Detection System - ARM Cortex-M3 Assembly Module
; Keil MDK / ARM Compiler 6 (AC6)
; Author: Sepideh Pashayan
; ============================================================

                AREA    MYDATA, DATA, READWRITE
pir_prev_state  DCB     0
                ALIGN

                AREA    MYCODE, CODE, READONLY
                THUMB

GPIOA_IDR       EQU     0x40010808
GPIOA_ODR       EQU     0x4001080C
GPIOC_ODR       EQU     0x4001100C
EXTI_PR         EQU     0x40010414

                EXPORT  ASM_GPIO_ReadPin
                EXPORT  ASM_GPIO_WritePin
                EXPORT  ASM_Delay_Loop
                EXPORT  ASM_CheckMotion
                IMPORT  motion_detected

; ============================================================
; ASM_GPIO_ReadPin
;   R0 = pin number (0-15)
;   Returns R0 = 0 or 1
; ============================================================
ASM_GPIO_ReadPin PROC
                PUSH    {R1, R2, LR}
                LDR     R1, =GPIOA_IDR
                LDR     R2, [R1]
                MOV     R1, #1
                LSL     R1, R1, R0
                AND     R0, R2, R1
                CMP     R0, #0
                ITE     NE
                MOVNE   R0, #1
                MOVEQ   R0, #0
                POP     {R1, R2, PC}
                ENDP

; ============================================================
; ASM_GPIO_WritePin
;   R0 = ODR address, R1 = pin bitmask, R2 = value (0/1)
; ============================================================
ASM_GPIO_WritePin PROC
                PUSH    {R3, R4, LR}
                LDR     R3, [R0]
                CMP     R2, #1
                BEQ     set_pin
clear_pin
                MVN     R4, R1
                AND     R3, R3, R4
                B       write_odr
set_pin
                ORR     R3, R3, R1
write_odr
                STR     R3, [R0]
                POP     {R3, R4, PC}
                ENDP

; ============================================================
; ASM_Delay_Loop
;   R0 = loop count
; ============================================================
ASM_Delay_Loop  PROC
                PUSH    {LR}
delay_loop
                SUBS    R0, R0, #1
                BNE     delay_loop
                POP     {PC}
                ENDP

; ============================================================
; ASM_CheckMotion
;   Returns R0 = 1 if motion detected, 0 if not
; ============================================================
ASM_CheckMotion PROC
                PUSH    {R1, R2, R3, LR}
                MOV     R0, #0
                BL      ASM_GPIO_ReadPin
                CMP     R0, #1
                BNE     no_motion
                LDR     R0, =240000
                BL      ASM_Delay_Loop
                MOV     R0, #0
                BL      ASM_GPIO_ReadPin
                CMP     R0, #1
                BNE     no_motion
motion_ok
                LDR     R1, =motion_detected
                MOV     R2, #1
                STRB    R2, [R1]
                MOV     R0, #1
                B       check_done
no_motion
                LDR     R1, =motion_detected
                MOV     R2, #0
                STRB    R2, [R1]
                MOV     R0, #0
check_done
                POP     {R1, R2, R3, PC}
                ENDP

                END
