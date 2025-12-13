# SimpleServo: AVR Servo Driver
SimpleServo is a register-level C++ library for controlling servo motors on ATmega328p microcontrollers (Arduino Uno/Nano).

This project was built from scratch to explore hardware timers, interrupt handling, and low-level resource management without relying on high-level Arduino abstractions.

## 🎯 Project Goal & Motivation
The standard Arduino Servo library is excellent, but it abstracts away the underlying hardware complexity. My goal with this project was to break through this abstraction and understand exactly how a microcontroller generates precise PWM signals for real-time control.

Instead of using pre-configured functions like digitalWrite or standard timer modes, I built this driver using the ATmega328p datasheet as my primary reference. This allowed me to master:

  - manually configuring Timer1 registers (TCCR1A, TCCR1B, TIMSK1) manually.

  - understanding the practical differences between CTC (Clear Timer on Compare) and Normal Mode.

  - managing atomic operations and race conditions in an 8-bit architecture.

  - minimizing ISR execution time via Direct Register Access

## ⚙️ Technical Implementation
### 1. Timer1 Architecture:
   
I chose Normal Mode (free-running timer) over the simpler CTC mode to ensure signal stability.

Concept: The timer runs continuously from 0 to 65535 and overflows naturally.

Scheduling: Events (Pulse Start/End) are scheduled by accumulating the Output Compare Register: OCR1A += interval.

Benefit: Unlike CTC mode, which resets the counter and destroys the time reference, Normal Mode maintains a fixed timeline. This prevents the accumulation of delays from previous interrupts.

### 2. GPIO Control: Direct Register Access

Implementation: Used direct bitwise operations on PORTB and PORTD registers using pre-calculated masks.

Justification: Standard functions take ~3-5 µs to execute. Direct manipulation takes 2 clock cycles (~0.125 µs). Minimizing ISR execution time is critical to prevent "jitter" caused by interrupts blocking the main loop or each other.

### 3. Atomic Operations

Since the ATmega328p is an 8-bit CPU, reading/writing 16-bit variables (like timer ticks) is not atomic.

Solution: I implemented critical sections using global interrupt locks (cli / sei) around shared variables.

Justification: This prevents Data Tearing, where an ISR could interrupt a 16-bit write operation halfway (between the low-byte and high-byte write), leading to corrupted values and unpredictable motor behavior.

## 🛠 Installation & Usage
1. Clone this repository into your Arduino libraries folder.

2. Include <SimpleServo.h> in your sketch.

##### Examples

You can find a minimal usage example in the examples/SimpleSweep folder.

## ⚠️ Known Limitations & Architectural Roadmap
### 1. Encapsulation Violation

Currently, the on_interrupt static method is declared public.

The Problem: The global ISR (TIMER1_COMPA_vect) needs access to the class logic, which currently forces me to expose an internal method to the public API. This breaks encapsulation.

Status & Research: I am actively researching C++ Design Patterns to solve this elegantly. Specifically, I am investigating the Factory Pattern and Interfaces (Abstract Base Classes) to decouple the hardware interrupt service routine from the driver implementation, ensuring better modularity and testability. This is part of my ongoing study of advanced C++ features.

### 2. Hardware Support

The current implementation is optimized specifically for ATmega328p (Timer1) and does not support generic pin mapping for other architectures.

## 📝 License
This project is open-source and available under the MIT License.
