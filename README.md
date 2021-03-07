# Managing the temperature of a Gaerbox for sour dough

- wio link ESP8266 board
- SHT31 (SHT30) temperature chip connect to PIN12/14 for I2C
- Relay connected to PIN 13 (should be able to handle 12V DC with min. 1A)
- I used a heating pad for cats/dogs with 12W power
- One insulating box (like for pizza delivery or smaller)

You need to cut the cord from 12V power supply to the heating mat and put 12V through the relay.
Setting target temperature is done through `mos config-set` or through mdash.net (need to connect your device to mdash then)
Setting the maximum temperature delta is also configurable through mos config or mos.yaml or mdash.

## Build and update the firmware

- `mos build --local --platform esp8266`
- `curl -v -F file=@build/fw.zip http://xx.xx.xx.xx/update` (look at mos for assigned address)
- or initially: `mos flash` and then configure wifi (or also mdash)

## configure wifi

- `mos wifi WIFISSID WIFIPASSWORD`
- this will need to be done again after flashing, but not after OTA

## Implementation details

- Wio link has two I2C interfaces, the one labeled on the board (PIN 4/5) is i2c1, the 12/14 PIN is i2c (bus 0), the i2c1.enable is not well documented (have to look in the mos.yml of the i2c library of your platform). I used the I2C bus 0 connection
- SHT31 driver will try to set/unset heating. If your chip doesn't support heating, use the SHT30 library (as I needed to)
- use mos config-get/mos config-set for updating vars or add it to mdash.net
- link will report current temperature, humdity, target temp and heating status
- see mos.yaml for more configuration options
- The grove ports need their power turned on, that is why you need to set the power GPIO pin of the board to 1 (true).
