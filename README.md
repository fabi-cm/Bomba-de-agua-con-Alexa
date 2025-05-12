# **Control de Bomba de Agua con Alexa y ESP32**  

Un proyecto simple para controlar una bomba de agua mediante comandos de voz en Alexa, utilizando un **ESP32** y **AWS IoT Core**.  

## **📌 Requisitos**  
### **Hardware**  
- ESP32 (con WiFi)  
- Sensor de humedad (opcional, si deseas monitoreo)  
- Módulo relé (para controlar la bomba)  
- Fuente de alimentación para la bomba  

### **Software**  
- **AWS IoT Core** (para el Shadow Device)  
- **Skill de Alexa** (para control por voz)
- **Alexa Backend** (El archivo se encuentra en lib/BackendAlexa)
- **Arduino IDE / PlatformIO** (para programar el ESP32)  

## **🔧 Configuración**  

### **1. ESP32 (Código Arduino)**  
Asegúrate de configurar:  
- **Certificados AWS IoT** (CA, Device Certificate y Private Key)  
- **Credenciales WiFi** (SSID y contraseña)  
- **Tópicos MQTT** (para el Shadow Device):  
  ```cpp
  #define SHADOW_UPDATE_TOPIC "$aws/things/prueba1/shadow/update"
  #define SHADOW_DELTA_TOPIC "$aws/things/prueba1/shadow/update/delta"
  ```  

📌 **Carga el código en el ESP32** y verifica que se conecte a AWS IoT.  

### **2. AWS IoT Core**  
1. **Crea un "Thing"** llamado `ESP32_Planta`.  
2. **Adjunta certificados** y una política que permita:  
   ```json
   {
     "Effect": "Allow",
     "Action": ["iot:Connect", "iot:Publish", "iot:Subscribe", "iot:Receive"],
     "Resource": "*"
   }
   ```  
3. **Verifica el Shadow Device** en la consola de AWS.  

### **3. Skill de Alexa**  
1. **Crea una Skill** en el [Alexa Developer Console](https://developer.amazon.com/alexa/console/ask).  
2. **Configura los Intents**:  
   - `TurnOnIntent` → Enciende la bomba.  
   - `TurnOffIntent` → Apaga la bomba.  
   - `StatusIntent` → Consulta el estado.  
3. **Vincula AWS Lambda** (para manejar las peticiones).  

## **🎙 Comandos de Voz**  
- *"Alexa, enciende la bomba de agua"* → **Activa el relé**.  
- *"Alexa, apaga la bomba"* → **Desactiva el relé**.  
- *"Alexa, ¿está encendida la bomba?"* → **Responde con el estado actual**.  

**¡Listo!** Ahora puedes controlar tu bomba de agua con Alexa. 🎉  

### **📜 Licencia**  
MIT License - Libre para uso y modificación.  