#include <math.h>
#include <stdio.h>

#define PI 3.141569

float soft_clip_2(float lim, float x) {
  float b = 1 / (1 - lim);//2
  float a = b / -2;//-1
  float c = lim * (1 - b - a * lim);// -0.25
  float norm = a + b + c;
  norm = 1;
  /* printf("%f, %f, %f\n", a, b, c); */
  if (x >= lim) {
    return (a * x * x + b * x + c) / norm;
  } else if (x <= -lim) {
    return (- a * x * x + b * x - c) / norm;
  } else {
    return x / norm;
  }
}

float soft_clip_4(float lim, float x) {
  float b = 1 / (1 - lim * lim * lim);//2
  float a = b / -4;//-1
  float c = lim * (1 - b - a * lim * lim * lim);// -0.25
  float norm = a + b + c;
  norm = 1;
  /* printf("%f, %f, %f\n", a, b, c); */
  if (x >= lim) {
    return (a * x * x * x * x + b * x + c) / norm;
  } else if (x <= -lim) {
    return (- a * x * x * x * x + b * x - c) / norm;
  } else {
    return x / norm;
  }
}

float soft_clip_6(float lim, float x) {
  float b = 1 / (1 - lim * lim * lim * lim * lim);//2
  float a = b / -6;//-1
  float c = lim * (1 - b - a * lim * lim * lim * lim * lim);// -0.25
  float norm = a + b + c;
  norm = 1;
  /* printf("%f, %f, %f\n", a, b, c); */
  if (x >= lim) {
    return (a * x * x * x * x * x * x + b * x + c) / norm;
  } else if (x <= -lim) {
    return (- a * x * x * x * x * x * x + b * x - c) / norm;
  } else {
    return x / norm;
  }
}

void main () {
  float x;
  float my_sig;
  float clip_lim = 0;
  for(x=0; x < 1.0; x += 0.001) {
    my_sig = 0.1 * sin(2 * PI * x * 40) + 0.9 * sin(2 * PI * x);
    printf("%f %f %f %f\n", my_sig,
	   soft_clip_2(clip_lim, my_sig),
	   soft_clip_4(clip_lim, my_sig),
	   soft_clip_6(clip_lim, my_sig));
  }
}
