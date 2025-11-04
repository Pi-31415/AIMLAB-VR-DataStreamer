# AIMLAB-VR Vibration Motor Controller - Serial Protocol
**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 02 November 2025  
**Version:** v3.0

---

## 🔌 Serial Communication Protocol

### Overview

The Motor Controller UI communicates with Arduino via serial (UART) at **9600 baud**. The protocol includes a handshake sequence and command structure.

---

## 📡 Connection Sequence

### 1. Port Scanning
```
PC scans COM1 through COM40
For each available port:
  - Open port
  - Configure (9600 8N1)
  - Attempt handshake
  - If success: connection established
  - If fail: close port and try next
```

### 2. Arduino Reset Delay
```cpp
Sleep(2000);  // 2 seconds
```

**Why?** Most Arduino boards (Uno, Nano, Mega) reset when the serial port is opened due to the DTR (Data Terminal Ready) line. The 2-second delay ensures the Arduino bootloader finishes and the sketch starts running.

### 3. Handshake Protocol

**PC to Arduino:**
```
"HELLO\r\n"
```
- Uses `\r\n` (CRLF) line ending
- Matches Arduino's `Serial.println()` format

**Arduino to PC:**
```
"Vibration Motor Controller Ready\r\n"
```
- Must contain substring: `"Vibration Motor Controller Ready"`
- Timeout: 4000 ms (4 seconds)

---

## 🎯 Command Protocol

### Test Vibration Command

**PC to Arduino:**
```
'1'
'\n'
```
- Single character `'1'`
- Followed by newline `'\n'`

**Arduino Response:**
- No response required
- Arduino should trigger vibration motor

---

## ⚙️ Serial Configuration

| Parameter | Value |
|-----------|-------|
| **Baud Rate** | 9600 |
| **Data Bits** | 8 |
| **Parity** | None |
| **Stop Bits** | 1 |
| **Flow Control** | None |
| **Format** | 8N1 |

### Timeout Settings
```cpp
COMMTIMEOUTS timeouts = { 0, 0, 0, 0, 0 };
```

All timeouts set to 0 for non-blocking behavior. The `readLine()` function implements its own timeout logic using `GetTickCount()`.

---

## 🔧 Key Improvements (v3.0)

### 1. Arduino Reset Handling
```cpp
// OLD (v2.0) - Immediate handshake
WriteFile(serial, "HELLO\n", ...);

// NEW (v3.0) - Wait for Arduino reset
Sleep(2000);  // Give Arduino time to boot
WriteFile(serial, "HELLO\r\n", ...);
```

### 2. Line Ending Compatibility
```cpp
// OLD - Unix style
"HELLO\n"

// NEW - Windows/Arduino style
"HELLO\r\n"
```

Arduino's `Serial.println()` uses `\r\n`, so we match that format for consistency.

### 3. Extended Timeout
```cpp
// OLD - 2 second timeout
readLine(serial, line, 2000)

// NEW - 4 second timeout
readLine(serial, line, 4000)
```

Allows more time for Arduino to respond, especially on slower boards or if the Arduino is performing initialization.

### 4. Simplified Connection Logic
```cpp
// OLD - Checked if already connected, could disconnect
if (connected.load()) {
    CloseHandle(hSerial);
    connected = false;
    return false;
}

// NEW - Direct connection attempt
// (Connection button handles disconnect separately if needed)
```

---

## 🤖 Arduino Firmware Requirements

### Minimal Example

```arduino
// AIMLAB-VR Vibration Motor Controller Firmware
// Author: Pi Ko (pi.ko@nyu.edu)

const int MOTOR_PIN = 9;  // PWM pin for motor

void setup() {
  Serial.begin(9600);
  pinMode(MOTOR_PIN, OUTPUT);
  
  // Send ready message
  Serial.println("Vibration Motor Controller Ready");
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();
    
    // Handle HELLO handshake
    if (cmd == 'H') {
      // Read rest of "ELLO\r\n"
      while (Serial.available()) Serial.read();
      Serial.println("Vibration Motor Controller Ready");
    }
    
    // Handle vibration command
    else if (cmd == '1') {
      // Trigger vibration for 500ms
      digitalWrite(MOTOR_PIN, HIGH);
      delay(500);
      digitalWrite(MOTOR_PIN, LOW);
    }
  }
}
```

### Enhanced Example with PWM

```arduino
const int MOTOR_PIN = 9;  // PWM pin
const int MAX_INTENSITY = 255;

void setup() {
  Serial.begin(9600);
  pinMode(MOTOR_PIN, OUTPUT);
  Serial.println("Vibration Motor Controller Ready");
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();
    
    switch(cmd) {
      case 'H':  // Handshake
        while (Serial.available()) Serial.read();
        Serial.println("Vibration Motor Controller Ready");
        break;
        
      case '1':  // Low intensity
        pulseMotor(128, 300);
        break;
        
      case '2':  // Medium intensity
        pulseMotor(192, 300);
        break;
        
      case '3':  // High intensity
        pulseMotor(255, 300);
        break;
        
      case '0':  // Stop
        analogWrite(MOTOR_PIN, 0);
        break;
    }
  }
}

void pulseMotor(int intensity, int duration) {
  analogWrite(MOTOR_PIN, intensity);
  delay(duration);
  analogWrite(MOTOR_PIN, 0);
}
```

---

## 📊 Timing Diagram

```
Time (ms)    PC                        Arduino
    0        Open COM port     →       [RESET triggered by DTR]
    
  2000       [Wait]                    [Bootloader running]
                                       [Sketch starts]
                                       setup() executes
                                       Send "Ready" message
    
  2100       Send "HELLO\r\n"  →       
  
  2150                         ←       "Vibration Motor...\r\n"
  
  2200       [Handshake OK]
             Connection established
             
  5000       User clicks "Test"
             Send "1\n"        →       
  
  5050                                 Motor ON
  5550                                 Motor OFF
```

---

## 🐛 Troubleshooting

### Issue: Handshake Timeout

**Symptoms:**
- UI shows "Vibration Motor Not Connected"
- All COM ports tested but none connect

**Solutions:**

1. **Check Arduino is running firmware**
   ```arduino
   Serial.println("Vibration Motor Controller Ready");
   ```
   Must be in `setup()` or sent in response to handshake.

2. **Verify baud rate**
   - Both PC and Arduino must use 9600 baud
   - Check `Serial.begin(9600)` in Arduino code

3. **Check line endings**
   - Arduino Serial Monitor: Set to "Both NL & CR" or "Newline"
   - Firmware should use `Serial.println()` not `Serial.print()`

4. **Increase timeout**
   - Edit `readLine(serial, line, 4000)` to higher value if needed
   - Some Arduinos (Mega 2560) may take longer to boot

### Issue: Connection Works But Vibration Doesn't Trigger

**Solutions:**

1. **Check command format**
   ```cpp
   // UI sends:
   '1' + '\n'
   
   // Arduino should check:
   if (cmd == '1') { /* trigger motor */ }
   ```

2. **Verify motor wiring**
   - Check motor connected to correct pin
   - Ensure adequate power supply
   - Test motor with direct digitalWrite/analogWrite

3. **Check serial buffer**
   - Clear buffer after commands: `while(Serial.available()) Serial.read();`

---

## 🔍 Debug Tips

### On PC Side

Add debug output to `connectToArduino()`:

```cpp
for (const auto& port : ports) {
    MessageBox(NULL, port.c_str(), "Trying port", MB_OK);
    // ... rest of connection logic
}
```

### On Arduino Side

Add status messages:

```arduino
void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();
    Serial.print("Received: ");
    Serial.println(cmd);
    
    if (cmd == '1') {
      Serial.println("Triggering motor");
      // ... motor code
    }
  }
}
```

---

## 📝 Protocol Summary

| Direction | Message | Purpose | Format |
|-----------|---------|---------|--------|
| PC → Arduino | `HELLO\r\n` | Initiate handshake | Text + CRLF |
| Arduino → PC | `Vibration Motor Controller Ready\r\n` | Confirm ready | Text + CRLF |
| PC → Arduino | `1\n` | Trigger vibration | Char + LF |
| PC → Arduino | `0\n` | Stop vibration (optional) | Char + LF |

---

## 🎯 Best Practices

1. **Always wait 2 seconds after opening port** - Allows Arduino to reset
2. **Use `\r\n` for text messages** - Matches Arduino Serial.println()
3. **Set reasonable timeouts** - 4 seconds for handshake
4. **Close port on failure** - Don't leave ports hanging
5. **Run serial I/O in background threads** - Keep UI responsive
6. **Clear serial buffers** - Prevent command buildup

---

**Implementation Status:** ✅ Complete  
**Tested:** Ready for Arduino integration  
**Compatibility:** Arduino Uno, Nano, Mega, and compatible boards

---

*Last Updated: 02 November 2025*

