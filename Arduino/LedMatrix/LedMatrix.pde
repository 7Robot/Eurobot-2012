/*
 * LedMatrix.pde
 * Shan Bréham and Martin d'Allens
 */

#define _ row(0);  r++;
#define X row(1); r++;
#define sl column(1); r = 0;
#define el delay(5); t+= 5; column(0); c++;

void setup() {
  for(int i = 1; i <= 12; i++)
    pinMode(i, INPUT);
}

int r;
int c;
int t;
int i = 0;
void loop() {
  t = 0;
  while(t < 500)
    digit(i);
  i = (i + 1) % 10;
}

void digit(int i) {
  c = 0;
  switch(i) {
    case 0:
    sl _ _ _ _ _ _ _ el
    sl _ _ X X X _ _ el
    sl _ X _ _ _ X _ el
    sl _ _ X X X _ _ el
    sl _ _ _ _ _ _ _ el
    break;
    case 1:
    sl _ _ _ _ _ _ _ el
    sl _ X _ _ X _ _ el
    sl _ X X X X X _ el
    sl _ X _ _ _ _ _ el
    sl _ _ _ _ _ _ _ el
    break;
    case 2:
    sl _ _ _ _ _ _ _ el
    sl _ X X _ _ X _ el
    sl _ X _ X _ X _ el
    sl _ X _ _ X _ _ el
    sl _ _ _ _ _ _ _ el
    break;
    case 0:
    sl _ _ _ _ _ _ _ el
    sl _ X _ _ _ X _ el
    sl _ X _ X _ X _ el
    sl _ _ X _ X X _ el
    sl _ _ _ _ _ _ _ el
    break;
    case 0:
    sl _ _ _ _ _ _ _ el
    sl _ _ _ X X X _ el
    sl _ _ _ X _ _ _ el
    sl _ X X X X X _ el
    sl _ _ _ _ _ _ _ el
    break;
  }
}

void column(bool on)
{
  if(on)
  {
    pinMode(c + 8, OUTPUT);
    digitalWrite(c + 8, HIGH);
  }
  else
  {
    pinMode(c + 8, INPUT);
  }
}

void row(bool on)
{
  if(on)
  {
    pinMode(r + 1, OUTPUT);
    digitalWrite(r + 1, LOW);
  }
  else
  {
    pinMode(r + 1, INPUT);
  }
}
