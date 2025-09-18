- no wifi

  1. wake up from button press (no display update)
  2. update display once per hour (if there is a button press in between, there would be a longer interval)
  3. turn on AP mode on long button press

- wifi ok, no mqtt

  1. wake up from button press (no display update)
  2. update display once per hour (connect WIFI and update NTP time when device wakes up, either at button press or at timer wakeup)
  3. STA mode, not AP mode

- wifi ok, mqtt ok

  1. wake up from button press (no display update)
  2. update display once per hour (connect WIFI and update NTP time when device wakes up, either at button press or at timer wakeup)
  3. STA mode, not AP mode

? introduce icons for WIFI and MQTT to indicate what's working
! introduce n minute wifi life after LED on, with 5 minute timeout after every http or mqtt access
! introduce the possibility to get the current json cofiguration through a new operation
! do not try to start mqtt without wifi being in station mode
! do not try to start http without wifi in station or ap mode
! do not try to start wifi station mode without valid credentials (maybe after a number of unsuccessful connection attempts)
! start in AP mode on long button press
! adapt wakeup and display redraw cycle when wifi connection attempts have been exceeded
! consider new configuration (do a restart)
! introduce the general possibility of storing a config in eeprom (seems it must fit 512 bytes)
! introduce the possibility to upload a json config through a new operation (use code from moth), then continue with that configuration
