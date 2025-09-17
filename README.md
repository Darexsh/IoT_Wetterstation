* * *

🌦️ ESP32-S3 Weather Station
============================

This project is a **custom weather station** built on the ESP32-S3 microcontroller. It integrates multiple sensors and a touchscreen display to provide real-time environmental monitoring, both locally and via a web interface.

* * *

⚡ Features
----------

* **🖥️ Microcontroller:** ESP32-S3 for high-performance processing and Wi-Fi connectivity.
    
* **🛠️ Sensors:**
    
    * **VCNL4040:** Measures ambient light and proximity.
        
    * **BME688:** Measures temperature 🌡️, humidity 💧, air pressure 🌬️, and air quality/gas levels 🌫️.
        
    * **LTR390-UV:** Measures UV light ☀️ and provides the UV index (UVA/UVB).
        
* **🖲️ Display:** 7-inch resistive touch TFT display connected via **parallel interface (DB0–DB7)**.
    
    * Display brightness is automatically adjusted based on ambient light 🌙.
        
* **🌐 Web Interface:** ESP32-S3 hosts a web page where all sensor readings can be monitored remotely.
    
* **💡 Automatic Brightness Control:** Display dims in low-light environments for energy efficiency and comfort.
    
* **🎨 Touchscreen UI:** Navigate easily through various sensor readings and settings.
    

* * *

🔍 Sensor Details
-----------------

* **VCNL4040:** Provides both proximity and light measurements to monitor the surrounding environment and adjust the display brightness.
    
* **BME688:** Advanced environmental sensor capable of measuring temperature, humidity, atmospheric pressure, and air quality/gas concentrations.
    
* **LTR390-UV:** Accurately measures UV light, allowing calculation of UV index (A or B) for health and safety purposes.
    

* * *

🌐 Web Dashboard
----------------

The ESP32-S3 creates a local web server, accessible via Wi-Fi. Features include:

* Real-time display of all sensor readings 📊.
    
* Historical logging of measurements for analysis 🗂️.
    
* Mobile-friendly interface, accessible from smartphones, tablets, or PCs 📱💻.
    

* * *

🔧 Hardware Setup
-----------------

* **ESP32-S3 microcontroller**.
    
* **VCNL4040, BME688, LTR390-UV sensors** connected via I2C.
    
* **7-inch resistive TFT touch display** connected via **DB0–DB7 (parallel interface)**.
    
* **Power supply** compatible with ESP32-S3 and TFT display ⚡.
    

* * *

🚀 Installation & Usage
-----------------------

1. Flash the provided firmware to the ESP32-S3 using **ESP-IDF** or **Arduino IDE** 💾.
    
2. Connect the sensors and display according to the wiring diagram 🔌.
    
3. Power on the device 🔋.
    
4. Access the local web server via Wi-Fi 🌐 to view real-time sensor data.
    
5. Interact with the TFT touchscreen to navigate the local display interface 🖲️.
    

* * *

✨ Optional Enhancements
-----------------------

* Historical data logging to an SD card or external server 🗄️.
    
* Graphical representation of trends for temperature, humidity, pressure, and UV index 📈.
    
* Configurable alerts for high UV levels ☀️, extreme temperatures 🌡️, or poor air quality 🌫️.
    
* Integration with home automation systems like Home Assistant or MQTT 🏠.
    

* * *

📜 License
----------

This project is provided **as-is** under the MIT License 📝.

* * *

<!-- Keep this line! Rendered picture of PCB is going to be displayed after Production files are pushed to branch! -->
![](./Docs/board_preview_top_2025-09-12_fc06468.png)
![](./Docs/board_preview_bottom_2025-09-12_fc06468.png)
![](./Docs/Case.png)
