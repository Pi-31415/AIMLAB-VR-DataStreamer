# AIMLAB VR Streamer - UI Layout Reference

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025  
**Version:** v1.0

---

## Application Layout Description

```
┌────────────────────────────────────────────────────────────────────┐
│  HEADER (NYU Purple Gradient Background)                          │
│  ┌──────────────────────────────────────────────────────────────┐ │
│  │  ◆ AIMLAB VR Streamer          [●] Disconnected             │ │
│  └──────────────────────────────────────────────────────────────┘ │
└────────────────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────────────────┐
│  CONNECTION SETTINGS                                               │
│  ──────────────────────────────────────────────────────────────   │
│                                                                    │
│  IP Address:                                                       │
│  [127.0.0.1                                              ]         │
│                                                                    │
│  Port:                                                             │
│  [8888                                                   ]         │
│                                                                    │
│  [▶ Connect]  [■ Disconnect]                                       │
└────────────────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────────────────┐
│  STREAMING CONTROLS                                                │
│  ──────────────────────────────────────────────────────────────   │
│                                                                    │
│  [● Start Streaming]  [● Stop Streaming]                           │
│                                                                    │
│  Data Transmission Progress:                                       │
│  ┌──────────────────────────────────────────────────────────────┐ │
│  │███████████░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░ 25.5%  │ │
│  └──────────────────────────────────────────────────────────────┘ │
│                                                                    │
│  Buffer Status:                                                    │
│  ┌──────────────────────────────────────────────────────────────┐ │
│  │████████████████████████░░░░░░░░░░░░░░░░░░░░░░░░░░░░ 45.2%   │ │
│  └──────────────────────────────────────────────────────────────┘ │
└────────────────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────────────────┐
│  STATISTICS                                                        │
│  ──────────────────────────────────────────────────────────────   │
│                                                                    │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  ┌────────┐│
│  │Data Points   │  │Packets/Sec   │  │Total Data    │  │Uptime  ││
│  │   Sent       │  │              │  │   Sent       │  │        ││
│  │              │  │              │  │              │  │        ││
│  │   1,234      │  │     45       │  │  125.5 KB    │  │00:05:32││
│  └──────────────┘  └──────────────┘  └──────────────┘  └────────┘│
└────────────────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────────────────┐
│  ACTIVITY LOG                                                      │
│  ──────────────────────────────────────────────────────────────   │
│  ┌──────────────────────────────────────────────────────────────┐ │
│  │ 00:00:00  Application started successfully                   │ │
│  │ 00:00:01  Ready to connect to VR system                      │ │
│  │ 00:01:15  Connecting to 127.0.0.1:8888...                    │ │
│  │ 00:01:16  Successfully connected to 127.0.0.1:8888           │ │
│  │ 00:01:20  Starting data stream...                            │ │
│  │ 00:01:21  Data streaming started successfully                │ │
│  │                                                              │ │
│  └──────────────────────────────────────────────────────────────┘ │
└────────────────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────────────────┐
│  FOOTER                                                            │
│  AIMLAB VR Data Streamer v1.0 | NYU Research Project              │
│  Author: Pi Ko (pi.ko@nyu.edu)                                     │
└────────────────────────────────────────────────────────────────────┘
```

---

## Color Legend

### Header
- Background: Purple gradient (#57068c → #330577)
- Text: White (#FFFFFF)
- Status Dot: Red (disconnected) / Green (connected)

### Panels
- Background: White (#FFFFFF)
- Border: Light Gray (#E0E0E0) → Purple on hover (#f0e5f9)
- Title Bar: Purple underline (#57068c)

### Buttons
- **Connect (Primary)**: Purple background (#57068c)
- **Disconnect (Secondary)**: Gray background (#757575)
- **Start Streaming (Success)**: Green background (#4CAF50)
- **Stop Streaming (Danger)**: Red background (#F44336)
- All buttons: White text, shadow on hover

### Progress Bars
- Background: Light gray (#F5F5F5)
- Data Progress Fill: Purple gradient (#57068c → #8900e1)
- Buffer Progress Fill: Blue gradient (#2196F3 → #64B5F6)
- Progress Text: Dark gray (#212121)

### Statistics Cards
- Background: Off-white gradient (#F5F5F5 → #FAFAFA)
- Left Border: Purple accent (#57068c)
- Label: Gray text (#666666)
- Value: Large purple numbers (#57068c)

### Activity Log
- Background: Light gray (#F5F5F5)
- Info entries: Blue accent (#2196F3)
- Success entries: Green accent (#4CAF50)
- Warning entries: Orange accent (#FF9800)
- Error entries: Red accent (#F44336)

### Footer
- Background: Light gray (#F5F5F5)
- Text: Medium gray (#666666)
- Top border: Light gray (#E0E0E0)

---

## Interactive States

### Button States
1. **Normal**: Full color, subtle shadow
2. **Hover**: Lifted effect (translateY -2px), larger shadow
3. **Active**: Pressed down (translateY 0)
4. **Disabled**: 50% opacity, no hover effects

### Input Fields
1. **Normal**: Gray border
2. **Focus**: Purple border with purple glow
3. **Error**: Red border (if validation fails)

### Status Indicator
1. **Disconnected**: Red pulsing dot
2. **Connected**: Green pulsing dot
3. Animation: Pulse every 2 seconds (opacity 1 → 0.5 → 1)

### Panel Interaction
1. **Normal**: Light shadow
2. **Hover**: Enhanced shadow, purple-tinted border

---

## Responsive Breakpoints

### Desktop (≥ 768px)
- Full multi-column layout
- Statistics cards in 4-column grid
- Side-by-side buttons

### Mobile/Tablet (< 768px)
- Single column layout
- Statistics cards stacked vertically
- Full-width buttons
- Reduced padding

---

## Typography

### Fonts
- **System Font Stack**: -apple-system, BlinkMacSystemFont, 'Segoe UI', 'Roboto', 'Oxygen', 'Ubuntu', 'Cantarell', 'Fira Sans', 'Droid Sans', 'Helvetica Neue', sans-serif
- **Monospace (Log)**: 'Courier New', Courier, monospace

### Sizes
- App Title: 28px
- Panel Titles: 20px
- Buttons: 16px
- Labels: 14px
- Log Entries: 13px
- Footer: 13px
- Statistics Values: 28px

### Weights
- Titles: 600 (Semi-bold)
- Buttons: 600 (Semi-bold)
- Labels: 500 (Medium)
- Statistics Values: 700 (Bold)
- Body Text: 400 (Regular)

---

## Spacing System

- **XS**: 4px - Minimal gaps
- **SM**: 8px - Tight spacing
- **MD**: 16px - Standard spacing
- **LG**: 24px - Section spacing
- **XL**: 32px - Major sections

## Border Radius

- **SM**: 4px - Small elements
- **MD**: 8px - Buttons, inputs
- **LG**: 12px - Panels, containers

---

## Accessibility Features

1. **High Contrast**: All text meets WCAG AA standards
2. **Focus Indicators**: Clear purple glow on focused elements
3. **Color Independence**: Icons and text supplement color coding
4. **Scalable Text**: Relative font sizes
5. **Keyboard Navigation**: Full tab order support
6. **Screen Reader**: Semantic HTML structure

---

**Note**: This reference describes the visual layout. The actual application renders dynamically with Electron and includes smooth animations and transitions.

