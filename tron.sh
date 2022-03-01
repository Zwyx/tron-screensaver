#!/bin/bash

TRON_CLOCK="$(dirname "$0")/tron-clock"

while [[ $(pgrep -c "$(basename "$0")") -gt 1 ]]; do
	pkill --oldest "$(basename "$0")"
done

while true; do
	while [[ $(xprintidle) -lt 300000 ]]; do
		sleep 30
	done

	# Hide the mouse pointer
	unclutter --timeout 0 &

	# Pause notifications
	xfconf-query -c xfce4-notifyd -p /do-not-disturb -s true

	# `tac`, opposite of `cat`, is to get the primary monitor at the end, to get the xfce panel hidden
	monitors=$(xrandr | grep " connected" | tac | sed "s/^.* [0-9]\+x[0-9]\++\([0-9]\++[0-9]\+\).*$/10x10+\1/")

	if [ -f "$TRON_CLOCK" ]; then
		for monitor in $monitors; do
			xfce4-terminal --geometry "$monitor" --title tron-clock --fullscreen --hide-menubar --hide-borders --hide-toolbar --hide-scrollbar --zoom 3 --color-bg=black -x "$TRON_CLOCK" $RANDOM &
		done
	else
		echo "Executable '$TRON_CLOCK' not found!"
	fi

	while [[ $(xprintidle) -gt 2000 ]]; do
		sleep 1
	done

	killall unclutter tron-clock
	xfconf-query -c xfce4-notifyd -p /do-not-disturb -s false
done
