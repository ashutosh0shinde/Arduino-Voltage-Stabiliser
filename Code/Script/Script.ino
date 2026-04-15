const int voltageReadPin = A0;

//voltage measurement
int ind = 0;
#define ARRLEN 10
float volArr[ARRLEN];
float sum = 0;
float voltage = 0;
float mains = 0;

const double v_change = 0.004312;

//Caklibration of voltage   voltage * (actual * sensed)
float calib_correction = 0.60/0.62;

//Two point calibration 
float x1 = 153.0, x2 = 237.0;
float y1 = 150.0, y2 = 243.0;
float k,c;



void MeasureVoltage();
void SetupVoltage();
float MainsToSense(int ms);

void setup() 
{
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
  Serial.print(voltage,3);
  Serial.print(" ");
  Serial.println(mains);
  delay(100);
}

float MainsToSense(int ms) { return v_change * ms;} 


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
  mains = mains * k + c;// 2 point calib
}
void MeasureVoltage() // taking moving average to smooth out the voltage
{
    sum -= volArr[ind];
    volArr[ind] = analogRead(voltageReadPin) * (1.1/1023.0);
    sum += volArr[ind];
    ind = (ind + 1)%ARRLEN;

    voltage = (sum/ARRLEN) * calib_correction;
    mains = voltage / v_change;
    mains = mains * k + c;// 2 point calib
}
