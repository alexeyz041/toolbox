
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <stdint.h>

#include "rosbag.hpp"

RosBagReader  br;

GST_DEBUG_CATEGORY (appsrc_pipeline_debug);

#define GST_CAT_DEFAULT appsrc_pipeline_debug


typedef struct {
  GstElement *pipeline;
  GstElement *appsrc;
  GMainLoop *loop;
  guint sourceid;
  //GTimer *timer;
  uint64_t num_samples;
} CustomData;



static gboolean read_data(CustomData *app)
{
guint len;
GstFlowReturn ret;
gdouble ms;
GstMapInfo map;

//    ms = g_timer_elapsed(app->timer, NULL);
//    if (ms > 0.1)
    {
        GstBuffer *buffer = NULL;
        gboolean ok = TRUE;

        GST_DEBUG("read_data timer");

        Frame *frame = br.get_next_frame();

        buffer = gst_buffer_new_and_alloc(frame->data.size());
        GST_BUFFER_TIMESTAMP (buffer) = gst_util_uint64_scale(frame->timestamp, GST_SECOND, 1000000);
        GST_BUFFER_DURATION (buffer) = gst_util_uint64_scale(frame->duration, GST_SECOND, 1000000);

        gst_buffer_map (buffer, &map, GST_MAP_WRITE);

        memcpy(map.data, frame->data.data(), frame->data.size());
        gst_buffer_unmap(buffer, &map);

        g_signal_emit_by_name (app->appsrc, "push-buffer", buffer, &ret);
        gst_buffer_unref (buffer);

        if (ret != GST_FLOW_OK) {
            /* some error, stop sending data */
            GST_DEBUG ("some error");
            ok = FALSE;
        }

//        g_timer_start(app->timer);
        return ok;
    }

    //  g_signal_emit_by_name (app->appsrc, "end-of-stream", &ret);
    return FALSE;
}


/* This signal callback is called when appsrc needs data, we add an idle handler
 * to the mainloop to start pushing data into the appsrc */
static void start_feed (GstElement * pipeline, guint size, CustomData *app)
{
  if (app->sourceid == 0) {
    GST_DEBUG ("start feeding");
    app->sourceid = g_idle_add ((GSourceFunc) read_data, app);
  }
}


/* This callback is called when appsrc has enough data and we can stop sending.
 * We remove the idle handler from the mainloop */
static void stop_feed (GstElement * pipeline, CustomData *app)
{
  if (app->sourceid != 0) {
    GST_DEBUG ("stop feeding");
    g_source_remove (app->sourceid);
    app->sourceid = 0;
  }
}


static gboolean bus_message (GstBus * bus, GstMessage * message, CustomData *app)
{
  GST_DEBUG ("got message %s",
      gst_message_type_get_name (GST_MESSAGE_TYPE (message)));

  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR: {
        GError *err = NULL;
        gchar *dbg_info = NULL;

        gst_message_parse_error (message, &err, &dbg_info);
        g_printerr ("ERROR from element %s: %s\n", GST_OBJECT_NAME (message->src), err->message);
        g_printerr ("Debugging info: %s\n", (dbg_info) ? dbg_info : "none");
        g_error_free (err);
        g_free (dbg_info);
        g_main_loop_quit (app->loop);
        break;
    }

    case GST_MESSAGE_EOS:
      GST_DEBUG("EOF");
      g_main_loop_quit (app->loop);
      break;

    default:
      break;
  }
  return TRUE;
}



int main (int argc, char *argv[])
{
CustomData data;
CustomData *pData = &data;
GError *err = NULL;
GstBus *bus = NULL;
GstCaps *caps = NULL;

    br.setFolder(argv[1]);

    memset(&data, 0, sizeof(data));
    gst_init(&argc, &argv);

    GST_DEBUG_CATEGORY_INIT (appsrc_pipeline_debug, "appsrc-pipeline", 0, "appsrc pipeline example");

    pData->pipeline = gst_parse_launch("appsrc name=mysource ! "
         "jpegdec ! "
         "videoconvert !"
         "timeoverlay halignment=right valignment=bottom shaded-background=true font-desc=\"Sans, 41\" ! "
         "autovideosink", &err);
    if(err) {
        g_print ("Error creating pipeline: %s\n", err->message);
        g_clear_error (&err);
        return -1;
    }
    g_assert(pData->pipeline);

    bus = gst_pipeline_get_bus(GST_PIPELINE(pData->pipeline));
    g_assert(bus);

    gst_bus_add_watch(bus, (GstBusFunc) bus_message, pData);

    pData->appsrc = gst_bin_get_by_name (GST_BIN(pData->pipeline), "mysource");
    g_assert(pData->appsrc);
    g_signal_connect (pData->appsrc, "need-data", G_CALLBACK (start_feed), pData);
    g_signal_connect (pData->appsrc, "enough-data", G_CALLBACK (stop_feed), pData);

    caps = gst_caps_new_simple ("image/jpeg",
        "width", G_TYPE_INT, 1920,
        "height", G_TYPE_INT, 1080,
        "framerate", GST_TYPE_FRACTION, 12500, 1000,
        NULL);

    g_object_set(pData->appsrc, "caps", caps, "format", GST_FORMAT_TIME, NULL);

    gst_element_set_state (pData->pipeline, GST_STATE_PLAYING);

    pData->loop = g_main_loop_new (NULL, TRUE);
//    pData->timer = g_timer_new();
    g_main_loop_run(pData->loop);

    gst_element_set_state (pData->pipeline, GST_STATE_NULL);

    gst_object_unref (bus);
    g_main_loop_unref (pData->loop);

    return 0;
}

