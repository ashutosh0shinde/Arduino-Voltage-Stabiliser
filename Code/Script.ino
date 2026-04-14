const int voltageReadPin = A5;

int ind = 0;
#define ARRLEN 10
float volArr[ARRLEN];
float sum = 0;

float voltage = 0;


void MeasureVoltage();
void SetupVoltage();

void setup() 
{
  Serial.begin(9600);
  SetupVoltage();

}

void loop()
{
  MeasureVoltage();
  Serial.println(voltage);
  delay(100);
}


void SetupVoltage()
{
  sum = 0;
  for(int i = 0;i < ARRLEN; i++)
  {
    volArr[i] = analogRead(voltageReadPin) * (4.94/1023.0);
    sum += volArr[i];
    delay(10);
  }
  voltage = sum/ARRLEN;
}
void MeasureVoltage() // taking moving average to smooth out the voltage
{
    sum -= volArr[ind];
    volArr[ind] = analogRead(voltageReadPin) * (4.94/1023.0);
    sum += volArr[ind];
    ind = (ind + 1)%ARRLEN;

    voltage = sum/ARRLEN;
}
