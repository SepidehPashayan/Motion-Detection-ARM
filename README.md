# Motion Detection and Alarm System
### ARM Cortex-M3 | Keil MDK | C + ARM Thumb-2 Assembly

A bare-metal embedded system that detects human presence within a 5-meter range using a PIR sensor and triggers audio/visual alerts. Implemented on an **ARM Cortex-M3** core with critical routines written in **ARM Thumb-2 Assembly**.

---

## Features

- PIR-based human motion detection (up to 5m range)
- Red LED + buzzer alarm activation on motion detection
- Green LED idle indicator when area is clear
- 3-second auto-reset after alarm triggers
- Bluetooth alert transmission via HC-05 (UART)
- Debounced PIR reading implemented in pure Assembly
- SysTick-driven 1ms precision timer

---

## Hardware

| Component | Pin | Role |
|-----------|-----|------|
| PIR Sensor | PA0 | Motion input |
| Green LED | PA1 | Idle indicator |
| Buzzer | PA2 | Alarm output |
| Red LED | PC13 | Alarm indicator |
| Bluetooth HC-05 | PA9 / PA10 | UART alert |

---

## Project Structure

```
├── main.c            # Main system logic in C
├── motion_asm.s      # Key subroutines in ARM Assembly
├── scatter.sct       # Linker scatter file (stack/memory layout)
├── PROJECT.uvprojx   # Keil MDK project file
└── RTE/              # CMSIS runtime environment
```

---

## Assembly Subroutines

| Function | Description |
|----------|-------------|
| `ASM_GPIO_ReadPin` | Reads a GPIO input pin via Register Indirect addressing |
| `ASM_GPIO_WritePin` | Writes HIGH/LOW to a GPIO output pin |
| `ASM_Delay_Loop` | Software delay loop (CPU-cycle based) |
| `ASM_CheckMotion` | Reads PIR sensor with 10ms debounce |

---

## System Flow

```
[START] → GPIO + SysTick Init
              ↓
        [Main Loop while(1)]
              ↓
      ASM_CheckMotion() → read PA0
              ↓
      Motion detected?
       YES → Red LED + Buzzer + Bluetooth alert
       NO  → Green LED (idle)
              ↓
      Alarm timer expired (3000ms)?
       YES → Deactivate alarm, back to idle
```

---

## Getting Started

**Requirements:** Keil MDK μVision 5, ARM Compiler 6

1. Clone the repository
2. Open `PROJECT.uvprojx` in Keil MDK
3. Open **Manage Run-Time Environment** and enable:
   - CMSIS → Core ✓
   - Device → Startup ✓
4. Press **F7** to build
5. Press **Ctrl+F5** to start simulation
6. Open **View → Watch Windows → Watch 1** and add `motion_detected` and `alarm_active` to monitor system state

---

## Simulation Results

**Idle state** — no motion detected (`motion_detected = 0x00`):

![Idle State](screenshots/idle.png)

**Alarm state** — motion detected (`motion_detected = 0x01`):

![Alarm State](screenshots/alarm.png)

---

## Authors

- **Sepideh Pashayan** 
- **Atieh Jafarjadeh**

---

## Architecture Concepts Covered

- 3-stage Cortex-M3 pipeline (Fetch / Decode / Execute)
- NVIC interrupt handling and SysTick ISR
- Addressing modes: Register Indirect, Immediate, PC-Relative
- Load/Store instructions: `LDR`, `STR`, `STRB`
- Stack management with `PUSH`/`POP` and AAPCS calling convention
- Subroutine calls with `BL` and return via `POP {PC}`
- C ↔ Assembly linkage via `EXPORT` / `IMPORT`
