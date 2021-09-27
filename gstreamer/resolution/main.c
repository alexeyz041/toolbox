#include <stdio.h>
#include <gst/gst.h>

int width;
int height;
int framerate_n;
int framerate_d;


/* Functions below print the Capabilities in a human-friendly format */
static gboolean print_field (GQuark field, const GValue * value, gpointer pfx) {
  gchar *str = gst_value_serialize (value);
  const gchar *s1, *p;
  g_print ("%s  %15s: %s\n", (gchar *) pfx, g_quark_to_string (field), str);

  s1 = g_quark_to_string(field);
  if(strcmp(s1, "width") == 0) {
      width = atoi(str);
  } else if(strcmp(s1, "height") == 0) {
      height = atoi(str);
  } else if(strcmp(s1, "framerate") == 0) {
      framerate_n = atoi(str);
      p = strchr(str, '/');
      if(p) framerate_d = atoi(p+1); else framerate_d = 1;
  }

  g_free (str);
  return TRUE;
}


static void print_caps (const GstCaps * caps, const gchar * pfx) {
  guint i;

  g_return_if_fail (caps != NULL);

  if (gst_caps_is_any (caps)) {
    g_print ("%sANY\n", pfx);
    return;
  }
  if (gst_caps_is_empty (caps)) {
    g_print ("%sEMPTY\n", pfx);
    return;
  }

  for (i = 0; i < gst_caps_get_size (caps); i++) {
    GstStructure *structure = gst_caps_get_structure (caps, i);

    g_print ("%s%s\n", pfx, gst_structure_get_name (structure));
    gst_structure_foreach (structure, print_field, (gpointer) pfx);
  }
}

/* Prints information about a Pad Template, including its Capabilities */
static void print_pad_templates_information (GstElementFactory * factory)
{
  const GList *pads;
  GstStaticPadTemplate *padtemplate;

  g_print ("Pad Templates for %s:\n", gst_element_factory_get_longname (factory));
  if (!gst_element_factory_get_num_pad_templates (factory)) {
    g_print ("  none\n");
    return;
  }

  pads = gst_element_factory_get_static_pad_templates (factory);
  while (pads) {
    padtemplate = pads->data;
    pads = g_list_next (pads);

    if (padtemplate->direction == GST_PAD_SRC)
      g_print ("  SRC template: '%s'\n", padtemplate->name_template);
    else if (padtemplate->direction == GST_PAD_SINK)
      g_print ("  SINK template: '%s'\n", padtemplate->name_template);
    else
      g_print ("  UNKNOWN!!! template: '%s'\n", padtemplate->name_template);

    if (padtemplate->presence == GST_PAD_ALWAYS)
      g_print ("    Availability: Always\n");
    else if (padtemplate->presence == GST_PAD_SOMETIMES)
      g_print ("    Availability: Sometimes\n");
    else if (padtemplate->presence == GST_PAD_REQUEST)
      g_print ("    Availability: On request\n");
    else
      g_print ("    Availability: UNKNOWN!!!\n");

    if (padtemplate->static_caps.string) {
      GstCaps *caps;
      g_print ("    Capabilities:\n");
      caps = gst_static_caps_get (&padtemplate->static_caps);
      print_caps (caps, "      ");
      gst_caps_unref (caps);

    }

    g_print ("\n");
  }
}

/* Shows the CURRENT capabilities of the requested pad in the given element */
static void print_pad_capabilities (GstElement *element, gchar *pad_name) {
  GstPad *pad = NULL;
  GstCaps *caps = NULL;

  /* Retrieve pad */
  pad = gst_element_get_static_pad (element, pad_name);
  if (!pad) {
    g_printerr ("Could not retrieve pad '%s'\n", pad_name);
    return;
  }

  /* Retrieve negotiated caps (or acceptable caps if negotiation is not finished yet) */
  caps = gst_pad_get_current_caps (pad);
  if (!caps)
    caps = gst_pad_query_caps (pad, NULL);

  /* Print and free */
  g_print ("Caps for the %s pad:\n", pad_name);
  print_caps (caps, "      ");
  gst_caps_unref (caps);
  gst_object_unref (pad);
}


static int ends_with(const char *str, const char *pat)
{
size_t len = strlen(str);
size_t pat_len = strlen(pat);
    return strcmp(str+len-pat_len,pat) == 0;
}


int get_resolution(const char *fnm, int *w, int *h, int *fr_n, int *fr_d)
{
GstElement *pipeline;
GstElement *dec;
GstBus *bus;
GstMessage *msg;
GstStateChangeReturn ret;
gboolean terminate = FALSE;
char pipe[1024] = {0};

    if(ends_with(fnm, ".mp4")) {
        snprintf(pipe, sizeof(pipe), "filesrc location=%s ! qtdemux ! h264parse ! avdec_h264 ! fakesink name=dec", fnm);
    } else if(ends_with(fnm, ".webm")) {
        snprintf(pipe, sizeof(pipe), "filesrc location=%s ! matroskademux ! vp8dec ! fakesink name=dec", fnm);
    } else {
        gst_print("Unsupported media file\n");
        return -1;
    }

    pipeline = gst_parse_launch(pipe, NULL);
    if(!pipeline) {
        gst_print("can't construct pipeline\n");
        return -1;
    }
    dec = gst_bin_get_by_name(GST_BIN(pipeline), "dec");
    if(!dec) {
        gst_print("can't find decoder\n");
        return -1;
    }

    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if(ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Unable to set the pipeline to the playing state (check the bus for error messages).\n");
    }

    bus = gst_element_get_bus(pipeline);
    do {
        msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS | GST_MESSAGE_STATE_CHANGED);

        if (msg != NULL) {
            GError *err;
            gchar *debug_info;

            switch (GST_MESSAGE_TYPE (msg)) {
                case GST_MESSAGE_ERROR:
                    gst_message_parse_error (msg, &err, &debug_info);
                    g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
                    g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
                    g_clear_error (&err);
                    g_free (debug_info);
                    terminate = TRUE;
                    break;

               case GST_MESSAGE_EOS:
                    g_print ("End-Of-Stream reached.\n");
                    terminate = TRUE;
                    break;

               case GST_MESSAGE_STATE_CHANGED:
                    /* We are only interested in state-changed messages from the pipeline */
                    if(GST_MESSAGE_SRC(msg) == GST_OBJECT (pipeline)) {
                        GstState old_state, new_state, pending_state;
                        gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
                        g_print("\nPipeline state changed from %s to %s:\n",  gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));

                        /* Print the current capabilities of the sink element */
                        if(new_state == GST_STATE_PLAYING) {
                            print_pad_capabilities(dec, "sink");
                            terminate = TRUE;
                        }
                    }
                    break;

               default:
                    /* We should not reach here because we only asked for ERRORs, EOS and STATE_CHANGED */
                    g_printerr ("Unexpected message received.\n");
                    break;
            }
            gst_message_unref (msg);
        }
    } while (!terminate);

    gst_object_unref(bus);
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    gst_object_unref(dec);

    *w = width;
    *h = height;
    *fr_n = framerate_n;
    *fr_d = framerate_d;
    return 0;
}


int main(int argc, char *argv[])
{
int w,h,n,d;
    gst_init (&argc, &argv);
    if(get_resolution(argv[1], &w, &h, &n, &d) == 0) {
        gst_print("%dx%d %d/%d\n", w, h, n, d);
    }
    return 0;
}
