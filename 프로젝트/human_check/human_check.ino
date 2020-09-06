#include <IRremote.h>
#include<HX711.h>


#define MAX 3

/*핀 설정*/
const int load_cell_DOUT_pin1 = 2;
const int load_cell_SCK_pin1 = 3;
const int load_cell_DOUT_pin2 = 4;
const int load_cell_SCK_pin2 = 5;
const int load_cell_DOUT_pin3 = 6;
const int load_cell_SCK_pin3 = 7;

/*HX711에서 읽을 값 저장*/
long reading[MAX] = {0, 0, 0};

/*인원수*/
int count = 0;

/*로드셀 켈리브레이션 보정 값*/
float calibration_factor = -200000;

/*인원 착석 유무 배열*/
boolean seat[MAX] = {false, false, false};

/*무게 센서 객체 생성*/
HX711 hx[MAX];

IRsend ir;

void setup(){
  Serial.begin(9600);
  //객체 핀 설정
  hx[0].begin(load_cell_DOUT_pin1, load_cell_SCK_pin1);
  hx[1].begin(load_cell_DOUT_pin2, load_cell_SCK_pin2);
  hx[2].begin(load_cell_DOUT_pin3, load_cell_SCK_pin3);
  
  for(int i = 0; i < MAX; i++)
  {
    hx[i].set_scale();
    hx[i].tare();
  }
}

void loop(){
  int i = 0; //반복문 변수
  
  //set_scale 켈리브레이션 백터값 설정
  for(i = 0; i < MAX; i++)
  {
    hx[i].set_scale(calibration_factor);
  }
  
  //각 hx711로부터 값 가져와서 무게 계산 후 착석 인원 값 변경
  for(i = 0; i < MAX; i++)
  {
    if(hx[i].is_ready())
    {
      reading[i] = hx[i].read();
      
      if(abs(reading[i]) > 1000000) // 8kg "1417325"
        seat[i] = true;
      else
        seat[i] = false;
    }
  }
  
  for(i=0; i<MAX;i++)
  {
    if(seat[i] == true)
    {
      count = count + 1;
    }
  }
  
  Serial.println(count);

  for(int i = 0;i<3;i++)
  {
     ir.sendNEC(count,32);
     delay(50);
  }
  count = 0;
}
