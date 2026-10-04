
import csv
import time
from datetime import datetime

import serial
from serial import SerialException

# Change COM3 to the port shown in Arduino IDE.
SERIAL_PORT = "COM3"
BAUD_RATE = 9600
CSV_FILE = "obstacle_sensor_log.csv"


def main():
    try:
        with serial.Serial(
            SERIAL_PORT, BAUD_RATE, timeout=2
        ) as arduino:
            # Allow the Arduino to restart after serial connection.
            time.sleep(2)

            with open(
                CSV_FILE, "a", newline="", encoding="utf-8"
            ) as file:
                writer = csv.writer(file)

                if file.tell() == 0:
                    writer.writerow([
                        "timestamp",
                        "time_ms",
                        "distance_cm",
                        "motion",
                        "status",
                    ])

                print("Logging sensor data. Press Ctrl+C to stop.")

                while True:
                    raw_line = arduino.readline().decode(
                        "utf-8", errors="replace"
                    ).strip()

                    if not raw_line:
                        continue

                    # Skip the CSV header sent by the Arduino.
                    if raw_line.startswith("time_ms,"):
                        continue

                    parts = raw_line.split(",")

                    if len(parts) != 4:
                        print("Skipped invalid line:", raw_line)
                        continue

                    try:
                        time_ms = int(parts[0])
                        distance_cm = float(parts[1])
                        motion = int(parts[2])
                        status = parts[3]
                    except ValueError:
                        print("Skipped invalid values:", raw_line)
                        continue

                    timestamp = datetime.now().isoformat(
                        timespec="seconds"
                    )

                    writer.writerow([
                        timestamp,
                        time_ms,
                        distance_cm,
                        motion,
                        status,
                    ])
                    file.flush()

                    print(
                        f"{timestamp} | "
                        f"Distance: {distance_cm:.2f} cm | "
                        f"Motion: {motion} | Status: {status}"
                    )

    except SerialException as error:
        print("Serial connection error:", error)
        print("Check the COM port, USB cable, and baud rate.")
    except KeyboardInterrupt:
        print("\nLogging stopped.")


if __name__ == "__main__":
    main()
