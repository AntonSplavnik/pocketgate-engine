# SPI (Serial Peripheral Interface)

**What is SPI?**

SPI is a communication protocol. Think of it like a one-way phone line where the Pico talks to the display very fast. The Pico is the **Master** (controls the conversation) and the display is the **Slave** (listens and responds).

**How it works:**
- Data is sent one bit at a time (serial)
- The clock signal synchronizes timing
- Very fast: 62.5 MHz = 62.5 million bits per second!

---

## Communication Pattern

### Sending a Command:
```
1. Set CS LOW       (select display)
2. Set DC LOW       (command mode)
3. Send command byte via SPI
4. Set CS HIGH      (deselect display)
```

### Sending Data:
```
1. Set CS LOW       (select display)
2. Set DC HIGH      (data mode)
3. Send data bytes via SPI
4. Set CS HIGH      (deselect display)
```

---

## Sharing the SPI Bus

Multiple devices (display and SD card) can share the same SPI bus:
- Both use the same SCK, MOSI, MISO pins
- Each device has its own CS (Chip Select) pin
- Only the device with CS LOW will respond
- **Never set both CS pins LOW simultaneously**
