#!/usr/bin/env sh

plugin_path=./build/wav_player
gst-launch-1.0 -v -m --gst-plugin-path="$plugin_path" wavplayer location=123 ! autoaudiosink
