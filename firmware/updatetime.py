# Imports
import pylink
import time

# Setup J-Link connection
jlink = pylink.JLink()
jlink.open(serial_no=440308528)  # Replace with your J-Link serial number
jlink.set_tif(interface=pylink.JLinkInterfaces.SWD)
jlink.connect(chip_name="EFR32MG24AxxxF1536", speed="auto", verbose=True)

# Start RTT
jlink.rtt_start()
time.sleep(1)

# Send your timestamp
timeStamp = int(time.time())
jlink.rtt_write(0, (str(timeStamp) + '\n').encode())

# Success Message
print("Value uploaded to RTT successfully.")