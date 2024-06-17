#include <U8glib.h>

// Definindo as portas
const int potPin = A0; 
int led = 12;
int trigger = 8;
int echo = 7;
float dist = 0;
const int larguraMaxOled = 128; //armazena a largura máxima do display


//Criação de um buffer, que é um tipo de array, que armazena dados de maneira temporária
//Ele auxiliará a construção dos batimentos cardíacos no display Oled, promovendo a sincronização e suavidade nessa construção
int bufferBatimento[larguraMaxOled];

// Criação do objeto para o display oled
U8GLIB_SSD1306_128X64 oled(U8G_I2C_OPT_NONE); 

void setup() {
  // Iniciando os componentes
  pinMode(led, OUTPUT);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  Serial.begin(9600);

//Inicialização do buffer de batimento cardíaco
  for(int i = 0; i < larguraMaxOled; i++) {
    bufferBatimento[i] = 30;
    //configura o inicio de todos os valores do buffer para 30
    //esse valor corresponde ao posicionamento em Y, colocando-os ao meio
  }

  // Definindo uma fonte suportada
  oled.setFont(u8g_font_unifont);
}

//Função para atualizar os valores do buffer 
void updateBuffer(int novoValor){
  //realiza o deslocamento dos valores dentro do buffer para a esquerda
  //dessa forma, criaremos a animação do batimento cardíaco
  for(int i = 0; i < larguraMaxOled - 1; i++){
    bufferBatimento[i] = bufferBatimento[i + 1];
  }
  //adiciona o novo valor no final do buffer 
  // e mapeia o valor do batimento cardíaco para o tamanho da altura do display(64 pixels)
  bufferBatimento[larguraMaxOled - 1] = map(novoValor,60,150,60,0);

  //no mapeamento, 60 representa o valor mais baixo, pois ele posiciona os elementos na parte inferior do display
  //detalhe: como há um print do BPM, para que os elementos não se sobreponham, embora o tamanho do display seja 64 pixels, mapeamos para 60 para evitar a sobreposição
  //enquanto isso, quanto mais próximo de 0, o elemento será posicionado mais para a parte superior
}

//Função para desenhar o batimento cardíaco no display OLED
//ela fará as linhas que representam a variação do batimento cardíaco ao longo do tempo
void desenharBatimento(){
  for(int i=0; i < larguraMaxOled - 1; i++){
    oled.drawLine(i, bufferBatimento[i], i+1, bufferBatimento[i+1]);
  }
}

void loop() {

  // Atribuindo o valor captado pelo potenciômetro para a variável que
  // irá armazená-lo e fazendo um mapeamento do valor para transformar em bpm
  int potValor = analogRead(potPin);  
  int mapValor = map(potValor, 0, 1023, 60, 150);

//chama a função para atualizar o buffer, adicionando o valor mapeado pelo potenciometro
  updateBuffer(mapValor);
  
  // Atualiza o display OLED
  oled.firstPage();
  do {
    desenharBatimento();
    oled.setPrintPos(0, 63); 
    oled.print("BPM: ");
    oled.print(mapValor);
  } while (oled.nextPage());


   // Inicialização do ultrassônico
  digitalWrite(trigger, LOW);
  delayMicroseconds(5);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
 
  // Echo captura o pulso enviado pelo trigger
  dist = pulseIn(echo, HIGH);
  dist = dist / 58.0;
  //Serial.println(dist);
  if (dist < 300) {
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }

  delayMicroseconds(1);
}