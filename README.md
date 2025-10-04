# ESP32 Mesh Messenger

A simple messenger system built on ESP32 using the ESP-NOW protocol in a mesh (ESP-MESH) configuration.  
The project is designed as a **crisis communication tool** in case GSM and internet networks are unavailable.  
Messages are sent and received via Serial, and then broadcast across the ESP32 mesh network.

---

## 🚀 Features
- Communication over ESP-NOW mesh (ESP-MESH) protocol
- Serial input → message is broadcast to all nodes
- Works without GSM or WiFi infrastructure
- Configurable network parameters (`mesh_config.hpp`)
- Can be connected to PC or smartphone (via Serial Monitor 115200)

---

## 🛠 Requirements
- ESP32 development board
- PlatformIO (recommended) or Arduino IDE
- USB connection for flashing and Serial Monitor

---

## ⚙️ Setup & Usage

1. Clone the repository:
   ```bash
   git clone https://github.com/your-user/esp32-mesh-messenger.git
   cd esp32-mesh-messenger
   ```
2. Configure network settings and username in mesh_config.h

3. Build and upload using PlatformIO:
   ```bash
   pio run --target upload
   ```
4. Open Serial Monitor:
   ```bash
   pio device monitor
   ```
5. Type a message → it will be broadcast across the ESP32 mesh network.
   Message structure: [sender: text]
---

## 🔧 Configuration
Network settings are defined in the `mesh_config` file.  
Adjust parameters as needed (e.g., channel, encryption keys, node IDs).

---

## 📖 Documentation
Full code documentation is available via [Doxygen](https://www.doxygen.nl).  
To generate it locally:
```bash
doxygen Doxyfile
```
Then open `docs/html/index.html`.
