/*******CONFIGURACION EN ARDUINO*******/
//Flash Mode		  : QIO 80MHz
//Flash Size		  : 16M (128Mb)
//PSRAM			      : Disabled
//Partition Scheme: Default 4MB with spiffs (1.2MB APP/1.5MB SPIFFS)*/
/*******WIFI***************************/
//hostname en el router   : esp32s3-66F0B4
//IP en el router         : 192.168.1.100
#include <WiFi.h>
#include <WebServer.h>

// Configuración WiFi
const char* ssid = "ssid_wifi";
const char* password = "password_wifi";

WebServer server(80);

// Configuración IP estática
IPAddress local_IP(192, 168, 1, 100);    // IP estática que quieres asignar
IPAddress gateway(192, 168, 1, 1);       // Puerta de enlace de tu router (normalmente .1)
IPAddress subnet(255, 255, 255, 0);      // Máscara de subred típica
IPAddress primaryDNS(8, 8, 8, 8);        // DNS primario (Google)
IPAddress secondaryDNS(8, 8, 4, 4);      // DNS secundario (Google)

// Variables para las rosas
int rosasBuenEstado = 8;
int rosasMalEstado = 3;

void handleRoot() {
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <title>🌹 Monitoreo de Rosas - ESP32</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body { 
        font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; 
        text-align: center; 
        margin: 0;
        padding: 20px;
        background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
        min-height: 100vh;
      }
      .container {
        background: white;
        padding: 30px;
        border-radius: 20px;
        box-shadow: 0 10px 30px rgba(0,0,0,0.2);
        max-width: 600px;
        margin: 0 auto;
      }
      h1 {
        color: #333;
        margin-bottom: 30px;
      }
      .estado-rosas {
        display: flex;
        justify-content: space-around;
        margin: 30px 0;
      }
      .buen-estado, .mal-estado {
        padding: 25px;
        border-radius: 15px;
        width: 45%;
      }
      .buen-estado {
        background: #E8F5E8;
        border: 3px solid #4CAF50;
      }
      .mal-estado {
        background: #FFEBEE;
        border: 3px solid #F44336;
      }
      .icono {
        font-size: 4em;
        margin-bottom: 10px;
      }
      .contador {
        font-size: 3.5em;
        font-weight: bold;
        margin: 10px 0;
      }
      .buen-estado .contador {
        color: #2E7D32;
      }
      .mal-estado .contador {
        color: #C62828;
      }
      .label {
        font-size: 1.2em;
        color: #666;
        font-weight: bold;
      }
      .info {
        margin-top: 25px;
        padding: 15px;
        background: #f8f9fa;
        border-radius: 10px;
        color: #666;
      }
      .total {
        font-size: 1.3em;
        font-weight: bold;
        color: #2196F3;
        margin: 15px 0;
      }
      .progreso {
        width: 100%;
        height: 20px;
        background: #e0e0e0;
        border-radius: 10px;
        margin: 15px 0;
        overflow: hidden;
      }
      .barra-buenas {
        height: 100%;
        background: linear-gradient(90deg, #4CAF50, #8BC34A);
      }
    </style>
  </head>
  <body>
    <div class="container">
      <h1>🌹 Jardín de Rosas - Monitoreo</h1>
      
      <div class="total">
        Total de rosas: )rawliteral";
  
  html += String(rosasBuenEstado + rosasMalEstado);
  
  html += R"rawliteral(
      </div>

      <div class="progreso">
        <div class="barra-buenas" style="width: )rawliteral";
  
  int total = rosasBuenEstado + rosasMalEstado;
  int porcentajeBuenas = total > 0 ? (rosasBuenEstado * 100) / total : 0;
  html += String(porcentajeBuenas) + "%";
  
  html += R"rawliteral("></div>
      </div>
      <div style="color: #666; margin-bottom: 20px;">
        🌹 Buen estado: )rawliteral";
  
  html += String(porcentajeBuenas) + "%";
  
  html += R"rawliteral(
      </div>

      <div class="estado-rosas">
        <div class="buen-estado">
          <div class="icono">🌹</div>
          <div class="contador">)rawliteral";
  
  html += String(rosasBuenEstado);
  
  html += R"rawliteral(</div>
          <div class="label">Rosas en Buen Estado</div>
        </div>
        
        <div class="mal-estado">
          <div class="icono">🥀</div>
          <div class="contador">)rawliteral";
  
  html += String(rosasMalEstado);
  
  html += R"rawliteral(</div>
          <div class="label">Rosas Marchitas</div>
        </div>
      </div>

      <div class="info">
        <p><strong>IP del Servidor:</strong> )rawliteral";
  
  html += WiFi.localIP().toString();
  
  html += R"rawliteral(</p>
        <p><strong>Tipo de IP:</strong> Estática</p>
        <p><strong>Memoria libre:</strong> )rawliteral";
  
  html += String(esp_get_free_heap_size());
  
  html += R"rawliteral( bytes</p>
      </div>
    </div>
  </body>
  </html>
  )rawliteral";
  
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  
  // Configurar IP estática
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("❌ Error al configurar IP estática");
  }
  
  // Conectar WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  
  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) {
    delay(500);
    Serial.print(".");
    timeout++;
  }
  
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("✅ WiFi conectado!");
    Serial.print("📍 IP estática: ");
    Serial.println(WiFi.localIP());
    Serial.print("📡 Gateway: ");
    Serial.println(gateway);
    Serial.print("🔧 Subnet: ");
    Serial.println(subnet);
  } else {
    Serial.println("❌ Error conectando al WiFi");
    return;
  }

  // Configurar única ruta del servidor
  server.on("/", handleRoot);
  
  // Iniciar servidor
  server.begin();
  Serial.println("✅ Servidor de monitoreo de rosas iniciado");
  Serial.println("🌐 Accede a: http://" + WiFi.localIP().toString());
}

void loop() {
  server.handleClient();
  delay(100);
}
