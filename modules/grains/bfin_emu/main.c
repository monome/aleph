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

#define IN_PORTS 4;
#define OUT_PORTS 4;

jack_port_t *input_ports[IN_PORTS];
jack_port_t *output_ports[OUT_PORTs];

/**
 * The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle.
 *
 * This client does nothing more than copy data from its input
 * port to its output port. It will exit when stopped by 
 * the user (e.g. using Ctrl-C on a unix-ish operating system)
 */
/* #include "fract32_emu.h" */
/* #include "types.h" */
#include "fix.h"
#include "fract32_jack.h"
#include "grain.h"


fract32 jack_sample_to_fract32 (jack_default_audio_sample_t in) {
  return (fract32) (in * ((jack_default_audio_sample_t) FR32_MAX));
}

jack_default_audio_sample_t fract32_to_jack_sample (fract32 in) {
  return ((jack_default_audio_sample_t) in) /
    ((jack_default_audio_sample_t) FR32_MAX);
}

fract32 in[IN_PORTS];
fract32 out[OUT_PORTS];

int process_block (jack_nframes_t nframes, void *arg) {
  jack_default_audio_sample_t *in_jack_block, *out_jack_block;
  int k;

  in_jack_block = jack_port_get_buffer (input_port, nframes);
  out_jack_block = jack_port_get_buffer (output_port, nframes);

  for (i=0; i < IN_PORTS; i++) {
    
  }
  
  for (k=0; k<nframes; k++) {
    int i;
    for(i=0; i < 4; i++)
      in[0] = jack_sample_to_fract32(in[k]);
      module_process_frame();
    out_jack_block[k] = fract32_to_jack_sample(out[0]);
			     }
  /* printf("%d\n", period ); */

  return 0;      
}

void jack_shutdown (void *arg) {
	fprintf(stderr, "JACK shut down, exiting ...\n");
	exit (1);
}

int main (int argc, char *argv[]) {
  module_init();
  const char **ports;
  const char *client_name = "aleph_sim";
  const char *server_name = NULL;
  jack_options_t options = JackNullOption;
  jack_status_t status;

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

  if (jack_connect (client, "jaaa:out_1", jack_port_name (input_port))) {
    fprintf (stderr, "cannot connect input ports\n");
  }
  if (jack_connect (client, "latent:input 1", jack_port_name (input_port))) {
    fprintf (stderr, "cannot connect input ports\n");
  }

  free (ports);
	
  ports = jack_get_ports (client, NULL, NULL,
			  JackPortIsPhysical|JackPortIsInput);
  if (ports == NULL) {
    fprintf(stderr, "no physical playback ports\n");
    exit (1);
  }

  if (jack_connect (client, jack_port_name (output_port), "jaaa:in_1")) {
    fprintf (stderr, "cannot connect output ports\n");
  }
  if (jack_connect (client, jack_port_name (output_port), "latent:input 0")) {
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

