record_ex:
	gcc -ggdb -o alsa-record-example record_ex.c -lasound && ./alsa-record-example plughw:1,0
	sox -t raw -b 16 -e signed-integer -r 44100 test.raw test.wav
