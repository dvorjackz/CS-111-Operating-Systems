// NAME: Jack Zhang
// EMAIL: dvorjackz@ucla.edu
// ID: 004993345

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <math.h>
#include <mraa/aio.h>
#include <mraa/gpio.h>

#define FAHRENHEIT 0
#define CELSIUS 1

// option variables
int log_flag = 0;
int log_fd;
int period = 1;
int scale = FAHRENHEIT;

// sensor pointers
mraa_aio_context tempSensor;
mraa_gpio_context button;

// run flag for the button
sig_atomic_t volatile run_flag = 1;

// temperature sensor setup variables
const int B = 4275; // thermistor B value
const int R0 = 100000;

// global time variables
struct timeval currTime;
struct tm *localTime;

void button_press_handler() {
  char buf[256];
  localTime = localtime(&currTime.tv_sec);
  sprintf(buf,"%02d:%02d:%02d SHUTDOWN\n", localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
  fputs(buf, stdout);
  run_flag = 0;
}

/* // reference: http://wiki.seeedstudio.com/Grove-Temperature_Sensor_V1.2/ */
double getTemp() {
  int reading = mraa_aio_read(tempSensor);
  if (reading == -1) {
    fprintf(stderr, "Error: could not obtain tempeature reading from sensor.\n");
    exit(1);
  }
  double R = 1023.0 / ((double) reading) - 1.0;
  R = R0 * R;
  double temperature = 1.0 / (log(R/R0)/B + 1/298.15) - 273.15;
  return temperature;
}

int main(int argc, char **argv) {
  
  opterr = 0; // suppress automatic stock error message

  static struct option long_options[] = {
					 {"period", required_argument, 0, 'p'},
					 {"scale", required_argument, 0, 's'},
					 {"log", required_argument, 0, 'l'},
					 {0, 0, 0, 0}
  };

  // parse options
  char opt;
  while (run_flag) {
    opt = getopt_long(argc, argv, "", long_options, NULL);
    if (opt == (char) -1) {
      break;
    }
    else if (opt == 'p') {
      period = atoi(optarg);
    }
    else if (opt == 's') {
      char sc = *optarg;
      switch (sc) {
      case 'C':
	scale = CELSIUS;
	break;
      case 'F':
	break;
      default:
	fprintf(stderr, "Incorrect scale option arguments. Please pass in either 'C' or 'F'.\n");
	exit(1);
      }
    }
    else if (opt == (char) 'l') {
      log_flag = 1;
      if ((log_fd = open(optarg, O_CREAT | O_WRONLY, 0666)) == -1) {
	fprintf(stderr, "Could not open/create file for logging purposes.\n");
      }
    }
    else {
      fprintf(stderr, "Error: invalid option or missing argument.\n");
      exit(1);
    }
  }

  // char buffer for temperature sensor output
  char buf[256];

  // initialize the button and temperature sensors
  tempSensor = mraa_aio_init(1);
  if (tempSensor == NULL) {
    fprintf(stderr, "Error: Failed to initialize temperature sensor.\n");
    mraa_deinit();
    exit(1);
  }
  button = mraa_gpio_init(60);
  if (button == NULL) {
    fprintf(stderr, "Error: Failed to initialize button.\n");
    mraa_deinit();
    exit(1);
  }

  // set button to be an input pin
  mraa_gpio_dir(button, MRAA_GPIO_IN);

  // set up a button listener that calls handler when pressed
  mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, &button_press_handler, NULL);

  // for current time
  if ((gettimeofday(&currTime, NULL)) == -1) {
    fprintf(stderr, "Error: could not retrieve time.\n");
  }

  // for next time, determined by user-submitted period value
  time_t nextTime = currTime.tv_sec + period;

  while (run_flag) {
    if ((gettimeofday(&currTime, NULL)) == -1) {
      fprintf(stderr, "Error: could not retrieve time.\n");
    }
    // when one second has elapsed
    if (currTime.tv_sec >= nextTime) {
      double currTemp = getTemp();
      localTime = localtime(&currTime.tv_sec);
      sprintf(buf,"%02d:%02d:%02d %.1f\n", localTime->tm_hour, localTime->tm_min, localTime->tm_sec, currTemp);
      fputs(buf, stdout);
      if (log_flag) {
	dprintf(log_fd, "%s", buf);
      }
      nextTime = currTime.tv_sec + period;
      fprintf(stdout, "%d second(s) have elapsed.\n", period);
    }
  }

  if (mraa_aio_close(tempSensor) != 0) {
    fprintf(stderr, "Error: could not close temperature sensor.\n");
    exit(1);
  }
  if (mraa_gpio_close(button) != 0) {
    fprintf(stderr, "Error: could not close button.\n");
    exit(1);
  }
  
  exit(0);
}
