# Managing the temperature of a Gaerbox for sour dough

## configure wifi

- `mos wifi WIFISSID WIFIPASSWORD`
- this will need to be done again after flashing, but not after OTA

## Build and update the firmware

- `mos build --local --platform esp8266`
- `curl -v -F file=@build/fw.zip http://192.168.178.38/update`

## Implementation details

-
