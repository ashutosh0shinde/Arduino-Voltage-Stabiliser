const int voltageReadPin = A0;

// voltage measurement
int ind = 0;
#define ARRLEN 10
float volArr[ARRLEN];
float sum = 0;
float v_sense = 0;
float mains = 0;

const double v_change = 0.004312;

// Calibration
float calib_correction = 0.60 / 0.62;

// Two point calibration
float x1 = 153.0, x2 = 237.0;
float y1 = 150.0, y2 = 243.0;
float k, c;

int mode = 1;
int m = 1;

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
void Boost(int i);

void setup() 
{
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(A4, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  pinMode(12, OUTPUT); // fixed
  pinMode(13, OUTPUT);

  Serial.begin(9600);
  analogReference(INTERNAL);

  // calibration
  k = (x2 - x1) / (y2 - y1);
  c = x1 - (y1 * k);

  SetupVoltage();
}

void loop()
{
  MeasureVoltage();

  Serial.print(v_sense, 3);
  Serial.print(" ");
  Serial.print(mains);
  Serial.print(" ");
  Serial.print(boostMode);
  Serial.print(" ");
  Serial.print(digitalRead(A1));
  Serial.print(" ");
  Serial.print(digitalRead(A2));
  Serial.print(" ");
  Serial.print(digitalRead(A3));
  Serial.print(" ");
  Serial.print(digitalRead(A4));
  Serial.print(" ");
  Serial.println(digitalRead(A5));

  digitalWrite(13, digitalRead(A1) == 0);

  SetState();

  delay(100);
}

void SetState()
{
  // reset states
  r1 = r2 = r3 = r4 = false;
  l1 = l2 = l3 = l4 = false;

  digitalWrite(12, digitalRead(A5));

  m = mode; // default fallback

  // -------- MANUAL MODE --------
  if (digitalRead(A2) == 0)
  {
    int a5 = digitalRead(A5);
    int a4 = digitalRead(A4);
    int a3 = digitalRead(A3);

    if (a3 && a4 && a5) m = 1;
    else if (!a3 && !a4 && !a5) m = 0;
    else if (!a3 && a4 && a5) m = 2;
    else if (a3 && !a4 && a5) m = 3;
    else if (a3 && a4 && !a5) m = 4;
  }
  // -------- AUTO MODE --------
  else
  {
    switch (mode)
    {
      case 0:
        if (mains > 50) m = 4;
        break;

      case 1:
        if (mains < 202) m = 2;
        break;

      case 2:
        if (mains > 206) m = 1;
        else if (mains < 189) m = 3;
        break;

      case 3:
        if (mains > 194) m = 2;
        else if (mains < 174) m = 4;
        break;

      case 4:
        if (mains > 177) m = 3;
        else if (mains < 42) m = 0;
        break;
    }
  }

  Boost(m);

  if (m != mode)
  {
    mode = m;
    SetRelay();
  }

  SetLed();
}

void Boost(int i)
{
  r1 = r2 = r3 = r4 = false;
  l1 = l2 = l3 = l4 = false;

  switch (i)
  {
    case 0: // disconnected
      r1 = true;
      l1 = l2 = l3 = l4 = true;
      boostMode = "DISC";
      break;

    case 1: // no boost
      l1 = true;
      boostMode = "NOBOOST";
      break;

    case 2: // low
      r1 = true;
      r4 = true;
      l2 = true;
      boostMode = "LOW";
      break;

    case 3: // medium
      r1 = true;
      r2 = true;
      r3 = true;
      l3 = true;
      boostMode = "MED";
      break;

    case 4: // high
      r1 = true;
      r2 = true;
      r4 = true;
      l4 = true;
      boostMode = "HIGH";
      break;
  }
}

void SetRelay()
{
  // turn all OFF first
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);

  delay(50);

  // then turn ON required
  if (r1) digitalWrite(7, HIGH);
  if (r2) digitalWrite(8, HIGH);
  if (r3) digitalWrite(9, HIGH);
  if (r4) digitalWrite(10, HIGH);
}

void SetLed()
{
  digitalWrite(2, l1);
  digitalWrite(3, l2);
  digitalWrite(4, l3);
  digitalWrite(5, l4);
}

void SetupVoltage()
{
  sum = 0;
  for (int i = 0; i < ARRLEN; i++)
  {
    float reading = analogRead(voltageReadPin);
    volArr[i] = (reading * 1.1) / 1023.0;
    sum += volArr[i];
    delay(10);
  }

  v_sense = (sum / ARRLEN) * calib_correction;
  mains = (v_sense / v_change) * k + c;
}

void MeasureVoltage()
{
  sum -= volArr[ind];

  float reading = analogRead(voltageReadPin);
  volArr[ind] = (reading * 1.1) / 1023.0;

  sum += volArr[ind];
  ind = (ind + 1) % ARRLEN;

  v_sense = (sum / ARRLEN) * calib_correction;
  mains = (v_sense / v_change) * k + c;
}
