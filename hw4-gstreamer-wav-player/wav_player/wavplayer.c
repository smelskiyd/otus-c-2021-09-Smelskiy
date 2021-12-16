//
// Created by Danya Smelskiy on 17.12.2021.
//

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>

#include "wavplayer.h"

GST_DEBUG_CATEGORY_STATIC (gst_wav_player_debug);
#define GST_CAT_DEFAULT gst_wav_player_debug

enum {
    PROP_0,
    PROP_SILENT
};

static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
                                                                    GST_PAD_SINK,
                                                                    GST_PAD_ALWAYS,
                                                                    GST_STATIC_CAPS ("ANY")
                                                                    );

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
                                                                   GST_PAD_SRC,
                                                                   GST_PAD_ALWAYS,
                                                                   GST_STATIC_CAPS ("ANY")
                                                                   );

#define gst_wav_player_parent_class parent_class
G_DEFINE_TYPE (GstWAVPlayer, gst_wav_player, GST_TYPE_ELEMENT);


static void gst_wav_player_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_wav_player_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);

static gboolean gst_wav_player_sink_event (GstPad * pad, GstObject * parent, GstEvent * event);
static GstFlowReturn gst_wav_player_chain (GstPad * pad, GstObject * parent, GstBuffer * buf);


static void gst_wav_player_class_init (GstWAVPlayerClass * klass) {
    GObjectClass *gobject_class;
    GstElementClass *gstelement_class;

    gobject_class = (GObjectClass *) klass;
    gstelement_class = (GstElementClass *) klass;

    gobject_class->set_property = gst_wav_player_set_property;
    gobject_class->get_property = gst_wav_player_get_property;

    g_object_class_install_property (gobject_class, PROP_SILENT,
                                     g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
                                                           FALSE, G_PARAM_READWRITE));

    gst_element_class_set_details_simple (gstelement_class,
                                          "WAVPlayer",
                                          "FIXME:Generic",
                                          "FIXME:Generic Template Element", " <<user@hostname.org>>");

    gst_element_class_add_pad_template (gstelement_class,
                                        gst_static_pad_template_get (&src_factory));
    gst_element_class_add_pad_template (gstelement_class,
                                        gst_static_pad_template_get (&sink_factory));
}

static void gst_wav_player_init (GstWAVPlayer * player) {
    player->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
    gst_pad_set_event_function (player->sinkpad,
                                GST_DEBUG_FUNCPTR (gst_wav_player_sink_event));
    gst_pad_set_chain_function (player->sinkpad,
                                GST_DEBUG_FUNCPTR (gst_wav_player_chain));
    GST_PAD_SET_PROXY_CAPS (player->sinkpad);
    gst_element_add_pad (GST_ELEMENT (player), player->sinkpad);

    player->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
    GST_PAD_SET_PROXY_CAPS (player->srcpad);
    gst_element_add_pad (GST_ELEMENT (player), player->srcpad);

    player->silent = FALSE;
}

static void gst_wav_player_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec) {
}

static void gst_wav_player_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec) {
}

static gboolean gst_wav_player_sink_event(GstPad * pad, GstObject * parent, GstEvent * event) {
    GstWAVPlayer *player;
    gboolean ret;

    player = GST_WAVPLAYER (parent);

    GST_LOG_OBJECT (player, "Received %s event: %" GST_PTR_FORMAT,
                    GST_EVENT_TYPE_NAME (event), event);

    switch (GST_EVENT_TYPE (event)) {
        case GST_EVENT_CAPS:
        {
            GstCaps *caps;
            gst_event_parse_caps (event, &caps);
            /* do something with the caps */
            /* and forward */
            ret = gst_pad_event_default (pad, parent, event);
            break;
        }
        default:
            ret = gst_pad_event_default (pad, parent, event);
            break;
    }
    return ret;
}

static GstFlowReturn gst_wav_player_chain (GstPad * pad, GstObject * parent, GstBuffer * buf) {
    GstWAVPlayer *player;

    player = GST_WAVPLAYER(parent);

    if (player->silent == FALSE)
        g_print ("I'm plugged, therefore I'm in.\n");

    return gst_pad_push (player->srcpad, buf);
}

static gboolean wavplayer_init (GstPlugin * player) {
    GST_DEBUG_CATEGORY_INIT (gst_wav_player_debug, "wavplayer",
                             0, "Template wavplayer");
    return gst_element_register (player, "wavplayer", GST_RANK_NONE,
                                 GST_TYPE_WAVPLAYER);
}

#ifndef PACKAGE
#define PACKAGE "wavplayer"
#endif

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
                  GST_VERSION_MINOR,
                  wavplayer,
                  "Template wavplayer",
                  wavplayer_init,
                  PACKAGE_VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
