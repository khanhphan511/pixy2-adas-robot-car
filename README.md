# Active ADAS Features on a Scaled Robotic Car (Pixy2 + Arduino)

This repository contains a scaled robotic car that implements several **Advanced Driver Assistance System (ADAS)** features using a **Pixy2 vision sensor**.

---

## Features: 

### Object Following with Safety Distance
- Teach Pixy2 a colored **ball** as a tracked object (signature).
- Car drives forward to follow the ball.
- Uses Pixy2 x-position and object size to:
  - Center the car on the ball.
  - Keep at least **6 inches** separation; car slows and stops if it gets too close.

### Traffic Light LED Control + Color Learning
- Drive a **3-LED traffic light** (red, yellow, green) from Arduino:
  - Red ON for 20 s, Yellow+Green OFF.
  - Green ON for 20 s, Red+Yellow OFF.
  - Yellow ON for 16 s, Red+Green OFF.
- Teach Pixy2 three color signatures:
  - `red`, `green`, and `yellow`.
- Adjust camera and LED brightness with PixyMon + potentiometer.

### Time-to-Pass Decision (Yellow Light Logic & Traffic Light Response)
- Use Pixy2 as a ranging sensor to measure distance to the traffic light.
- Use a tachometer / encoder to measure car speed and compute linear velocity.
- Compute the required travel time to reach the light:

  `t_required = distance / speed`

- Traffic light behavior:
  1. **Red ON**  
     - The car slows down smoothly and comes to a complete stop before the traffic light.  
     - The final stop position is between **2" and 6"**.
  2. **Green ON**  
     - The car maintains its current speed and continues cruising.
  3. **Yellow light “time-to-pass” logic**
  - Using the measured distance and speed, the car decides whether it can safely pass in 8 seconds:
    - **Scenario 1 – Eight seconds are NOT sufficient**  
      - Configure parameters so `t_required > 8 s`.  
      - The car should stop before the traffic light, simulating a red light.
    - **Scenario 2 – Eight seconds are sufficient**  
      - Configure parameters so `t_required ≤ 8 s`.  
      - The car keeps cruising at the same speed, simulating a green light.

### Lane Keeping
- Use Pixy2 to detect **two lane lines** on the floor.
- Control steering so the car:
  - Stays centered between left and right lines.
  - Does **not** drive directly over either line.

---

## Code structure

- `firmware/ball_following/` – Ball tracking and 6" safety distance.
- `firmware/traffic_light_control/` – 20s/16s traffic light patterns and color signatures.
- `firmware/time_to_pass/` – Distance, speed, time-to-pass logic and serial output.
- `firmware/lane_keeping/` – Lane detection and centering.

Each sketch is written in Arduino C/C++ and uses the Pixy2 Arduino library.

---

## How to run (example)

1. Install the **Pixy2 Arduino library** and any encoder/tachometer libraries.
2. Open the desired `.ino` file in the `firmware/...` folder in the Arduino IDE.
3. Select the correct board and COM port, then upload.
4. For Pixy2:
   - Use PixyMon to teach the required signatures (ball, red, yellow4, green, lanes).
5. Open the Serial Monitor to view distance, speed, and timing outputs (IPR3).
