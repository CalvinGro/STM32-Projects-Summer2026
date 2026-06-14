import serial
import sys

PORT = "COM9"
BAUD = 115200
OUTPUT_FILE = "imu_log.csv"

def main():
    saving = False

    with serial.Serial(PORT, BAUD, timeout=1) as ser, open(OUTPUT_FILE, "w", newline="") as file:
        print (f"Listening on port: {PORT} at baud: {BAUD}.")
        print (f"Hold B1 to dump flash from STM32.")

        while True:
            raw = ser.readline()

            if not raw:
                continue

            line = raw.decode("ascii", errors="replace").rstrip("\r\n")

            if line == "STARTING FLASH DUMP":
                saving = True
            elif line == "FINISHED FLASH DUMP":
                break
            elif saving:
                file.write(line + "\n")
            print(line)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n\nStopped.")
        sys.exit(0)