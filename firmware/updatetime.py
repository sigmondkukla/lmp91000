import pylink
import time

jlink = pylink.JLink()
jlink.open(serial_no=440308528)  # Replace with your J-Link serial number
jlink.set_tif(interface=pylink.JLinkInterfaces.SWD)
jlink.connect(chip_name="EFR32MG24AxxxF1536", speed="auto", verbose=True)  # e.g. EFR32MG24AxxxF1536

# Start RTT
jlink.rtt_start()
time.sleep(1)  # Wait for RTT to initialize

# Send your timestamp
#jlink.rtt_write(0, b"1720224000")
#timeStamp = 1720224000
timeStamp = int(time.time())
jlink.rtt_write(0, (str(timeStamp) + '\n').encode())

jlink.rtt_stop()
jlink.close()

print("Value uploaded to RTT successfully.")