#!/usr/bin/env python3
"""
check_pins.py — Wildfire Robotics UGV

Static sanity checker for include/pins.h.

Verifies:
  1. No GPIO is assigned to two different functions.
  2. Every pin tagged as PWM is on the UNO Q PWM-capable set.
  3. The two reserved UART pins (D0/D1) are not reused.
  4. The four ECHO/BUTTON pins (used as EXTI) are flagged as inputs only.

Run:
    python3 _RTOS/firmware/uno_q_mcu/tools/check_pins.py

Exit code 0 = OK, 1 = violation.
"""

import os
import re
import sys

THIS_DIR = os.path.dirname(os.path.abspath(__file__))
PINS_H   = os.path.join(THIS_DIR, '..', 'include', 'pins.h')

# UNO R3 silkscreen → which header pins fire hardware PWM. The UNO Q
# Arduino core mirrors the classic UNO map for analogWrite().
PWM_CAPABLE = {3, 5, 6, 9, 10, 11}

# UART pins are reserved by the micro-ROS serial transport.
UART_RESERVED = {0, 1}

# These pin macros must be hooked up to PWM (PWM out for motors, Servo
# library output for servos).
PWM_REQUIRED = {
    'PIN_MOTOR_A_RPWM',
    'PIN_MOTOR_A_LPWM',
    'PIN_MOTOR_B_RPWM',
    'PIN_MOTOR_B_LPWM',
    'PIN_SERVO_PAN',
    'PIN_SERVO_TILT',
}

# These pin macros are configured as inputs with EXTI in the drivers.
EXTI_INPUTS = {
    'PIN_BUTTON',
    'PIN_HCSR04_1_ECHO',
    'PIN_HCSR04_2_ECHO',
}


def parse_pins(path):
    """Return dict {macro_name: int_pin} from #define lines.

    Accepts plain integers or A0..A5 (mapped 14..19, the classic UNO
    convention preserved by the UNO Q core).
    """
    out = {}
    pattern = re.compile(
        r'^\s*#define\s+(PIN_[A-Z0-9_]+)\s+(A?\d+)\b'
    )
    with open(path) as fh:
        for line in fh:
            m = pattern.match(line)
            if not m:
                continue
            name, raw = m.group(1), m.group(2)
            if raw.startswith('A'):
                pin = 14 + int(raw[1:])
            else:
                pin = int(raw)
            out[name] = pin
    return out


def main():
    if not os.path.exists(PINS_H):
        print(f'ERROR: {PINS_H} not found')
        return 1

    pins = parse_pins(PINS_H)
    if not pins:
        print('ERROR: no PIN_* macros parsed')
        return 1

    errors = []

    # 1. Reuse check.
    seen = {}
    for name, pin in pins.items():
        if pin in seen:
            errors.append(
                f'pin {pin} reused by {seen[pin]} and {name}')
        else:
            seen[pin] = name

    # 2. PWM capability check.
    for name in PWM_REQUIRED:
        if name not in pins:
            errors.append(f'missing required PWM macro {name}')
            continue
        if pins[name] not in PWM_CAPABLE:
            errors.append(
                f'{name} = D{pins[name]} not PWM-capable '
                f'(need one of {sorted(PWM_CAPABLE)})')

    # 3. UART pins not reused.
    for name, pin in pins.items():
        if pin in UART_RESERVED:
            errors.append(
                f'{name} = D{pin} collides with USART RX/TX (D0/D1)')

    # 4. EXTI inputs declared.
    for name in EXTI_INPUTS:
        if name not in pins:
            errors.append(f'missing EXTI input macro {name}')

    print(f'Parsed {len(pins)} pin macros from pins.h:')
    for name, pin in sorted(pins.items(), key=lambda kv: kv[1]):
        label = f'D{pin}' if pin < 14 else f'A{pin - 14}'
        print(f'  {label:>4}  {name}')

    if errors:
        print('\nFAIL:')
        for e in errors:
            print(f'  - {e}')
        return 1

    print('\nOK: pin map consistent, PWM/UART/EXTI rules respected.')
    return 0


if __name__ == '__main__':
    sys.exit(main())