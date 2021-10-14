#include <stdio.h>
#include <gst/gst.h>

gboolean check_feature(GstPlugin *plugin, const char *feature_name)
{
GList *features, *orig_features;
gboolean found = FALSE;
    orig_features = features = gst_registry_get_feature_list_by_plugin(gst_registry_get(), gst_plugin_get_name(plugin));
    while(features) {
      GstPluginFeature *feature = NULL;
      if(features->data) {
          feature = GST_PLUGIN_FEATURE(features->data);
          if(GST_IS_ELEMENT_FACTORY(feature)) {
              char *name = gst_plugin_feature_get_name(feature);
              if(name && strcmp(feature_name, name) == 0) {
                  found = TRUE;
                  break;
              }
          }
      }
      features = g_list_next(features);
    }

    gst_plugin_feature_list_free(orig_features);
    return found;
}


gboolean check_plugins (void)
{
  int i;
  gboolean ret;
  GstPlugin *plugin;
  GstRegistry *registry;

  const char *needed[] = {
      "opus", "vpx", "nice", "webrtc", "dtls", "srtp", "rtpmanager", "videotestsrc", "audiotestsrc",
      "webrtc.webrtcbin", "jpeg.jpegdec", "vpx.vp8enc", "vpx.vp8dec", "rtp.rtph264pay", "rtp.rtpvp8pay", "x264.x264enc",
      "videoparsersbad.h264parse", "coreelements.filesrc", "matroska.matroskademux", "isomp4.qtdemux",
      "videorate", "videoscale", "videoconvert", "coreelements.queue", "app.appsrc", "libav.avdec_h264",
      "coreelements.fakesink",
      NULL
  };

  registry = gst_registry_get();
  ret = TRUE;
  for(i=0; needed[i]; i++) {
    char *n = strdup(needed[i]);
    if(!n) return FALSE;
    char *pt = strchr(n, '.');
    char *plugin_name = n;
    char *elem_name = NULL;
    if(pt) {
        *pt = '\0';
        elem_name = pt+1;
    }

    plugin = gst_registry_find_plugin(registry, plugin_name);
    if(!plugin) {
      gst_print("Required gstreamer plugin '%s' not found\n", plugin_name);
      ret = FALSE;
      free(n);
      continue;
    }
    if(elem_name) {
        if(!check_feature(plugin, elem_name)) {
            gst_print("Required gstreamer plugin '%s' does not provide '%s' feature\n", plugin_name, elem_name);
            ret = FALSE;
        }
    }
    gst_object_unref(plugin);
    free(n);
  }
  return ret;
}


int main(int argc, char *argv[])
{
  gst_init (&argc, &argv);
  if(!check_plugins ()) {
    printf("check plugins failed\n");
    return 1;
  }
  printf("check plugins ok\n");
  return 0;
}

