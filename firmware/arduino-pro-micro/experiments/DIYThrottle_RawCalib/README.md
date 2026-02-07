# DIYThrottle_RawCalib — Step 4 + HID (Variant B: 0..1023 input to Rz)

**Path:** `firmware/arduino-pro-micro/experiments/DIYThrottle_RawCalib/DIYThrottle_RawCalib.ino`

This variant **feeds 0..1023** to `Joystick.setRzAxis()` **and** calls `Joystick.setRzAxisRange(0,1023)`.
The MHeironimus HID library scales to the USB HID report internally (v2.1.x sends 0..65535 to host).

## Requirements
- Library: **ArduinoJoystickLibrary** by Matthew Heironimus (install via Add .ZIP Library).  
- Board: **Arduino Leonardo** (Pro Micro compatible).  
- Baud: **115200**.

## Test in Windows
- `Win+R → joy.cpl` → select the device → **Test** tab → **Z Rotation** should move smoothly.

## CSV header (Serial Plotter)
`S1,S2,V1,V2,F1,F2,N1,N2,D1,D2,Y1,Y2,DELTA,FAULT,OUT,RZ`

## Git (branch → commit → PR)
```powershell
cd "<repo-root>"
git switch -c feature/step4-hid-variantB

git add firmware/arduino-pro-micro/experiments/DIYThrottle_RawCalib/DIYThrottle_RawCalib.ino         firmware/arduino-pro-micro/experiments/DIYThrottle_RawCalib/README.md

git commit -m "Step 4 + HID (Variant B): setRzAxisRange(0,1023) and feed 0..1023"

git push -u origin feature/step4-hid-variantB
```
Then **Compare & pull request** → verify → **Merge** → **Delete branch**.
