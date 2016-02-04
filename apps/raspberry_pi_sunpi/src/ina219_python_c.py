
#!/usr/bin/python
from __future__ import division
from Subfact_ina219 import INA219

# Initialize INA219 breakout board at 0x40 
ina = INA219(0x40)

# Getting battery voltage (12 - shunt since running on mains)
bat_vol = (12 - (ina.getShuntVoltage_mV()/1000))

# Getting battery power (Watt)
bat_pow = -bat_vol*ina.getCurrent_mA()/1000

print bat_vol
print bat_pow
