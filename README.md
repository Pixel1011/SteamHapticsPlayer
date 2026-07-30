# Steam Haptics Player

This project allows stereo audio to be streamed from a file to the haptics of a Steam controller (2026), practically allowing anything to be played on it's haptic motors.

[![Watch the video here!](https://img.youtube.com/vi/iLLmQzjEhYs/0.jpg)](http://www.youtube.com/watch?v=iLLmQzjEhYs "Still Alive on a Steam Controller")

## How To


**Supported device:** Steam Controller (2026). Other controllers are unsupported.

**Requirements:** FFmpeg on PATH

#### **Note For puck audio:**
- When streaming audio via the Puck, you may encounter popping noises or other audio quality issues due to packet loss. Wired should normally be much cleaner.

#### On Windows
1. Open Powershell in the file directory where you have extracted the executable
2. (skip if already installed) Install FFmpeg with `winget install ffmpeg`
3. Run the program with `.\steam-haptics-player [AUDIO_FILE]`
4. Profit!

OR
1. (skip if already installed) Install FFmpeg with `winget install ffmpeg` in powershell
2. Drag an audio file onto the executable.
    - *This will not let you skip the setup portion of the program if it has already been run once, however it is the easier option*


#### On Linux
1. Right click inside the folder
2. Click "Open in Terminal"
3. (skip if already installed) Install FFmpeg with a package manager of your choice
4. Install libusb (with i believe either, `sudo apt install libhidapi-dev` or `sudo pacman -S hidapi`)
5. Type `chmod +x steam-haptics-singer` to make the program executable
6. Run the program with `.\steam-haptics-player [AUDIO_FILE]`
7. Profit!


### Usage from command prompt:
	Usage: play-pcm.exe <file path>

### Tips
  - Try pressing the controller down into a solid object (although not too hard), it will make the sound **much** louder

### Troubleshooting
- **No audio:**  
  - Ensure you ran the setup phase (do not use `-s`) at least once since the controller was last started.
- **Garbled audio:**  
  - Re-run without `-s`. If issues persist, restart the controller and run setup again.
- **Loud static immediately on launch:**  
  - Restart the controller and try again.
- **ffmpeg not found**  
  - Install `ffmpeg` and ensure it is on your `PATH` (`winget install ffmpeg` on Windows).
- **if all else fails:**
  - Try connect the steam controller directly via USB and then try again.


## Compiling
**Build (Linux / WSL)**

```bash
sudo apt update
sudo apt install build-essential pkg-config libhidapi-dev ffmpeg
make
```

**Build (Windows, MSYS2 UCRT64)**
```bash
pacman -Syu

pacman -S mingw-w64-ucrt-x86_64-gcc \
          mingw-w64-ucrt-x86_64-hidapi \
          mingw-w64-ucrt-x86_64-ffmpeg \
          mingw-w64-ucrt-x86_64-make \
          mingw-w64-ucrt-x86_64-pkgconf

make
```
## Changelog
[v1.3.0]
Added progress bar, reworked how files are loaded

[v1.2.0]
Fixed controller detection with puck (Thanks @CrazyCritic89)

[v1.1.0]
Linux support
Checks for ffmpeg

[v1.0.0]
Initial release!


I spent alot of time and effort reverse engineering the Steam controller's firmware to find the HID commands needed to do this so i would appreciate the stars very much!

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/N4N6145I0V)

I was also heavily inspired to do this by CrazyCritic89's [SteamHapticsSinger](https://github.com/CrazyCritic89/SteamHapticsSinger). I suggest checking it out if you wish to play MIDI files on the steam controller!
(they honestly can sound better than this at times)

If you do happen to make a video with this, please do link back to this github repo so others can try too :steamhappy: