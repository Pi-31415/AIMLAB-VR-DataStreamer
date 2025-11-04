# Icon Resources

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025  
**Version:** v1.0

## Icon Requirements

For a custom application icon, place the following files in this directory:

### Windows
- `icon.ico` - Windows icon (256x256, 128x128, 64x64, 48x48, 32x32, 16x16)

### macOS
- `icon.icns` - macOS icon (512x512, 256x256, 128x128, 64x64, 32x32, 16x16)

### Linux
- `icon.png` - PNG icon (512x512 recommended)

## Current Status

Currently building without custom icon. Electron-builder will use default Electron icon.

To add a custom icon:
1. Create an icon with NYU purple theme (#57068c)
2. Include a diamond symbol (◆) to match the app branding
3. Save as `icon.ico` for Windows
4. Rebuild with `npm run build`

## Icon Design Suggestions

- Background: NYU Purple gradient
- Symbol: White diamond or VR headset icon
- Text: Optional "AIMLAB" or "VR" text
- Style: Modern, flat design
- Format: Transparent background (PNG source) or white background (ICO)

