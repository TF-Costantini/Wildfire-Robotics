# Macchina a Stati — UGV Antincendio Wildfire Robotics

## Diagramma Stati

```
                    ┌──────────────────────────────────────────────────────┐
                    │                                                      │
                    │                                                      ▼
              ┌─────┴─────┐         press         ┌──────────┐         press
              │   IDLE    │ ◄───────────────────► │  FOLLOW  │ ◄──────────────────┐
              └─────┬─────┘                      └────┬─────┘                   │
                    │                                   │                         │
                    │                                   │        ┌──────────┐      │
                    │                                   ▼        │          │      │
                    │           press ──────────────────────────┼──► FIRE ─┘      │
                    │                                                │               │
                    │                                                │               │
                    └────────────────────────────────────────────────┘               │
                         (any state, long press or watchdog timeout?)               │
                                                                                    │
                              ┌─────────────────────────────────────────────────────┘
                              │
                              ▼
                    ┌─────────────────┐
                    │   ERROR/WATCHDOG │ (optional recovery state)
                    └────────┬────────┘
                             │
                             │ timeout/reset
                             ▼
                         IDLE
```

## Tabella Transizioni

| Stato Attuale | Evento | Stato Successivo | Azioni |
|---------------|--------|-----------------|--------|
| IDLE | button_press | FOLLOW | reset watchdog, pan-tilt to home, laser OFF |
| FOLLOW | button_press | FIRE | stop drive, start fire sweep |
| FOLLOW | person_lost_timeout | FOLLOW | stop drive, keep waiting |
| FOLLOW | watchdog_timeout | IDLE | emergency stop all |
| FIRE | button_press | IDLE | stop everything, reset |
| FIRE | lock_lost_timeout | SWEEPING | laser OFF, restart sweep |
| FIRE | lock_acquired | TRACKING | start tracking |
| SWEEPING | fire_detected | TRACKING | center on target |
| TRACKING | error < threshold + 0.5s | LOCKED | laser ON |
| TRACKING | error > threshold | SWEEPING | restart sweep |
| LOCKED | error > threshold | TRACKING | laser OFF |
| LOCKED | fire_lost > 0.3s | SWEEPING | laser OFF |

## Stati Dettagliati

### IDLE
- **Descrizione**: Stato di riposo, nessun movimento
- **Azioni**:
  - Drive: 0, 0 (motori fermi)
  - Pan-Tilt: home (0°, 0°)
  - Laser: OFF
  - Watchdog: attivo (500ms timeout)
- **Transizioni in uscita**: qualsiasi button_press → FOLLOW

### FOLLOW
- **Descrizione**: Tracking di una persona con mantenimento distanza
- **Input**: `/vision/person` (Detection), `/ultrasonic/left`, `/ultrasonic/right`
- **Azioni**:
  - Drive: calcolato da follow_controller (differenziale)
  - Pan-Tilt: home (0°, 0°) - fermo
  - Laser: OFF
  - Watchdog: 500ms senza cmd_drive → IDLE
- **Transizioni in uscita**:
  - button_press → FIRE
  - watchdog_timeout → IDLE

### FIRE
- **Descrizione**: Ricerca e tracciamento fuoco con laser
- **Sottostati interni**:
  - SWEEPING: pan-tilt in movimento per cercare fuoco
  - TRACKING: fuoco trovato, tracking attivo
  - LOCKED: fuoco centrato, laser attivo
- **Input**: `/vision/fire` (Detection)
- **Azioni**:
  - Drive: 0, 0 (fermo)
  - Pan-Tilt: controllato da fire_controller
  - Laser: ON solo in LOCKED
  - Watchdog: 500ms → IDLE
- **Transizioni in uscita**:
  - button_press → IDLE
  - watchdog_timeout → IDLE

## Logica Gating Centrale

Lo `state_machine_node` implementa il gating centrale:
- **IDLE**: forward ZERO su tutti i cmd (drive, pantilt, laser)
- **FOLLOW**: forward solo cmd_drive dal follow_controller; pantilt → home; laser OFF
- **FIRE**: forward solo cmd_pantilt e cmd_laser dal fire_controller; drive ZERO

## Watchdog

- Timeout: 500ms
- Se nessun comando valido arriva dal controller attivo entro il timeout:
  - Pubblicare comando neutro (drive=0, pantilt=home, laser=OFF)
  - Opzionalmente: transizione a IDLE

## Condizioni di Lock (FIRE → LOCKED)

- Errore di centratura < N pixel (threshold da calibrare)
- Condizione mantenuta per ≥ 0.5 secondi consecutivi
- Laser si accende SOLO quando LOCKED
- Perdita lock: errore > threshold OPPURE found=false per > 0.3s
