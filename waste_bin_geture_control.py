import cv2
import mediapipe as mp
import serial
import time

# Serial setup
ser = serial.Serial("COM3", 115200, timeout=1)
time.sleep(2)  # allow ESP32 to reset


def send_cmd(cmd):
    ser.write((cmd + "\n").encode())


# MediaPipe hand tracking
mp_hands = mp.solutions.hands
mp_draw = mp.solutions.drawing_utils

hands = mp_hands.Hands(
    max_num_hands=1, min_detection_confidence=0.7, min_tracking_confidence=0.7
)

# Webcam setup
cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)

# Keep track of the last command to avoid spamming serial
last_command = None

while True:
    success, frame = cap.read()
    if not success:
        print("Frame no dey come")
        break

    frame = cv2.flip(frame, 1)  # mirror image
    rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    result = hands.process(rgb)

    command = None  # default no command

    if result.multi_hand_landmarks:
        hand = result.multi_hand_landmarks[0]
        # Get finger tip landmarks (indices in MediaPipe)
        finger_tips = [8, 12, 16, 20]  # Index, Middle, Ring, Pinky tips
        thumb_tip = 4

        # Check if fingers are extended
        extended_fingers = 0

        for tip in finger_tips:
            if hand.landmark[tip].y < hand.landmark[tip - 2].y:
                extended_fingers += 1

        # Thumb check (simplified)
        if hand.landmark[thumb_tip].x < hand.landmark[thumb_tip - 2].x:
            extended_fingers += 1

        # Gesture logic
        if extended_fingers >= 4:
            command = "OPEN"  # Palm open
        else:
            command = "CLOSE"  # Fist

        # Draw landmarks
        mp_draw.draw_landmarks(frame, hand, mp_hands.HAND_CONNECTIONS)

    # Send command only if it changed
    if command and command != last_command:
        send_cmd(command)
        last_command = command
        print(f"Sent command: {command}")

    cv2.imshow("Gesture Control", frame)
    if cv2.waitKey(1) & 0xFF == ord("q"):
        break

# Cleanup
cap.release()
cv2.destroyAllWindows()
ser.close()
