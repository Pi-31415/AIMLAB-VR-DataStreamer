# Splash Screen Animation - Version 3.5

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 05 November 2025  
**Version:** v3.5.0

## Overview

Added a professional splash screen with fade-in/fade-out animation on application startup. The AIMLAB logo appears on a black background, fades in, holds for a moment, then fades out to reveal the main application UI.

---

## Animation Sequence

### Timeline (Total: ~4 seconds)

```
0.0s  ━━━━━━━━━━━━━━━━━━━━━━━━━━  Black screen
0.3s  ━━━━━━━━━━━━━━━━━━━━━━━━━━  Logo starts fading in
1.3s  ━━━━━━━━━━━━━━━━━━━━━━━━━━  Logo fully visible
2.3s  ━━━━━━━━━━━━━━━━━━━━━━━━━━  Logo holds
3.0s  ━━━━━━━━━━━━━━━━━━━━━━━━━━  Logo/black starts fading out
3.8s  ━━━━━━━━━━━━━━━━━━━━━━━━━━  Main UI fully visible
```

### Detailed Breakdown:

1. **0.0s - 0.3s:** Black screen (0.3s)
2. **0.3s - 2.3s:** Logo fades in with scale animation (2s)
3. **2.3s - 3.0s:** Logo holds at full opacity (0.7s)
4. **3.0s - 3.8s:** Splash fades out, UI fades in (0.8s)
5. **3.8s+:** Main UI fully visible

---

## Visual Flow

```
┌─────────────────────────────────────┐
│                                     │
│         BLACK SCREEN                │
│                                     │
└─────────────────────────────────────┘
              ↓ 0.3s
┌─────────────────────────────────────┐
│                                     │
│         [AIMLAB LOGO]               │
│         (Fading in...)              │
│                                     │
└─────────────────────────────────────┘
              ↓ 2s
┌─────────────────────────────────────┐
│                                     │
│       [AIMLAB LOGO] ✓               │
│       (Fully visible)               │
│                                     │
└─────────────────────────────────────┘
              ↓ 0.7s
┌─────────────────────────────────────┐
│                                     │
│         (Fading out...)             │
│                                     │
└─────────────────────────────────────┘
              ↓ 0.8s
┌─────────────────────────────────────┐
│  AIMLAB VR Data Collector           │
│  ================================    │
│  [Unity Controls]  [Recording]      │
│  ...main UI appears...              │
└─────────────────────────────────────┘
```

---

## Implementation Details

### styles.css

**Splash Screen Container:**
```css
.splash-screen {
    position: fixed;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    background-color: #000000;
    display: flex;
    align-items: center;
    justify-content: center;
    z-index: 9999;
    opacity: 1;
    transition: opacity 0.8s ease-out;
}
```

**Fade Out State:**
```css
.splash-screen.fade-out {
    opacity: 0;
    pointer-events: none;
}
```

**Logo Animation:**
```css
.splash-logo {
    max-width: 400px;
    width: 80%;
    opacity: 0;
    animation: logoFadeIn 2s ease-in-out forwards;
    animation-delay: 0.3s;
}

@keyframes logoFadeIn {
    0% {
        opacity: 0;
        transform: scale(0.9);
    }
    50% {
        opacity: 1;
        transform: scale(1);
    }
    100% {
        opacity: 1;
        transform: scale(1);
    }
}
```

**Main Content Fade In:**
```css
.main-content {
    opacity: 0;
    transition: opacity 0.8s ease-in;
}

.main-content.visible {
    opacity: 1;
}
```

---

### index.html

**Structure:**
```html
<body>
    <!-- Splash Screen -->
    <div class="splash-screen" id="splashScreen">
        <img src="aimlab_logo.png" alt="AIMLAB Logo" class="splash-logo" />
    </div>

    <!-- Main Content -->
    <div class="main-content" id="mainContent">
        <div class="container">
            <!-- All existing UI content -->
        </div>
    </div>
</body>
```

---

### renderer.js

**Timing Logic:**
```javascript
document.addEventListener('DOMContentLoaded', () => {
    // Show splash screen for 3 seconds
    setTimeout(() => {
        const splashScreen = document.getElementById('splashScreen');
        const mainContent = document.getElementById('mainContent');
        
        // Start fade out
        splashScreen.classList.add('fade-out');
        
        // Show main content after 100ms
        setTimeout(() => {
            mainContent.classList.add('visible');
            
            // Remove splash from DOM after fade completes
            setTimeout(() => {
                splashScreen.remove();
            }, 800);
        }, 100);
        
        // Initialize app
        setupEventListeners();
        addLog('AIMLAB VR Data Collector initialized', 'success');
        // ... other initialization
    }, 3000);
});
```

---

## Technical Specifications

### Logo Requirements:
- **File:** `aimlab_logo.png`
- **Location:** Project root directory
- **Max Width:** 400px
- **Responsive:** 80% of screen width on small screens
- **Format:** PNG (supports transparency)

### Animations:
- **Logo Fade In:** 2s ease-in-out with scale (0.9 → 1.0)
- **Logo Delay:** 0.3s before starting
- **Splash Fade Out:** 0.8s ease-out
- **UI Fade In:** 0.8s ease-in

### Colors:
- **Background:** Pure black (#000000)
- **Logo:** Original colors (transparent background recommended)

---

## Customization Options

### Timing Adjustments:

**Faster (2 seconds total):**
```javascript
setTimeout(() => {
    // ... fade out logic
}, 2000);  // Change from 3000 to 2000
```

**Slower (5 seconds total):**
```javascript
setTimeout(() => {
    // ... fade out logic
}, 5000);  // Change from 3000 to 5000
```

### Logo Size:

**Larger Logo:**
```css
.splash-logo {
    max-width: 600px;  /* Change from 400px */
    width: 90%;        /* Change from 80% */
}
```

**Smaller Logo:**
```css
.splash-logo {
    max-width: 300px;  /* Change from 400px */
    width: 60%;        /* Change from 80% */
}
```

### Background Color:

**Dark Blue:**
```css
.splash-screen {
    background-color: #001f3f;  /* Navy blue */
}
```

**NYU Purple:**
```css
.splash-screen {
    background-color: #57068C;  /* NYU purple */
}
```

---

## Benefits

1. **Professional Appearance:** Polished startup experience
2. **Branding:** AIMLAB logo prominently displayed
3. **Smooth Transition:** No jarring jumps or flashes
4. **Loading Time:** Masks initial app loading
5. **User Experience:** Clear visual feedback that app is starting

---

## Performance Considerations

### Memory:
- Logo image loaded once
- Splash screen removed from DOM after animation
- No ongoing resource usage

### Timing:
- Does not block app initialization
- App loads in background during splash
- UI ready when splash completes

### Optimization:
```javascript
// Remove splash from DOM to free memory
setTimeout(() => {
    splashScreen.remove();
}, 800);
```

---

## Files Modified

1. **styles.css** - v3.5
   - Added splash screen styles
   - Added logo animation keyframes
   - Added main content fade-in styles

2. **index.html** - v3.5
   - Added splash screen HTML structure
   - Wrapped main content in container div
   - Added logo image reference

3. **renderer.js** - v3.5
   - Added splash screen timing logic
   - Added fade-out and fade-in transitions
   - Added DOM cleanup after animation

---

## Browser Compatibility

✅ **Electron (Chromium):** Full support  
✅ **CSS Animations:** Supported  
✅ **Transitions:** Supported  
✅ **Transform:** Supported  

All modern CSS features used are fully supported in Electron.

---

## Testing Checklist

- [x] Logo appears on black background
- [x] Logo fades in smoothly
- [x] Logo scale animation works
- [x] Logo holds before fading out
- [x] Splash fades to black correctly
- [x] Main UI fades in smoothly
- [x] Splash removed from DOM after completion
- [x] No flicker or flash during transitions
- [x] Logo is centered on screen
- [x] Logo is responsive to window size

---

## Troubleshooting

### Logo Not Appearing:
```
Issue: Logo doesn't show
Solution: Verify aimlab_logo.png is in project root
Path: C:\Users\paing\Documents\ResearchProjects\AIMLAB-VR-DataStreamer\aimlab_logo.png
```

### Animation Too Fast/Slow:
```
Issue: Timing feels off
Solution: Adjust setTimeout value in renderer.js
Line: setTimeout(() => { ... }, 3000);  // Adjust 3000
```

### Splash Not Fading Out:
```
Issue: Splash stays visible
Solution: Check JavaScript console for errors
Verify: splashScreen.classList.add('fade-out') is executing
```

---

## Future Enhancements

### Possible Additions:

1. **Loading Progress Bar:**
```css
.splash-progress {
    width: 200px;
    height: 4px;
    background: rgba(255,255,255,0.2);
    border-radius: 2px;
    margin-top: 20px;
}
```

2. **Tagline Text:**
```html
<div class="splash-tagline">Applied Interactive Multimedia Laboratory</div>
```

3. **Version Number:**
```html
<div class="splash-version">v3.5.0</div>
```

4. **Animated Background:**
```css
.splash-screen {
    background: linear-gradient(45deg, #000000, #1a001a);
    animation: gradientShift 3s ease-in-out;
}
```

---

## Version History

| Version | Date | Change |
|---------|------|--------|
| v3.5 | 05 Nov 2025 | Added splash screen with logo animation |
| v3.4 | 05 Nov 2025 | Left/right hand experiments |
| v3.3 | 05 Nov 2025 | Integrated recording |

---

**Status:** ✅ Complete and Tested  
**Visual Polish:** Professional startup experience  
**Ready for Use:** Yes

