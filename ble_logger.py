import asyncio
import sys
from bleak import BleakScanner, BleakClient
from bleak.backends.characteristic import BleakGATTCharacteristic

# ==========================================
# CONFIGURATION
# ==========================================
# Match this to your device's advertised name
TARGET_DEVICE_NAME = "OTA_potentiostat" 

# Replace this with your exact Debug Log Characteristic UUID from gatt_db.h / btconf
DEBUG_CHAR_UUID = "4b872075-b5ae-4af5-a7a1-6cefcae1cb5f" 
# ==========================================

def notification_handler(characteristic: BleakGATTCharacteristic, data: bytearray):
    """Callback function that prints incoming BLE notifications."""
    try:
        # Decode ASCII/UTF-8 data coming from the MCU's ble_printf
        log_text = data.decode("utf-8")
        # Print without adding extra newlines (since ble_printf usually includes \n)
        print(log_text, end="", flush=True)
    except UnicodeDecodeError:
        # Fallback to hex printing if we catch non-string binary data
        hex_data = " ".join(f"{b:02X}" for b in data)
        print(f"\n[RAW HEX]: {hex_data}")

async def main():
    print(f"[STATUS] Scanning for a BLE device named '{TARGET_DEVICE_NAME}'...")
    
    # 1. Scan for the target device
    device = await BleakScanner.find_device_by_filter(
        lambda d, ad: d.name and TARGET_DEVICE_NAME.lower() in d.name.lower(),
        timeout=10.0
    )
    
    if not device:
        print(f"[ERROR] Could not find any device named '{TARGET_DEVICE_NAME}' advertising nearby.")
        sys.exit(1)
        
    print(f"[SUCCESS] Found device: {device.name} ({device.address})")
    print(f"[STATUS] Connecting to {device.address}...")
    
    # 2. Connect to the device
    async with BleakClient(device) as client:
        if client.is_connected:
            print("[SUCCESS] Connected successfully!")
        else:
            print("[ERROR] Failed to connect.")
            sys.exit(1)

        # Helper: list available characteristics if the user didn't change the placeholder UUID
        if DEBUG_CHAR_UUID == "00000000-0000-0000-0000-000000000000":
            print("\n[INFO] Showing discovered characteristics to help you find your UUID:")
            for service in client.services:
                for char in service.characteristics:
                    if "notify" in char.properties:
                        print(f"  * Service: {service.uuid} | Char: {char.uuid} (Supports Notifications)")
            print("\n[WARNING] Please stop the script, paste your target UUID into 'DEBUG_CHAR_UUID', and run again.\n")
            return

        # 3. Start receiving notifications from the debug characteristic
        print(f"[STATUS] Subscribing to debug log notifications (UUID: {DEBUG_CHAR_UUID})...")
        try:
            await client.start_notify(DEBUG_CHAR_UUID, notification_handler)
            print("--- Active Log Stream Started (Press Ctrl+C to stop) ---")
            
            # Keep the async loop running indefinitely while we receive notifications
            while True:
                await asyncio.sleep(1.0)
                
        except asyncio.CancelledError:
            print("\n[STATUS] Shutting down logger...")
        except Exception as e:
            print(f"\n[ERROR] Error during notification stream: {e}")
        finally:
            print("[STATUS] Stopping notifications and disconnecting...")
            await client.stop_notify(DEBUG_CHAR_UUID)

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\n[INFO] Client stopped by user.")