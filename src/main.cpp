/*
  Exemplo de código para ESP32-CAM
  Captura uma foto quando um pulso é detectado em um pino,
  acende o LED do flash e salva a imagem no cartão de memória.
*/

#include "esp_camera.h"
#include "FS.h"
#include "SD_MMC.h"

// Pino para detecção de pulso
const int pinoPulso = 14;

// LED do flash
const int pinoFlash = 4;

// Configurações da câmera
#define CAMERA_MODEL_AI_THINKER

// Inicializa a câmera
camera_fb_t * fb = NULL;

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando a câmera...");

  // Configura a câmera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5;
  config.pin_d1 = 18;
  config.pin_d2 = 19;
  config.pin_d3 = 21;
  config.pin_d4 = 36;
  config.pin_d5 = 39;
  config.pin_d6 = 34;
  config.pin_d7 = 35;
  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn = 32;
  config.pin_reset = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_UXGA;
  config.jpeg_quality = 10;
  config.fb_count = 2;

  // Inicializa a câmera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Falha ao iniciar a câmera: 0x%x", err);
    ESP.restart();
  }

  // Configura o pino de pulso como entrada
  pinMode(pinoPulso, INPUT_PULLUP);

  // Inicializa o cartão de memória
  if (!SD_MMC.begin("/sdcard", true)) {
    Serial.println("Falha ao iniciar o cartão SD!");
    ESP.restart();
  }

  // Define o diretório raiz
  SD_MMC.mkdir("/photos");
}

void loop() {
  // Verifica se um pulso foi detectado
  if (digitalRead(pinoPulso) == LOW) {
    Serial.println("Pulso detectado!");

    // Acende o LED do flash
    digitalWrite(pinoFlash, HIGH);

    // Captura a foto
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Falha ao capturar a foto!");
      return;
    }

    // Gera o nome do arquivo com base no timestamp
    char fileName[100];
    sprintf(fileName, "/photos/foto_%lu.jpg", millis());

    // Abre o arquivo para gravação
    File file = SD_MMC.open(fileName, FILE_WRITE);
    if (!file) {
      Serial.println("Falha ao abrir o arquivo!");
      return;
    }

    // Grava os dados da imagem no arquivo
    file.write(fb->buf, fb->len);
    file.close();
    Serial.printf("Imagem salva: %s\n", fileName);

    // Libera a memória da imagem
    esp_camera_fb_return(fb);

    // Desliga o LED do flash
    digitalWrite(pinoFlash, LOW);

    // Aguarda um intervalo antes de capturar a próxima foto
    delay(5000);
  }
}