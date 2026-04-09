const int voltageReadPin = A0;

int ind = 0;
float volArr[20];
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
  delay(50);
}


void SetupVoltage()
{
  for(int i = 0;i < 20; i++)
  {
    volArr[i] = analogRead(voltageReadPin) * (5.0/1023.0);
    sum += volArr[i];
    delay(10);
  }
  voltage = sum/20.0;
}
void MeasureVoltage() // taking moving average to smooth out the voltage
{
    sum -= volArr[ind];
    volArr[ind] = analogRead(voltageReadPin) * (5.0/1023.0);
    sum += volArr[ind];
    ind = (ind + 1)%20;

    voltage = sum/20;
}
