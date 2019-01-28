// Time visualizer for UNO.

#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library

// Similar to F(), but for PROGMEM string pointers rather than literals
#define F2(progmem_ptr) (const __FlashStringHelper *)progmem_ptr

#define CLK 11  // MUST be on PORTB! (Use pin 11 on Mega)
#define LAT 10
#define OE  9
#define A   A0
#define B   A1
#define C   A2
#define D   A3

/* Pin Mapping:
 *  Sig   Uno  Mega
 *  R0    2    24
 *  G0    3    25
 *  B0    4    26
 *  R1    5    27
 *  G1    6    28
 *  B1    7    29
 */

// Note "false" for double-buffering to consume less memory, as we're on an UNO.
RGBmatrixPanel matrix(A, B, C,  D,  CLK, LAT, OE, false);


#define AUDIO_PIN A4
#define GAIN_PIN  A5

int gain=1;

#define SAMPLE_SIZE 32
int sample[SAMPLE_SIZE] = {0};

void setup() 
{
  Serial.begin(9600);
  matrix.begin();
}

void collect_samples( void )
{
  int i;
  for (i = 0; i < SAMPLE_SIZE; i++)
  {
    sample[i] = analogRead(AUDIO_PIN);
  }
}

#define SAMPLE_BIAS 512
#define GAIN        8

// Mapped sample should give a number between 0 and 31
int map_sample( int input )
{
  int mapped_sample;

  // start by taking out DC bias.  This will make negative #s...
  mapped_sample = input - SAMPLE_BIAS;

  // add in gain.
  mapped_sample = mapped_sample / gain;
  
  // center on 16.
  mapped_sample = mapped_sample + 16;

  // and clip.
  if (mapped_sample > 31) mapped_sample = 31;
  if (mapped_sample < 0) mapped_sample = 0;

  return mapped_sample;
}

void read_gain( void )
{
   int raw_gain;

   raw_gain = analogRead(GAIN_PIN);
   gain = map(raw_gain, 0, 1023, 1, 32);
}


void show_samples_lines( void )
{
  int x;
  int y;
  int last_x=0;
  int last_y=16;

  matrix.fillScreen(0);
  
  for (x=0; x < SAMPLE_SIZE; x++)
  {
    y=map_sample(sample[x]);
    matrix.drawLine(last_x,last_y,x,y,matrix.Color333(0,0,1));
    last_x = x;
    last_y = y;
  }
}

void loop() 
{
  read_gain();
  collect_samples();
  show_samples_lines();

  //delay(50);
  
  matrix.swapBuffers(true);
}
