This is the code for the packs involved with SyncSymphony. 
If you want to see the code for the hub, view the repository SyncSymphony.
This requires you to have the esp-idf toolchain installed.

Code is found in the main directory. Main.c is the code that executes first. 
npt.c updates the clock of the pack. websocket.c controls what happens when
data is received. metronome.c actually controls the motors and makes the
metronome beat at the correct interval.

Build with
idf.py build 
in the root directory

Send to the board with
idf.py flash

View debug information with
idf.py monitor

The pack will auto connect to the hub's WiFi with
SSID: SyncSymphonyHub
password: syncingandswimming

NOTE the following files in esp-idf need to be modified: 
sntp.h and add void sntp_request(void *) 
sntp.c and change static void sntp_request(void *arg) to void sntp_request(void *arg)  
sntp.c has static void sntp_request(void *arg) in there twice 
follow as per https://stackoverflow.com/questions/56025619/how-to-resync-time-from-ntp-server-in-esp-idf

