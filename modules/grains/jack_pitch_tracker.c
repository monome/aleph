/** @file latent_client.c
 *
 * @brief This simple client demonstrates the most basic features of JACK
 * as they would be used by many applications.
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <jack/jack.h>
#include <math.h>


jack_port_t *input_port;
jack_port_t *output_port;
jack_client_t *client;

jack_default_audio_sample_t *delay_line;
jack_nframes_t delay_index;
jack_nframes_t latency = 1024;

/**
 * The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle.
 *
 * This client does nothing more than copy data from its input
 * port to its output port. It will exit when stopped by 
 * the user (e.g. using Ctrl-C on a unix-ish operating system)
 */
#include "types.h"
#include "fract32_jack.h"
#include "ricks_tricks.h"

fract32 jack_sample_to_fract32 (jack_default_audio_sample_t in) {
  return (fract32) (in * ((jack_default_audio_sample_t) FR32_MAX));
}

jack_default_audio_sample_t fract32_to_jack_sample (fract32 in) {
  return ((jack_default_audio_sample_t) in) /
    ((jack_default_audio_sample_t) FR32_MAX);
}

/* debug - this is a cleaner osc using floating point math libs (obviously doesn't work on bfin)*/
/* fract32 osc (fract32 phase) { */
/*   float phase_float = 3.141579 * ( (double) phase ) / ( (double) FR32_MAX ); */
/*   return (double) (cos (phase_float) * ( (double) (FR32_MAX / 16) )); */
/* } */


fract32 instantaneousPeriod = hzToDimensionless(1000);
fract32 lastIn = 0.0;
fract32 period = 48;
fract32 phase = 0.0;
int nsamples = 0;

hpf dcBlocker;
lpf adaptiveFilter;

fract32 pitchTrack (fract32 preIn) {
  fract32 in = hpf_next_dynamic(&dcBlocker, preIn, hzToDimensionless(50));
  in = lpf_next_dynamic (&adaptiveFilter, in , FR32_MAX / period);
  if (lastIn <= 0 && in >= 0 && nsamples > 10.0) {
    simple_slew (period, max
		 (min((fract32) nsamples, FR32_MAX / hzToDimensionless(70)),
		  FR32_MAX / hzToDimensionless(2000)),
		 FR32_MAX - 1024);
    nsamples = 0;
  }
  nsamples += 1;
  simple_slew (instantaneousPeriod, period, FR32_MAX - 4096 * 2);
  phase += FR32_MAX / instantaneousPeriod ;
  lastIn = in;
  return osc(phase);
}

fract32 process_frame (fract32 in) {
    /* out[k] = delay_line[delay_index]; */
    /* delay_line[delay_index] = in[k]; */
    /* delay_index = (delay_index + 1) % latency; */
    
    /* fr32_out = hpf(fr32_in); */
    /* fr32_out = mult_fr1x32x32(fr32_in, jack_sample_to_fract32(1.0 / 48.0)); */

    /* fr32_out = lpf(fr32_in, (FR32_MAX / 12)); */
    /* fr32_out = hpf(fr32_in, hzToDimensionless(1000)); */
  return pitchTrack( in );
}

int process_block (jack_nframes_t nframes, void *arg) {
  jack_default_audio_sample_t *in, *out;
  int k;

  in = jack_port_get_buffer (input_port, nframes);
  out = jack_port_get_buffer (output_port, nframes);

  for (k=0; k<nframes; k++) {
    fract32 fr32_in = jack_sample_to_fract32(in[k]);
    out[k] = fract32_to_jack_sample( process_frame ( fr32_in));
			     }
  /* printf("%d\n", period ); */

  return 0;      
}

void arithmetic_tests () {
  printf ("1K = %d\n",mult_fr1x32x32( FR32_MAX,
				      jack_sample_to_fract32(1.0 / 48)));
  printf("0 clipped = %d\n", clip_to_fr32(0));
  printf("1+1 = %d\n", add_fr1x32(1, 1));
  printf("max+max = %d\n", add_fr1x32(FR32_MAX,FR32_MAX));
  printf("max-max = %d\n", sub_fr1x32(FR32_MAX,FR32_MAX));
  printf("max+min = %d\n", add_fr1x32(FR32_MAX,FR32_MIN));
  printf("min+min = %d\n", add_fr1x32(FR32_MIN,FR32_MIN));

  printf("osc(0) = %d\n", osc(0));
  printf("osc(1) = %d\n", osc(1));
  printf("osc(-1) = %d\n", osc(-1));

  printf("osc(max-1) = %d\n", osc(FR32_MAX - 1));
  printf("osc(max) = %d\n", osc(FR32_MAX));
  printf("osc(max+1) = %d\n", osc(FR32_MAX+1));
  printf("osc(min) = %d\n", osc(FR32_MIN));
  printf("osc(min+1) = %d\n", osc(((fract32)FR32_MIN)+1));
  
}  

void
latency_cb (jack_latency_callback_mode_t mode, void *arg)
{
	jack_latency_range_t range;
	if (mode == JackCaptureLatency) {
		jack_port_get_latency_range (input_port, mode, &range);
		range.min += latency;
		range.max += latency;
		jack_port_set_latency_range (output_port, mode, &range);
	} else {
		jack_port_get_latency_range (output_port, mode, &range);
		range.min += latency;
		range.max += latency;
		jack_port_set_latency_range (input_port, mode, &range);
	}
}

/**
 * JACK calls this shutdown_callback if the server ever shuts down or
 * decides to disconnect the client.
 */
void
jack_shutdown (void *arg)
{
	fprintf(stderr, "JACK shut down, exiting ...\n");
	exit (1);
}

void init_dsp () {
  hpf_init(&dcBlocker);
  lpf_init(&adaptiveFilter);
}

int main (int argc, char *argv[]) {
  init_dsp();
  const char **ports;
  const char *client_name = "latent";
  const char *server_name = NULL;
  jack_options_t options = JackNullOption;
  jack_status_t status;
  arithmetic_tests();
  if (argc == 2)
    latency = atoi(argv[1]);

  delay_line = malloc( latency * sizeof(jack_default_audio_sample_t));
  if (delay_line == NULL) {
    fprintf (stderr, "no memory");
    exit(1);
  }

  memset (delay_line, 0, latency * sizeof(jack_default_audio_sample_t));

  /* open a client connection to the JACK server */

  client = jack_client_open (client_name, options, &status, server_name);
  if (client == NULL) {
    fprintf (stderr, "jack_client_open() failed, "
	     "status = 0x%2.0x\n", status);
    if (status & JackServerFailed) {
      fprintf (stderr, "Unable to connect to JACK server\n");
    }
    exit (1);
  }
  if (status & JackServerStarted) {
    fprintf (stderr, "JACK server started\n");
  }
  if (status & JackNameNotUnique) {
    client_name = jack_get_client_name(client);
    fprintf (stderr, "unique name `%s' assigned\n", client_name);
  }

  /* tell the JACK server to call `process()' whenever
     there is work to be done.
  */

  jack_set_process_callback (client, process_block, 0);

  /* tell the JACK server to call `latency()' whenever
     the latency needs to be recalculated.
  */
  if (jack_set_latency_callback)
    jack_set_latency_callback (client, latency_cb, 0);

  /* tell the JACK server to call `jack_shutdown()' if
     it ever shuts down, either entirely, or if it
     just decides to stop calling us.
  */

  jack_on_shutdown (client, jack_shutdown, 0);

  /* display the current sample rate. 
   */

  printf ("engine sample rate: %" PRIu32 "\n",
	  jack_get_sample_rate (client));

  /* create two ports */

  input_port = jack_port_register (client, "input",
				   JACK_DEFAULT_AUDIO_TYPE,
				   JackPortIsInput, 0);
  output_port = jack_port_register (client, "output",
				    JACK_DEFAULT_AUDIO_TYPE,
				    JackPortIsOutput, 0);

  if ((input_port == NULL) || (output_port == NULL)) {
    fprintf(stderr, "no more JACK ports available\n");
    exit (1);
  }

  /* Tell the JACK server that we are ready to roll.  Our
   * process() callback will start running now. */

  if (jack_activate (client)) {
    fprintf (stderr, "cannot activate client");
    exit (1);
  }

  /* Connect the ports.  You can't do this before the client is
   * activated, because we can't make connections to clients
   * that aren't running.  Note the confusing (but necessary)
   * orientation of the driver backend ports: playback ports are
   * "input" to the backend, and capture ports are "output" from
   * it.
   */

  ports = jack_get_ports (client, NULL, NULL,
			  JackPortIsPhysical|JackPortIsOutput);
  if (ports == NULL) {
    fprintf(stderr, "no physical capture ports\n");
    exit (1);
  }

  if (jack_connect (client, ports[0], jack_port_name (input_port))) {
    fprintf (stderr, "cannot connect input ports\n");
  }

  free (ports);
	
  ports = jack_get_ports (client, NULL, NULL,
			  JackPortIsPhysical|JackPortIsInput);
  if (ports == NULL) {
    fprintf(stderr, "no physical playback ports\n");
    exit (1);
  }

  if (jack_connect (client, jack_port_name (output_port), ports[0])) {
    fprintf (stderr, "cannot connect output ports\n");
  }

  free (ports);

  /* keep running until stopped by the user */

#ifdef WIN32
  Sleep (-1);
#else
  sleep (-1);
#endif

  /* this is never reached but if the program
     had some other way to exit besides being killed,
     they would be important to call.
  */

  jack_client_close (client);
  exit (0);
}

