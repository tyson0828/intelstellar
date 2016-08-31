# Intelstellar

This project demonstrates Arduino101 + BLE watching object approaching and notifying users through beef and connected cell phones if the object is within the distance set.

The arduino implementation scans objects 360 degree like a radar. If it detects any within the distance set, it beefs and sends the notification with the distance measured to the connected cell phones.

The app installed in cell phone looks for Arduino101 running the radar service and automatically connects. Once connected, the app subscribes for alarm notifications. If the app is in the background, the user is notified via local notifications.

## Installing

### iOS

    $ git clone git@github.com:tyson0828/intelstellar.git
    $ cd intelstellar
    $ cordova platform add ios
    $ cordova run ios --device

### Android <Currently not available> ###

    $ git clone git@github.com:tyson0828/intelstellar.git
    $ cd intelstellar
    $ cordova platform add android
    $ cordova run android --device

### Arduino101

The application needs Arduino101 running the radar service. The radar service monitors any object and sends a notification to connected cell phones.
