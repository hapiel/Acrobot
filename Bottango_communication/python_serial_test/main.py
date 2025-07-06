import argparse
from pythonosc.dispatcher import Dispatcher
from pythonosc import osc_server
import serial
import threading

# Initialize serial communication
def init_serial(port, baudrate):
    try:
        ser = serial.Serial(port, baudrate)
        print(f"Connected to serial port: {port} at {baudrate} baud.")
        return ser
    except Exception as e:
        print(f"Error initializing serial port: {e}")
        return None

# Function to handle incoming OSC messages
def osc_handler(unused_addr, *args):
    message = f"OSC Message: {unused_addr} {args}"
    print(message)
    if ser and ser.is_open:
        try:
            # Convert OSC message to a string and send it over serial
            serial_message = f"{unused_addr} {args}\n"
            ser.write(".\n".encode('utf-8'))
        except Exception as e:
            print(f"Error sending serial message: {e}")

# Thread to keep reading from the serial port and print incoming messages
def read_serial():
    while ser and ser.is_open:
        try:
            if ser.in_waiting > 0:
                incoming_message = ser.readline().decode('utf-8').strip()
                print(f"Incoming Serial Message: {incoming_message}")
        except Exception as e:
            print(f"Error reading serial message: {e}")

if __name__ == "__main__":
    # Parse command-line arguments
    parser = argparse.ArgumentParser()
    parser.add_argument("--osc-ip", type=str, default="127.0.0.1", help="IP address to listen to OSC messages.")
    parser.add_argument("--osc-port", type=int, default=9000, help="Port to listen to OSC messages.")
    parser.add_argument("--serial-port", type=str, default="COM3", help="Serial port to use.")
    parser.add_argument("--baudrate", type=int, default=115200, help="Baudrate for the serial communication.")
    args = parser.parse_args()

    # Initialize the dispatcher and server
    dispatcher = Dispatcher()
    dispatcher.set_default_handler(osc_handler)

    server = osc_server.ThreadingOSCUDPServer((args.osc_ip, args.osc_port), dispatcher)
    print(f"Serving OSC on {args.osc_ip}:{args.osc_port}")

    # Initialize the serial port
    ser = init_serial(args.serial_port, args.baudrate)

    # Start the serial reading thread
    if ser and ser.is_open:
        serial_thread = threading.Thread(target=read_serial, daemon=True)
        serial_thread.start()

    # Start the OSC server
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nShutting down OSC server...")
    finally:
        if ser and ser.is_open:
            ser.close()
            print("Serial port closed.")
