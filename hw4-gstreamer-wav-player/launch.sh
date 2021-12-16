#!/usr/bin/env sh

plugin_path=./build/wav_player
gst-launch-1.0 -v -m --gst-plugin-path="$plugin_path" gstwavplayer location=123
