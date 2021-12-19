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
    PROP_LOCATION
};

#define INPUT_AUDIO_CAPS GST_STATIC_CAPS("audio/x-rav, " \
                                         "format = S16LE, " \
                                         "rate = 48000, " \
                                         "channels = 1, " \
                                         "endianness = 1234," \
                                         "depth = 16")

static GstStaticPadTemplate gst_wav_player_src_template = GST_STATIC_PAD_TEMPLATE(
        "src", GST_PAD_SRC, GST_PAD_ALWAYS, INPUT_AUDIO_CAPS);

#define gst_wav_player_parent_class parent_class
G_DEFINE_TYPE (GstWAVPlayer, gst_wav_player, GST_TYPE_PUSH_SRC);

static void gst_wav_player_set_property (GObject* object, guint prop_id, const GValue* value, GParamSpec* pspec);
static void gst_wav_player_get_property (GObject* object, guint prop_id, GValue* value, GParamSpec* pspec);
static void gst_wav_player_finalize(GObject* object);
static gboolean gst_wav_player_negotiate(GstBaseSrc* src);
static gboolean gst_wav_player_start(GstBaseSrc* src);
static gboolean gst_wav_player_stop(GstBaseSrc* src);
static gboolean gst_wav_player_query(GstBaseSrc* src, GstQuery* query);
static GstFlowReturn gst_wav_player_create(GstBaseSrc* src, guint64 offset, guint size, GstBuffer** buf);

static void gst_wav_player_class_init (GstWAVPlayerClass* klass) {
    GObjectClass* gobject_class;
    GstBaseSrcClass* gstbasesrc_class;

    gobject_class = (GObjectClass*) klass;
    gstbasesrc_class = (GstBaseSrcClass*) klass;

    gobject_class->set_property = gst_wav_player_set_property;
    gobject_class->get_property = gst_wav_player_get_property;
    gobject_class->finalize = gst_wav_player_finalize;
    gstbasesrc_class->negotiate = GST_DEBUG_FUNCPTR(gst_wav_player_negotiate);
    gstbasesrc_class->start = GST_DEBUG_FUNCPTR(gst_wav_player_start);
    gstbasesrc_class->stop = GST_DEBUG_FUNCPTR(gst_wav_player_stop);
    gstbasesrc_class->query = GST_DEBUG_FUNCPTR(gst_wav_player_query);
    gstbasesrc_class->create = GST_DEBUG_FUNCPTR(gst_wav_player_create);

    gst_element_class_add_static_pad_template(GST_ELEMENT_CLASS(klass),
                                              &gst_wav_player_src_template);

    g_object_class_install_property(gobject_class, PROP_LOCATION,
                                    g_param_spec_string ("location", "Location", "Input file location",
                                                         "", G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    gst_element_class_set_details_simple (gstbasesrc_class,
                                          "WAVPlayer",
                                          "FIXME:Generic",
                                          "FIXME:Generic Template Element", " <<user@hostname.org>>");
}

static void gst_wav_player_init(GstWAVPlayer* player) {
    player->file_location = NULL;
}

static void gst_wav_player_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* pspec) {
    GstWAVPlayer* wav_player = (GstWAVPlayer*)(object);

    GST_DEBUG_OBJECT(wav_player, "set_property");

    switch (prop_id) {
        case PROP_LOCATION:
            wav_player->file_location = g_strdup(g_value_get_string(value));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }
}

static void gst_wav_player_get_property (GObject* object, guint prop_id, GValue* value, GParamSpec* pspec) {
    GstWAVPlayer* wav_player = (GstWAVPlayer*)(object);

    GST_DEBUG_OBJECT(wav_player, "get_property");

    switch (prop_id) {
        case PROP_LOCATION:
            g_value_set_string(value, wav_player->file_location);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }
}

void gst_wav_player_finalize(GObject* object) {
    GstWAVPlayer* wav_player = (GstWAVPlayer*)(object);

    GST_DEBUG_OBJECT(wav_player, "finalize");

    G_OBJECT_CLASS(gst_wav_player_parent_class)->finalize(object);
}

static gboolean gst_wav_player_negotiate(GstBaseSrc* src) {
    GstWAVPlayer* wav_player = (GstWAVPlayer*)(src);

    GST_DEBUG_OBJECT(wav_player, "negotiate");

    return TRUE;
}

static gboolean gst_wav_player_start(GstBaseSrc* src) {
    GstWAVPlayer* wav_player = (GstWAVPlayer*)(src);

    GST_DEBUG_OBJECT(wav_player, "start");
    GST_DEBUG_OBJECT(wav_player, "location=%s", wav_player->file_location);

    return TRUE;
}

static gboolean gst_wav_player_stop(GstBaseSrc* src) {
    GstWAVPlayer* wav_player = (GstWAVPlayer*)(src);

    GST_DEBUG_OBJECT(wav_player, "stop");

    if (wav_player->file_location != NULL) {
        g_free(wav_player->file_location);
        wav_player->file_location = NULL;
    }

    return TRUE;
}

static gboolean gst_wav_player_query(GstBaseSrc* src, GstQuery* query) {
    GstWAVPlayer* wav_player = (GstWAVPlayer*)(src);

    GST_DEBUG_OBJECT(wav_player, "query %s",
                     gst_query_type_get_name(GST_QUERY_TYPE(query)));

    gboolean ret = GST_BASE_SRC_CLASS(gst_wav_player_parent_class)->query(src, query);

    return ret;
}

static gsize gst_wav_player_read_next_data(gint16** data) {
    *data = g_malloc(1 * sizeof(gint16));
    (*data)[0] = 1;
    return 1;
}

static GstBuffer* gst_wav_player_process(GstWAVPlayer* wav_player, guint64 offset,
                                         guint size) {
    GstBuffer* buffer;
    buffer = gst_buffer_new();
//    GstCaps* caps;
  //  caps = gst_static_pad_template_get_caps(&gst_wav_player_src_template);
    //gst_buffer_set_caps(buffer, caps);

    gint16* data;
    g_assert(data);
    gsize realsize;
    realsize = gst_wav_player_read_next_data(&data);

    GstMemory* memory = gst_memory_new_wrapped(0, data, realsize, 0, realsize, data, g_free);
    g_assert(memory);

    gst_buffer_insert_memory(buffer, -1, memory);
    return buffer;
}

static GstFlowReturn gst_wav_player_create(GstBaseSrc* src, guint64 offset,
                                           guint size, GstBuffer** buf) {
    GstWAVPlayer* wav_player = (GstWAVPlayer*)(src);

    GST_DEBUG_OBJECT(wav_player, "create");

    if (wav_player->file_location == NULL) {
        exit(1);
    }

    *buf = gst_wav_player_process(wav_player, offset, size);
    return GST_FLOW_OK;
}

static gboolean wavplayer_init (GstPlugin* player) {
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
