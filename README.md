# Steam Haptics Player

This project allows stereo audio to be streamed from a file to the haptics of a Steam controller (2026), practically allowing anything to be played on it's haptic motors.

[<img src="https://img.youtube.com/vi/xfg437QMPSw/maxresdefault.jpg" alt="Want you gone on a Steam Controller" width="500"/>](https://www.youtube.com/watch?v=xfg437QMPSw)

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/N4N6145I0V)
No need to donate, though is greatly appreciated!

I also made [Steam Controller Battery Monitor](https://github.com/Pixel1011/Steam-Controller-Battery-Monitor), since there currently isn't a convenient way to view the controller's exact battery percentage.

## Download
[**Download the latest build here!**](https://github.com/Pixel1011/SteamHapticsPlayer/releases/latest)

## Usage


**Supported device:** Steam Controller (2026). Other controllers are unsupported.

**Requirements:** FFmpeg on PATH

#### **Note For puck audio:**
- When streaming audio via the Puck, you may encounter popping noises or other audio quality issues due to packet loss. Wired should normally be much cleaner.

---

#### On Windows
1. Open Powershell in the file directory where you have extracted the executable
2. (skip if already installed) Install FFmpeg with `winget install ffmpeg`
3. Run the program with `.\steam-haptics-player.exe [AUDIO_FILE]`
4. Profit!

OR

1. (skip if already installed) Install FFmpeg with `winget install ffmpeg` in powershell
2. Drag an audio file onto the executable.

#### On Linux
1. Right click inside the folder
2. Click "Open in Terminal"
3. (skip if already installed) Install FFmpeg with a package manager of your choice
4. Install libhid 
   - Debian/Ubuntu: `sudo apt install libhidapi-dev`
   - Arch: `sudo pacman -S hidapi`
5. Type `chmod +x steam-haptics-player` to make the program executable
6. Run the program with `./steam-haptics-player [AUDIO_FILE]`
7. Profit!


### Usage from command prompt:
	Usage: play-pcm.exe <file path>

### Tips
  - Try pressing the controller (while upright) down into a solid object, it will make the sound **much** louder

### Troubleshooting
- **ffmpeg not found**  
  - Install `ffmpeg` and ensure it is on your `PATH` (`winget install ffmpeg` on Windows).
- **if anything else fails:**
  - Try restart the program and controller, use another audio file to test,
  - Try connect the steam controller directly via USB and then try again.
  - No controller found on linux
    - Try running with sudo, if that works, you may need an appropriate udev rule for /dev/hidraw* devices


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
[v1.4.0]
(Thanks @iczero for your knowledge!), 
Add support for (and use) 16bit 8khz audio on wired, 
Use 8 bit µlaw for puck instead, for better audio quality, 
Simplify pcm setup immensely, 
Removed -s option as pcm setup is much faster

[v1.3.1]
Compile statically (so i dont have to ship dll files)

[v1.3.0]
Added progress bar, reworked how files are loaded

[v1.2.0]
Fixed controller detection with puck (Thanks @CrazyCritic89)

[v1.1.0]
Linux support, 
Checks for ffmpeg

[v1.0.0]
Initial release!

## maybe future stuff
 - volume control
 - looping music

I spent a lot of time and effort reverse engineering the Steam controller's firmware to find the HID commands needed to do this so i would appreciate the stars very much!


I was also heavily inspired to do this by CrazyCritic89's [SteamHapticsSinger](https://github.com/CrazyCritic89/SteamHapticsSinger). I suggest checking it out if you wish to play MIDI files on the steam controller!
(they honestly can sound better than this at times)

If you do happen to make a video with this, please do link back to this github repo so others can try too :steamhappy: