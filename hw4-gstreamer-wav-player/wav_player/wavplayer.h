//
// Created by Danya Smelskiy on 17.12.2021.
//

#ifndef OTUSHW_WAV_PLAYER_H
#define OTUSHW_WAV_PLAYER_H

#include <gst/gst.h>

G_BEGIN_DECLS

#define GST_TYPE_WAVPLAYER (gst_wav_player_get_type())
G_DECLARE_FINAL_TYPE (GstWAVPlayer, gst_wav_player, GST, WAVPLAYER, GstElement)

struct _GstWAVPlayer {
    GstElement element;

    GstPad *sinkpad, *srcpad;

    gboolean silent;
};

G_END_DECLS

#endif //OTUSHW_WAV_PLAYER_H
