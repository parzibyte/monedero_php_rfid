/*
  ____          _____               _ _           _       
 |  _ \        |  __ \             (_) |         | |      
 | |_) |_   _  | |__) |_ _ _ __ _____| |__  _   _| |_ ___ 
 |  _ <| | | | |  ___/ _` | '__|_  / | '_ \| | | | __/ _ \
 | |_) | |_| | | |  | (_| | |   / /| | |_) | |_| | ||  __/
 |____/ \__, | |_|   \__,_|_|  /___|_|_.__/ \__, |\__\___|
         __/ |                               __/ |        
        |___/                               |___/         
    
____________________________________
/ Si necesitas ayuda, contáctame en \
\ https://parzibyte.me               /
 ------------------------------------
        \   ^__^
         \  (oo)\_______
            (__)\       )\/\
                ||----w |
                ||     ||
Creado por Parzibyte (https://parzibyte.me).
------------------------------------------------------------------------------------------------
            | IMPORTANTE |
Si vas a borrar este encabezado, considera:
Seguirme: https://parzibyte.me/blog/sigueme/
Y compartir mi blog con tus amigos
También tengo canal de YouTube: https://www.youtube.com/channel/UCroP4BTWjfM0CkGB6AFUoBg?sub_confirmation=1
Twitter: https://twitter.com/parzibyte
Facebook: https://facebook.com/parzibyte.fanpage
Instagram: https://instagram.com/parzibyte
Hacer una donación vía PayPal: https://paypal.me/LuisCabreraBenito
------------------------------------------------------------------------------------------------
*/
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

#define LONGITUD_BYTES 18
#define LONGITUD_BYTES_ESCRITURA 16
#define SEPARADOR_ORDENES_SERVIDOR ";"
#define ORDEN_RECARGAR "r"
#define ORDEN_DESCONTAR "d"
/*
Pines para conectar el lector
*/
#define RST_PIN D3
#define SS_PIN D4

// Credenciales para conectar a la red

const char *ssid = "Aquí va el nombre de tu red";
const char *password = "Aquí va la contraseña de tu red";
const String DIRECCION_SERVIDOR = "http://192.168.1.82/rfid_saldo";
// Conexión a WiFi
ESP8266WiFiMulti wifiMulti;
// La LCD
LiquidCrystal_I2C lcd(0x3f, 16, 2);

// El lector
MFRC522 lector(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key clave;
int tarjetaLeidaDesdeLoop = 0;
double saldoGlobal;

void obtenerSaldo(double *saldo)
{

  if (!lector.PICC_IsNewCardPresent())
  {
    *saldo = -1;
    Serial.println("No hay nueva tarjeta presente");
    return;
  }
  if (!lector.PICC_ReadCardSerial())
  {
    *saldo = -1;
    Serial.println("Error leyendo serial");
    return;
  }

  byte bloque, longitud, buferLectura[LONGITUD_BYTES];
  MFRC522::StatusCode estado;
  bloque = 1;
  estado = lector.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, bloque, &clave, &(lector.uid));
  if (estado != MFRC522::STATUS_OK)
  {
    Serial.println("Error autenticando");
    Serial.println(lector.GetStatusCodeName(estado));
    *saldo = -1;
    return;
  }
  longitud = LONGITUD_BYTES;
  estado = lector.MIFARE_Read(bloque, buferLectura, &longitud);
  if (estado != MFRC522::STATUS_OK)
  {
    Serial.println("Error leyendo bloque");
    Serial.println(lector.GetStatusCodeName(estado));
    *saldo = -1;
    return;
  }
  char mensaje[LONGITUD_BYTES] = "";

  for (uint8_t i = 0; i < LONGITUD_BYTES - 2; i++)
  {
    mensaje[i] = buferLectura[i];
  }
  double saldoLeido = atof(mensaje);
  Serial.println("Se lee el saldo");
  Serial.println(saldoLeido);
  *saldo = saldoLeido;
  lector.PICC_IsNewCardPresent();
}

void guardarSaldo(double saldo)
{

  if (!lector.PICC_IsNewCardPresent())
  {
    Serial.println("No hay nueva tarjeta presente al guardar saldo");
    return;
  }
  if (!lector.PICC_ReadCardSerial())
  {
    Serial.println("Error leyendo serial");
    return;
  }
  Serial.println("Se guarda el saldo");
  Serial.println(saldo);

  byte bloque, buferEscritura[LONGITUD_BYTES_ESCRITURA];
  // Convertir saldo a cadena
  char cadena[LONGITUD_BYTES_ESCRITURA] = "";
  snprintf(cadena, LONGITUD_BYTES_ESCRITURA, "%.2lf", saldo);
  // Copiar cadena al búfer
  for (uint8_t i = 0; i < LONGITUD_BYTES_ESCRITURA; i++)
  {
    buferEscritura[i] = cadena[i];
  }
  MFRC522::StatusCode estado;
  bloque = 1;
  estado = lector.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, bloque, &clave, &(lector.uid));
  if (estado != MFRC522::STATUS_OK)
  {
    Serial.println("Error autenticando");
    Serial.println(lector.GetStatusCodeName(estado));
    return;
  }
  estado = lector.MIFARE_Write(bloque, buferEscritura, LONGITUD_BYTES_ESCRITURA);
  if (estado != MFRC522::STATUS_OK)
  {
    Serial.println("Error escribiendo bloque");
    Serial.println(lector.GetStatusCodeName(estado));
    return;
  }
  // Ya pueden retirar la tarjeta

  lector.PICC_HaltA();
  lector.PCD_StopCrypto1();
  tarjetaLeidaDesdeLoop = 0;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SALDO:");
  lcd.setCursor(6, 0);
  lcd.print(saldo);
  lcd.setCursor(0, 1);
  lcd.print("RETIRE LA TARJETA");
}

void setup()
{
  // Iniciar comunicación Serial. Esto es útil solo para depurar, después se puede remover
  Serial.begin(9600);
  while (!Serial)
  {
    // Esperar serial. Nota: la tarjeta NO HARÁ NADA hasta que haya comunicación Serial (es decir, que el monitor serial sea abierto)
    // si tú no quieres esto, simplemente elimina todas las llamadas a Serial
  }
  // Iniciar LCD
  lcd.begin(16, 2); // <----------- En algunas versiones de NodeMCU (v2) debe llamarse antes de todo, antes de SPI.begin(); y antes de lector.PCD_Init();
  lcd.init();       // Encender la LCD
  lcd.backlight();

  // Iniciar lector
  SPI.begin();
  lector.PCD_Init();
  // Esperar algunos segundos
  delay(4);
  // Preparar la clave para leer las tarjetas RFID
  for (byte i = 0; i < 6; i++)
  {
    clave.keyByte[i] = 0xFF;
  }

  // Aquí puedes agregar varias redes. La tarjeta se conectará a la más cercana
  wifiMulti.addAP(ssid, password);
  // wifiMulti.addAP("Otra red", "Contraseña");

  // Esperar conexión WiFi. La tarjeta NO HARÁ NADA si no hay conexión WiFi
  Serial.print("Conectando al WiFi...");
  while (wifiMulti.run() != WL_CONNECTED)
  {
    delay(250);
    Serial.print(".");
  }
  lcd.clear();
  lcd.print("TODO OK");
  lcd.setCursor(0, 1);
  lcd.print("COLOCAR RFID");
  Serial.println("Todo OK");
}

// Loop infinito...
void loop()
{

  if (WiFi.status() != WL_CONNECTED)
  {
    // Si no hay WiFi, no hacemos nada
    Serial.println("No hay WiFi");
    lcd.clear();
    lcd.print("NO HAY");
    lcd.setCursor(0, 1);
    lcd.print("WIFI");
    return;
  }
  // Si no han colocado la tarjeta, vamos a ver si hay alguna presente
  if (!tarjetaLeidaDesdeLoop)
  {
    // Si no hay ninguna, nos detenemos
    if (!lector.PICC_IsNewCardPresent())
    {
      return;
    }

    // Si no se puede elegir la tarjeta, nos detenemos
    if (!lector.PICC_ReadCardSerial())
    {
      return;
    }
    Serial.println("Tarjeta leída");
    // hasta aquí ya hemos leído la tarjeta, así que marcamos la bandera en true (1)
    tarjetaLeidaDesdeLoop = 1;
    // Curiosamente esta línea hace que las siguientes llamadas a PICC_IsNewCardPresent devuelvan true
    lector.PICC_IsNewCardPresent();
    // Obtenemos el saldo de la tarjeta actualmente colocada en el lector
    obtenerSaldo(&saldoGlobal);
    // Lo imprimimos en la LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SALDO:");
    lcd.setCursor(6, 0);
    lcd.print(saldoGlobal);
    lcd.setCursor(0, 1);
    lcd.print("NO RETIRE RFID");
    lector.PCD_StopCrypto1();
  }
  else
  {
    Serial.println("Tarjeta en lector. Verificando con servidor");
    // Significa que la tarjeta ya está colocada en el lector, vamos a consultar el servidor para ver si hay que hacer algún cambio
    HTTPClient http;
    // Invocamos al servidor y le pasamos el saldo que hay actualmente
    String url = DIRECCION_SERVIDOR + "/index.php?saldo=" + saldoGlobal;
    http.begin(url);

    int httpCode = http.GET();
    if (httpCode > 0)
    {
      if (httpCode == HTTP_CODE_OK)
      {
        // si el servidor responde correctamente, obtenemos lo que nos haya respondido
        String payload = http.getString();
        // Leemos la respuesta en una cadena válida de C, no de la clase String
        char payloadComoCadenaValida[50] = "";
        for (uint8_t i = 0; i < payload.length(); i++)
        {
          payloadComoCadenaValida[i] = payload.charAt(i);
        }
        // El servidor nos dará una orden como "d;10" que quiere decir "descontar 10" así que debemos separar la cadena por ;
        char delimitador[] = ";";
        char *ordenExtraida = strtok(payloadComoCadenaValida, delimitador);
        char *saldoExtraido = strtok(NULL, delimitador);
        if (ordenExtraida != NULL && ordenExtraida != NULL)
        {
          // Extraemos el saldo que el servidor nos indica
          double diferenciaSaldo = atof(saldoExtraido);
          // Si se recarga, sumamos el saldo y lo guardamos
          if (strcmp(ordenExtraida, ORDEN_RECARGAR) == 0)
          {
            guardarSaldo(saldoGlobal + diferenciaSaldo);
          }
          else if (strcmp(ordenExtraida, ORDEN_DESCONTAR) == 0)
          {
            // En este caso, se descuenta el saldo
            double nuevoSaldo = saldoGlobal - diferenciaSaldo;
            // Guardar el nuevo saldo solo si el usuario tiene lo suficiente
            if (nuevoSaldo >= 0)
            {
              guardarSaldo(nuevoSaldo);
            }
            else
            {
              // Si el saldo fuera insuficiente, se indica y se guarda el mismo que había en la tarjeta. Es decir, no se hace ningún cambio
              guardarSaldo(saldoGlobal);
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("SALDO INSUFICIENTE");
              lcd.setCursor(0, 1);
              lcd.print("RETIRE TARJETA");
            }
          }
        }
      }
      else
      {
        Serial.println("Error: httpCode es " + http.errorToString(httpCode));
      }
    }
    else
    {
      Serial.println("Error en la solicitud" + http.errorToString(httpCode));
    }
    http.end(); //Cerramos la solicitud
    // Y hacemos esta petición cada 5 segundos
    delay(5000);
  }
}