const int voltageReadPin = A0;

int ind = 0;
#define ARRLEN 10
float volArr[ARRLEN];
float sum = 0;

//Caklibration of voltage   voltage * (actual * sensed)
float calib_correction = 0.60/0.62;

float voltage = 0;


void MeasureVoltage();
void SetupVoltage();

void setup() 
{
  Serial.begin(9600);
  analogReference(INTERNAL);
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
    volArr[i] = analogRead(voltageReadPin) * (1.1/1023.0);
    sum += volArr[i];
    delay(10);
  }
  voltage = (sum/ARRLEN) * calib_correction;
}
void MeasureVoltage() // taking moving average to smooth out the voltage
{
    sum -= volArr[ind];
    volArr[ind] = analogRead(voltageReadPin) * (1.1/1023.0);
    sum += volArr[ind];
    ind = (ind + 1)%ARRLEN;

    voltage = (sum/ARRLEN) * calib_correction;
}
