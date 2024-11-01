from bluepy.btle import Peripheral, UUID, BTLEException
import RPi.GPIO as GPIO
import time

# MAC address of the Arduino Nano 33 IoT to connect to
nano_mac_address = "94:b5:55:c0:6c:6e"
BUZZER_PIN = 23 # GPIO pin for the buzzer
GPIO.setmode(GPIO.BCM)  # Set GPIO mode to BCM
GPIO.setup(BUZZER_PIN, GPIO.OUT)  # Set buzzer pin as an output

try:
    # Attempt to connect to the Arduino Nano 33 IoT
    print("Connecting to Arduino Nano 33 IoT...")
    nano = Peripheral(nano_mac_address, addrType="public")  # Connect with public address type

    # Define UUIDs for the custom BLE service and characteristic
    service_uuid = UUID("f47ac10b-58cc-4372-a567-0e02b2c3d479")  # Custom service UUID
    characteristic_uuid = UUID("f47ac10b-58cc-4372-a567-0e02b2c3d480")  # Custom characteristic UUID

    # Retrieve the custom service and characteristic from the device
    service = nano.getServiceByUUID(service_uuid)
    characteristic = service.getCharacteristics(characteristic_uuid)[0]

    while True:
        if characteristic.supportsRead():  # Check if the characteristic supports reading
            try:
                # Read and decode distance data from the characteristic
                data = characteristic.read().decode('utf-8')
                distance = float(data)  # Convert the received data to a float
                print(f"Distance from wall: {distance} cm")
            except ValueError:
                print("Error decoding or converting the distance data.")
                continue  # Skip to the next loop iteration on error

            # Trigger actions based on distance
            if distance < 60:
                GPIO.output(BUZZER_PIN, GPIO.HIGH)  # Activate the buzzer
            else:
                GPIO.output(BUZZER_PIN, GPIO.LOW)  # Deactivate the buzzer

            time.sleep(1)  # Wait for 1 second before the next measurement

except BTLEException as e:
    print(f"Failed to connect to the device: {e}")  # Print connection error
finally:
    # Clean up resources and reset GPIO settings
    GPIO.cleanup()  # Clean up GPIO settings
    print("GPIO cleanup completed.")  # Indicate completion of cleanup
