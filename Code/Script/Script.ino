const int voltageReadPin = A0;

//voltage measurement
int ind = 0;
#define ARRLEN 10
float volArr[ARRLEN];
float sum = 0;
float v_sense = 0;
float mains = 0;

const double v_change = 0.004312;

//Caklibration of voltage   voltage * (actual * sensed)
float calib_correction = 0.60/0.62;

//Two point calibration 
float x1 = 153.0, x2 = 237.0;
float y1 = 150.0, y2 = 243.0;
float k,c;

bool err = false;

bool r1 = false;
bool r2 = false;
bool r3 = false;
bool r4 = false;

bool l1 = false;
bool l2 = false;
bool l3 = false;
bool l4 = false;

String boostMode;

void MeasureVoltage();
void SetupVoltage();
void SetState();
void SetRelay();
void SetLed();
void Boost(int i); // 0-disc 1-noboost 2-low 3-med 4-high

void setup() 
{
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
  pinMode(A4,INPUT);
  pinMode(A5,INPUT);
  
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);

  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);

  pinMode(12,OUTPUT);
  
  Serial.begin(9600);
  analogReference(INTERNAL);

//Two point calibration
  k = (x2-x1)/(y2-y1); //slope
  c = x1 - (y1 * k); //offset
  
  SetupVoltage();
}

void loop()
{
  MeasureVoltage();
  Serial.print(v_sense,3);
  Serial.print(" ");
  Serial.print(mains);
  Serial.print(" ");
  Serial.println(boostMode);
  delay(100);
}
void SetState()
{
  r1 = false;
  r2 = false;
  r3 = false;
  r4 = false;
  
  l1 = false;
  l2 = false;
  l3 = false;
  l4 = false;

  if(digitalRead(A5)) digitalWrite(12,HIGH);
  else digitalWrite(12,LOW);

  if(digitalRead(A4))
  {
    
  }
  else
  {
    if(mains > 206)
    {
      Boost(1);
    }
    else if(mains > 192)
    {
      Boost(2);
    }
    else if(mains > 174)
    {
      Boost(3);
    }
    else
    {
      Boost(4);
    }
  }

  if(r3 && r4)
  {
    Boost(0);
    l1 = false;
    l2 = false;
  }
  SetRelay();
  SetLed();
}
void Boost(int i)
{
  r1 = false;
  r2 = false;
  r3 = false;
  r4 = false;
  
  l1 = false;
  l2 = false;
  l3 = false;
  l4 = false;
  
  switch(i)
  {
    case 0: //disconnected
        r1 = true;

        l1 = true;
        l2 = true;
        l3 = true;
        l4 = true;
        boostMode = "DISC";
        break;
        
    case 1:  //no boost
        l1 = true;
        boostMode = "NOBOOST";
       break;
       
    case 2: // low boost
        r1 = true;
        r4 = true;
        
        l2 = true;
        boostMode = "LOW";
        break;
        
    case 3: //medium boost
        r1 = true;
        r2 = true;
        r3 = true;

        l3 = true;
        boostMode = "MED";
        break;
        
    case 4: //high boost
        r1 = true;
        r2 = true;
        r4 = true;

        l4 = true;
        boostMode = "HIGH";
        break;
    default:
        break;
  }
}
void SetRelay()
{ 
  digitalWrite(7,LOW);
  delay(10);
  
  if(r2) digitalWrite(8,HIGH);
  else digitalWrite(8,LOW);
  
  if(r3) digitalWrite(9,HIGH);
  else digitalWrite(9,LOW);
  
  if(r4) digitalWrite(10,HIGH);
  else digitalWrite(10,LOW);

  delay(10);
  if(r1) digitalWrite(7,HIGH);
  else digitalWrite(7,LOW);
}
void SetLed()
{
  if(l1) digitalWrite(2,HIGH);
  else digitalWrite(2,LOW);

  if(l2) digitalWrite(3,HIGH);
  else digitalWrite(3,LOW);

  if(l3) digitalWrite(4,HIGH);
  else digitalWrite(4,LOW);

  if(l4) digitalWrite(5,HIGH);
  else digitalWrite(5,LOW);
}
void SetupVoltage()
{
  sum = 0;
  for(int i = 0;i < ARRLEN; i++)
  {
    volArr[i] = analogRead(voltageReadPin) * (1.1/1023.0);
    sum += volArr[i];
    delay(10);
  }
  v_sense = (sum/ARRLEN) * calib_correction;
  mains = v_sense / v_change;
  mains = mains * k + c;// 2 point calib
}
void MeasureVoltage() // taking moving average to smooth out the voltage
{
    sum -= volArr[ind];
    volArr[ind] = analogRead(voltageReadPin) * (1.1/1023.0);
    sum += volArr[ind];
    ind = (ind + 1)%ARRLEN;

    v_sense = (sum/ARRLEN) * calib_correction;
    mains = v_sense / v_change;
    mains = mains * k + c;// 2 point calib
}
