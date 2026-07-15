# EFR32BG24 Potentiostat Firmware & Debugger

This repository contains the C++ firmware for the custom potentiostat platform (featuring an LMP91000 analog front end, EFR32BG24 MCU, and an LSM6DSV IMU) alongside a Python-based real-time wireless BLE debugging client.

---

## 1. Prerequisites and Requirements

### Hardware

* Custom Potentiostat PCB (EFR32BG24, LMP91000, LSM6DSV)
* Segger J-Link or Simplicity Link Debugger
* A stable external power source (e.g., 3.3V bench power or a well-charged battery with adequate bulk capacitance)

### Software

* Simplicity Commander (added to your system PATH)
* Python 3.8 or newer
* Bleak Python library (for BLE communications)

---

## 2. Initial Setup (Wired Flashing)

Before the board can accept wireless updates, you must flash the bootloader and the initial application image via a physical JTAG/SWD debug connection.

### Step 1: Flash the Gecko Bootloader

The Gecko Bootloader is responsible for managing internal storage and installing GBL files. You must flash the combined bootloader image containing both the first-stage and main bootloader configurations.

```bash
commander flash bootloader-storage-internal-single-combined.s37 --device EFR32BG24

```

### Step 2: Flash the Application Firmware

Flash the initial C++ application binary:

```bash
commander flash potentiostat_app.s37 --device EFR32BG24

```

---

## 3. Over-The-Air (OTA) Firmware Updates

The custom board supports wireless updates using the Silicon Labs Apploader. When the board reboots into OTA mode, it will advertise as `OTA_potentiostat`.

### OTA Update Steps
#### *Make sure to hold down the power button during the entire OTA upload process, and for about 15-20 seconds after it finishes*

1. Build the firmware project to generate your target `.gbl` file.
2. Put the device into OTA mode. The device will stop regular advertising and begin advertising as `OTA_potentiostat`.
3. Open the **EFR Connect** app on your smartphone or pc.
4. Scan and connect to **OTA_potentiostat**.
5. Select **OTA DFU**, choose **Partial OTA** (or select your `.gbl` file), and initiate the transfer.
6. Upon reaching 100%, the chip will reboot, verify the image, and boot into your new application.

---

## 4. Real-Time Wireless Log Stream

The firmware includes a high-speed wireless logger that routes standard debug prints over a custom BLE characteristic.

### Installation

Install the required asynchronous BLE library:

```bash
pip install bleak

```

### Python Logger Client: `ble_logger.py`

### Usage

1. Power on your potentiostat board.
2. Run the client script:
```bash
python ble_logger.py

```


3. The script will automatically scan, establish a secure BLE connection to your EFR32, subscribe to the notifications, and print custom `ble_printf` debug logs directly to your console.
