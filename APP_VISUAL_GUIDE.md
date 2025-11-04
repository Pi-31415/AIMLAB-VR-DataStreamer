# AIMLAB VR Streamer - Visual Guide

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025  
**Version:** v1.0

---

## 🎨 Visual Design Overview

This document describes what the AIMLAB VR Streamer application looks like when running.

---

## 📸 Application Appearance

### Window Properties
- **Title**: "AIMLAB VR Streamer"
- **Default Size**: 1200px × 800px
- **Minimum Size**: 800px × 600px
- **Background**: Solid White (#FFFFFF)
- **Resizable**: Yes
- **Frame**: Standard OS window frame

---

## 🎨 Visual Hierarchy (Top to Bottom)

### 1️⃣ HEADER SECTION
```
╔════════════════════════════════════════════════════════════╗
║  [Purple Gradient Background]                              ║
║                                                            ║
║  ◆ AIMLAB VR Streamer        [●] Disconnected            ║
║     [White Text - 28px]       [Status Badge - Animated]   ║
╚════════════════════════════════════════════════════════════╝
```

**Visual Details:**
- **Background**: Purple to dark purple gradient (left to right)
- **Title**: Large white text with diamond icon
- **Status**: Floating badge with animated dot
  - Red pulsing dot when disconnected
  - Green pulsing dot when connected

---

### 2️⃣ CONNECTION SETTINGS PANEL
```
┌────────────────────────────────────────────────────────────┐
│  CONNECTION SETTINGS                                       │
│  ━━━━━━━━━━━━━━━━━━━━━━━ [Purple underline]              │
│                                                            │
│  IP Address:                                               │
│  ┌──────────────────────────────────────────────────────┐ │
│  │ 127.0.0.1                                            │ │
│  └──────────────────────────────────────────────────────┘ │
│                                                            │
│  Port:                                                     │
│  ┌──────────────────────────────────────────────────────┐ │
│  │ 8888                                                 │ │
│  └──────────────────────────────────────────────────────┘ │
│                                                            │
│  ┌─────────────────┐  ┌─────────────────┐                │
│  │  ▶  Connect     │  │  ■  Disconnect  │                │
│  │  [Purple Btn]   │  │  [Gray Btn]     │                │
│  └─────────────────┘  └─────────────────┘                │
└────────────────────────────────────────────────────────────┘
```

**Visual Details:**
- **Panel**: White background, gray border, subtle shadow
- **Title**: Purple with thick underline
- **Inputs**: Light border, purple glow on focus
- **Buttons**: 
  - Connect: NYU Purple with white text
  - Disconnect: Gray (disabled initially)
  - Both have shadow and lift on hover

---

### 3️⃣ STREAMING CONTROLS PANEL
```
┌────────────────────────────────────────────────────────────┐
│  STREAMING CONTROLS                                        │
│  ━━━━━━━━━━━━━━━━━━━━━━━ [Purple underline]              │
│                                                            │
│  ┌─────────────────────┐  ┌─────────────────────┐        │
│  │  ●  Start Streaming │  │  ●  Stop Streaming  │        │
│  │  [Green Button]     │  │  [Red Button]       │        │
│  └─────────────────────┘  └─────────────────────┘        │
│                                                            │
│  Data Transmission Progress:                               │
│  ┌──────────────────────────────────────────────────────┐ │
│  │ ████████████████░░░░░░░░░░░░░░░░░░░░░░░░░░░░ 35.2%  │ │
│  │ [Purple Gradient Fill]                               │ │
│  └──────────────────────────────────────────────────────┘ │
│                                                            │
│  Buffer Status:                                            │
│  ┌──────────────────────────────────────────────────────┐ │
│  │ ██████████████████████████░░░░░░░░░░░░░░░░░░ 52.8%  │ │
│  │ [Blue Gradient Fill]                                 │ │
│  └──────────────────────────────────────────────────────┘ │
└────────────────────────────────────────────────────────────┘
```

**Visual Details:**
- **Buttons**:
  - Start: Green (#4CAF50) with white text
  - Stop: Red (#F44336) with white text
  - Both disabled until connected
- **Progress Bars**:
  - Container: Light gray with border
  - Fill: Smooth gradient animation
  - Text: Centered percentage overlay
  - Height: 32px with rounded corners

---

### 4️⃣ STATISTICS PANEL
```
┌────────────────────────────────────────────────────────────┐
│  STATISTICS                                                │
│  ━━━━━━━━━━━━━━━━━━━━━━━ [Purple underline]              │
│                                                            │
│  ┌──────────────┐ ┌──────────────┐ ┌──────────────┐ ┌───┐│
│  │ Data Points  │ │ Packets/Sec  │ │ Total Data   │ │   ││
│  │    Sent      │ │              │ │    Sent      │ │   ││
│  │              │ │              │ │              │ │   ││
│  │   1,234      │ │     45       │ │  125.5 KB    │ │00:││
│  │  [Purple]    │ │  [Purple]    │ │  [Purple]    │ │[P]││
│  └──────────────┘ └──────────────┘ └──────────────┘ └───┘│
└────────────────────────────────────────────────────────────┘
```

**Visual Details:**
- **Card Layout**: 4 equal cards (responsive grid)
- **Card Design**:
  - Light gray background
  - Purple left border (4px thick)
  - Centered content
  - Label: Small gray text on top
  - Value: Large purple number (28px)
- **Animation**: Numbers update smoothly

---

### 5️⃣ ACTIVITY LOG PANEL
```
┌────────────────────────────────────────────────────────────┐
│  ACTIVITY LOG                                              │
│  ━━━━━━━━━━━━━━━━━━━━━━━ [Purple underline]              │
│  ┌──────────────────────────────────────────────────────┐ │
│  │ [Monospace Font - Light Gray Background]            │ │
│  │                                                      │ │
│  │ ├─ 00:00:00  Application started successfully       │ │
│  │ │  [Blue accent line]                               │ │
│  │                                                      │ │
│  │ ├─ 00:00:01  Ready to connect to VR system          │ │
│  │ │  [Blue accent line]                               │ │
│  │                                                      │ │
│  │ ├─ 00:01:15  Successfully connected                 │ │
│  │ │  [Green accent line]                              │ │
│  │                                                      │ │
│  │ ├─ 00:01:20  Data streaming started                 │ │
│  │ │  [Green accent line]                              │ │
│  │                                                      │ │
│  │ ⋮  [Auto-scrolls as more entries added]            │ │
│  └──────────────────────────────────────────────────────┘ │
└────────────────────────────────────────────────────────────┘
```

**Visual Details:**
- **Container**: Light gray background, scrollable
- **Max Height**: 250px
- **Entries**: Color-coded by type
  - Info: Blue left border
  - Success: Green left border
  - Warning: Orange left border
  - Error: Red left border
- **Format**: `[Time] Message`
- **Scrollbar**: Custom purple scrollbar

---

### 6️⃣ FOOTER SECTION
```
╔════════════════════════════════════════════════════════════╗
║  AIMLAB VR Data Streamer v1.0 | NYU Research Project      ║
║  Author: Pi Ko (pi.ko@nyu.edu)                             ║
╚════════════════════════════════════════════════════════════╝
```

**Visual Details:**
- **Background**: Light gray (#F5F5F5)
- **Border**: Gray line on top
- **Text**: Centered, medium gray
- **Font Size**: 13px

---

## 🎭 Interactive Elements

### Hover Effects
1. **Buttons**: 
   - Lift up 2px
   - Shadow increases
   - Smooth 0.3s transition

2. **Panels**:
   - Border changes to purple tint
   - Shadow increases
   - Subtle highlight

3. **Inputs**:
   - Border changes to purple
   - Purple glow appears
   - Cursor changes to text input

### Click Effects
1. **Buttons**:
   - Press down animation
   - Ripple effect (visual feedback)
   - State change (enabled/disabled)

### Animations
1. **Status Dot**: Pulse every 2 seconds
2. **Progress Bars**: Smooth width transition (0.5s)
3. **Log Entries**: Fade in when added
4. **Statistics**: Count-up animation

---

## 📱 Responsive Behavior

### Desktop (1200px+)
- Full multi-column layout
- 4-column statistics grid
- Side-by-side buttons

### Tablet (768px - 1200px)
- Adjusted column widths
- 2-column statistics grid
- Comfortable spacing

### Mobile (< 768px)
- Single column layout
- Stacked statistics
- Full-width buttons
- Reduced padding

---

## 🎨 Color Usage Map

| Element | Normal | Hover | Active | Disabled |
|---------|--------|-------|--------|----------|
| Primary Button | Purple | Dark Purple | Purple | Light Purple |
| Secondary Button | Gray | Dark Gray | Gray | Light Gray |
| Success Button | Green | Dark Green | Green | Light Green |
| Danger Button | Red | Dark Red | Red | Light Red |
| Input Border | Gray | Purple | Purple | Light Gray |
| Panel Border | Gray | Purple Tint | - | Gray |

---

## 🌈 Visual Consistency

### Spacing
- Consistent 16px padding in panels
- 24px gaps between panels
- 8px gaps between related elements

### Borders
- 2px solid borders everywhere
- 8px border radius on buttons/inputs
- 12px border radius on panels

### Shadows
- Subtle shadows on panels
- Enhanced shadows on hover
- No shadows on flat elements

### Typography
- Consistent font family
- Size hierarchy maintained
- Weight hierarchy maintained

---

## ✨ Polish Details

1. **Smooth Transitions**: All interactions animated (0.3s)
2. **Focus States**: Clear purple outline on focused elements
3. **Loading States**: Animated progress bars
4. **Empty States**: Placeholder content shown
5. **Error States**: Red borders and messages
6. **Success States**: Green accents and animations

---

## 🎯 Accessibility Features

1. **Color Contrast**: WCAG AA compliant
2. **Focus Indicators**: Clearly visible
3. **Text Scaling**: Supports zoom
4. **Keyboard Navigation**: Full support
5. **Screen Reader**: Semantic HTML

---

## 📐 Measurements

### Dimensions
- **Window**: 1200×800px (default)
- **Min Window**: 800×600px
- **Header**: Full width, ~80px height
- **Panel Padding**: 24px
- **Button Height**: 48px
- **Input Height**: 48px
- **Progress Bar**: 32px height
- **Footer**: Full width, ~60px height

### Spacing
- **Between Panels**: 24px
- **Inside Panels**: 16px
- **Button Gaps**: 16px
- **Vertical Rhythm**: 8px base

---

**Note**: This visual guide describes the appearance. For layout structure, see `UI_LAYOUT_REFERENCE.md`. For technical details, see `README_ELECTRON.md`.

---

*The application is live and interactive. These descriptions show static representations of a dynamic, animated interface.*

