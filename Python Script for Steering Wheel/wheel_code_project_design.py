import pygame
import serial
import time

def main():
    pygame.init()
    pygame.joystick.init()
    
    if pygame.joystick.get_count() == 0:
        print("No joystick detected!")
        return

    joystick = pygame.joystick.Joystick(0)
    joystick.init()
    print("Joystick initialized:", joystick.get_name())
    
    # Set up serial communication with Arduino (adjust COM port as needed)
    ser = serial.Serial('COM5', 9600, timeout=1)
    time.sleep(2)  # Allow Arduino to reset

    last_command = None
    try:
        while True:
            for event in pygame.event.get():
                pass  # Process events to update joystick state

            # Read steering axis (axis 0, normalized [-1.0, 1.0])
            steering_value = joystick.get_axis(0)
            # Map to -450° to +450°     
            steering_angle = steering_value * 450
            print(f"Steering Value: {steering_value:.3f} -> Steering Angle: {steering_angle:.1f}°")
            
            if steering_angle <= -60:
                command = "LEFT"
            elif steering_angle >= 60:
                command = "RIGHT"
            else:
                command = "NEUTRAL"
            
            if command != last_command:
                ser.write((command + "\n").encode())
                print("Sent command:", command)
                last_command = command
            
            time.sleep(0.1)
    except KeyboardInterrupt:
        print("Exiting...")
    finally:
        ser.close()
        pygame.quit()

if __name__ == "__main__":
    main()
