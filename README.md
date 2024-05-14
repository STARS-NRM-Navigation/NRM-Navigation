# NRM-Navigation
## FW v0.0.1 Features
### TOF
- Enables one sensor connected to i2c
- Uses an aggregate function which takes the average of each column in
the 8x8 grid, then takes the average of each two adjacent columns. This
outputs a 1x4 array of distance values in mm.

### LED
- Enables 9 LEDs
- Receives TOF distance values in 1x4 array and outputs to the first 4
LEDs with the following thresholds:
    - $<1500$mm : RED (danger)
    - $<3000$mm : ORANGE (warning)
    - $3000\le$mm : BLACK (OK)

