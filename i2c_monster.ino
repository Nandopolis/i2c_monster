#include <Wire.h>
#include "CommandParser.h"
CommandParser cmd;

// direccion del dispositivo esclavo
#define SLA_ADDRESS 15
#define MASTER_REQ_READ    0x12
#define MASTER_REQ_WRITE   0x34

enum DATA_enum {
  YEST_B_R, // velocidad angular del motor trasero derecho
  YEST_B_L, // velocidad angular del motor trasero izquierdo
  YEST_F_R, // velocidad angular del motor delantero derecho
  YEST_F_L, // velocidad angular del motor delantero izquierdo
  V_LIN, // velocidad lineal del robot m/s
  W_ANG, // velocidad angular del robot rad/s
  
  DATA_MAX,
};

union { int32_t q; uint8_t b[4];} tx_data;
union { int32_t q; uint8_t b[4];} rx_data;

uint8_t aux, i, j;    // numero de bytes recividos

// tipos de errores que reconoce la libreria Wire
enum error_enum {
  SUCCESS,    // sin errores
  BUFF_OVF,   // los datos a transmitir no entran en el buffer
  N_ADDRESS,  // se recibio nack al enviar la direccion
  N_DATA,     // se recibio nack al enviar un dato
};

// variable en la que se gurdara el error recibido
uint8_t error;

void readReg(String *args) {
  uint8_t aux;
  uint8_t error, j;
  uint8_t *pointer;
  int32_t num;
  
  if (args[0] != ""){
    aux = args[0].toInt();
    Serial.println(args[0]);
    Serial.println(aux);
    if (aux > 0) {
      Serial.println("solicitando lectura");
      // tipo de transaccion
      do {
        Wire.beginTransmission(SLA_ADDRESS);
        Wire.write(MASTER_REQ_READ);
        error = Wire.endTransmission();
        Serial.println(error);
      } while (error != SUCCESS);
    
      Serial.print("solicitando reg ");
      Serial.println(aux);
      // direccion del registro de memoria
      do {
        Wire.beginTransmission(SLA_ADDRESS);
        Wire.write(aux);
        error = Wire.endTransmission();
        Serial.println(error);
      } while (error != SUCCESS);
    
      Serial.println("solicitando lectura, 4 bytes...");
      do {
        error = Wire.requestFrom(SLA_ADDRESS, 4);
        Serial.println(error);
      } while (error != 4);
      if (error == 4) {
        pointer = (uint8_t *)&rx_data;
        for (j = 0; j < 4; j++) {
          rx_data.b[j] = Wire.read();
        }
        float flo = rx_data.q / 65536.0;
        Serial.print("float leido: ");
        Serial.println(flo);
      }
      else {
        Serial.println("error al solicitar lectura...");
      }
          
      Serial.println("");
    }
  }
}

void writeReg(String *args) {
  uint8_t index;
  float aux;
  
  if (args[0] != "" && args[1] != ""){
    index = args[0].toInt();
    aux = args[1].toFloat();
    Serial.println(args[0]);
    Serial.println(index);
    Serial.println(args[1]);
    Serial.println(aux);
    tx_data.q = (int32_t)(aux * 65536);
    if (aux > 0 && index > 0) {
      Serial.println("solicitando escritura");
      // tipo de transaccion
      do {
        Wire.beginTransmission(SLA_ADDRESS);
        Wire.write(MASTER_REQ_WRITE);
        error = Wire.endTransmission();
        Serial.println(error);
      } while (error != SUCCESS);
    
      Serial.print("solicitando reg ");
      Serial.println(index);
      // direccion del registro de memoria
      do {
        Wire.beginTransmission(SLA_ADDRESS);
        Wire.write(index);
        error = Wire.endTransmission();
        Serial.println(error);
      } while (error != SUCCESS);
    
      Serial.print("solicitando escritura ");
      Serial.println(aux);
      do {
        Wire.beginTransmission(SLA_ADDRESS);
        Wire.write((uint8_t *)tx_data.b, 4);
        error = Wire.endTransmission();
        Serial.println(error);
      } while (error != SUCCESS);
      
      Serial.println("");
    }
  }
}

void setup() {
  // put your setup code here, to run once:

  // configuramos el puerto i2c
  Wire.begin();           // iniciamos como maestro
  Wire.setClock(100000);  // frecuencia del clock = 100 kHz

  // configurmos el puerto serial
  // solo con fines de debug
  Serial.begin(115200);
  while(!Serial);

  cmd.setDelimiter(' '); // utilizamos un espacio en blanco (' ') como delimitador
  cmd.addCommand("read", readReg);
  cmd.addCommand("write", writeReg);
}

void loop() {
  // put your main code here, to run repeatedly:

  cmd.run();
}
