# Wiring Diagram — UGV Antincendio Wildfire Robotics

## Panoramica
Questo documento descrive il cablaggio completo di tutti i componenti elettronici del robot.
Modifiche rispetto al PDF `Electronics.pdf`:
- **Rimosso**: thermal camera, flame sensor
- **Aggiunto**: laser puntatore KY-008 5V (pack ANGEEK)
- **HC-SR04 x2**: usati solo per FOLLOW (distanza persona), non per fire detection

## Distribuzione di Potenza

```
LIPO 11.1V 3S 2000mAh ──XT60── SWITCH PRINCIPALE ──┬── TB6612FNG VM (11.1V direct) ──→ 2× DC Motor
                                                    ├── UBEC YSIDO 8A ──→ 6V ──→ 2× Servo MG996R (VCC)
                                                    └── XL6019 Buck ──→ 5V ──→ Arduino Uno Q VIN
                                                                  ├── KY-008 Laser VCC
                                                                  └── HC-SR04 x2 VCC
```

### Sequenza di Accensione
1. Verificare che lo switch principale sia OFF
2. Collegare LIPO al connettore XT60
3. Accendere lo switch principale
4. Attendere che il sistema ROS2 completi il boot (LED verde Arduino Uno Q)
5. Per spegnere: invertire la sequenza

### Sicurezza LIPO
- [ ] Installare fusibile in linea XT60 (3A raccomandato)
- [ ] Verificare che la batteria non superi 11.1V a piena carica (max 12.6V)
- [ ] Non lasciare incustodito durante la carica
- [ ] Utilizzare sack LIPO per stoccaggio sicuro

## Tabella Pinout Completa

### MCU (STM32U585) — Arduino Uno Q

| Componente | Segnale | Pin MCU | Note |
|------------|---------|---------|------|
| TB6612FNG AIN1 | GPIO out | TODO: Assign pin | Motor A direction |
| TB6612FNG AIN2 | GPIO out | TODO: Assign pin | Motor A direction |
| TB6612FNG PWMA | PWM out | TODO: Assign pin | Motor A speed |
| TB6612FNG BIN1 | GPIO out | TODO: Assign pin | Motor B direction |
| TB6612FNG BIN2 | GPIO out | TODO: Assign pin | Motor B direction |
| TB6612FNG PWMB | PWM out | TODO: Assign pin | Motor B speed |
| TB6612FNG STBY | GPIO out | TODO: Assign pin | Standby (active low) |
| Servo Pan MG996R | PWM 50Hz | TODO: Assign pin | Pan rotation |
| Servo Tilt MG996R | PWM 50Hz | TODO: Assign pin | Tilt rotation |
| HC-SR04 #1 TRIG | GPIO out | TODO: Assign pin | Front-left trigger |
| HC-SR04 #1 ECHO | GPIO in | TODO: Assign pin | Front-left echo |
| HC-SR04 #2 TRIG | GPIO out | TODO: Assign pin | Front-right trigger |
| HC-SR04 #2 ECHO | GPIO in | TODO: Assign pin | Front-right echo |
| KY-008 Laser SIG | GPIO out | TODO: Assign pin | Laser control (5V tolerant) |
| Push Button | GPIO in | TODO: Assign pin | Pull-up interno, con debounce RC+SW |

### MPU (Qualcomm Dragonwing) — Arduino Uno Q

| Componente | Segnale | Pin MPU | Note |
|------------|---------|---------|------|
| USB Camera | USB UVC | USB-A port | 640x480 @ 15fps |
| micro-ROS bridge | UART | TODO: Assign pins | MPU↔MCU communication |

## Schema Connessioni

### TB6612FNG Motor Driver
```
MCU GPIO ──→ AIN1, AIN2, BIN1, BIN2 (logic level)
MCU PWM  ──→ PWMA, PWMB (PWM signal)
VM (11.1V) ──→ DC Motor A, DC Motor B
GND       ──→ WAGO common ground
STBY      ──→ MCU GPIO (high = enabled)
```

### HC-SR04 Ultrasonic Sensors
```
MCU GPIO ──→ TRIG (pulse 10μs)
MCU GPIO ←── ECHO (pulse width = distance)
VCC (5V) ──→ from XL6019 buck
GND      ──→ common ground
```

### KY-008 Laser Pointer
```
MCU GPIO ──→ Signal (5V logic OK from MCU)
VCC (5V) ──→ from XL6019 buck
GND      ──→ common ground
```

### Push Button
```
MCU GPIO ←── Button ──→ GND
Pull-up interno MCU (o RC network esterno)
Debounce: RC 100nF + 10kΩ in parallelo
```

### Servo MG996R (Pan e Tilt)
```
MCU PWM  ──→ Signal (PWM 50Hz, 1-2ms duty cycle)
VCC (6V) ──→ from UBEC YSIDO 8A
GND      ──→ common ground
```

## Cavi e Connettori

- [ ] WAGO 221-415 per tutti i GND comuni
- [ ] XT60 per connessione batteria
- [ ] JST-PH 2pin per HC-SR04
- [ ] 3pin dupont per servopan/tilt
- [ ] 2pin per laser KY-008

## Verifica Cablaggio

### Prima Accensione
- [ ] Verificare polarità di tutti i connettori
- [ ] Misurare tensioni in uscita dai regolatori prima di collegare carichi
- [ ] Verificare che tutti i GND siano in common
- [ ] Verificare che nessun cavo sia schiacciato o danneggiato
