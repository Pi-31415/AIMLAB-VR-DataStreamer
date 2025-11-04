# AIMLAB VR Data Streamer - Documentation Index

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025  
**Version:** v1.0

---

## 📚 Documentation Navigation

Welcome to the AIMLAB VR Data Streamer project! This index will help you find the right documentation for your needs.

---

## 🚀 New Users Start Here

### 1. [GETTING_STARTED.md](GETTING_STARTED.md) 📖 **START HERE**
**For:** Complete beginners  
**Time:** 15 minutes  
**What you'll learn:**
- How to verify your setup
- How to build the application
- How to run your first recording
- How to analyze your data

**Start with this if:** You're new to the project and want to get up and running quickly.

---

## 📖 Documentation by Purpose

### Quick Reference

#### [QUICK_START.md](QUICK_START.md) ⚡
**For:** Users who need a quick reminder  
**Time:** 2 minutes  
**Contains:**
- 5-minute setup procedure
- Common tasks (test vibration, record, analyze)
- File outputs location
- Quick tips

**Use this when:** You know the basics and just need a quick reference.

---

### Complete Guides

#### [README.md](README.md) 📘
**For:** All users  
**Time:** 10 minutes  
**Contains:**
- Project overview
- Features list
- Requirements
- Build instructions
- Usage guide
- Network configuration
- Data format
- Troubleshooting basics

**Use this when:** You want a complete overview of the project.

#### [SETUP_GUIDE.md](SETUP_GUIDE.md) 🔧
**For:** First-time setup and troubleshooting  
**Time:** 30 minutes  
**Contains:**
- Detailed system requirements
- Step-by-step PC setup
- Arduino hardware and software setup
- Unity VR configuration
- Network configuration
- Complete testing procedures
- Advanced troubleshooting
- Configuration options

**Use this when:** You're setting up for the first time or troubleshooting issues.

---

### Technical Documentation

#### [PROJECT_OVERVIEW.md](PROJECT_OVERVIEW.md) 🏗️
**For:** Developers and researchers  
**Time:** 20 minutes  
**Contains:**
- System architecture
- Component interaction diagrams
- Network protocol specification
- Serial protocol specification
- Data format details
- Performance specifications
- Use cases
- Customization options
- Dependencies
- Version history

**Use this when:** You need to understand how the system works or want to customize it.

#### [IMPLEMENTATION_SUMMARY.md](IMPLEMENTATION_SUMMARY.md) 📊
**For:** Project managers and technical reviewers  
**Time:** 10 minutes  
**Contains:**
- Complete deliverables list
- Features implemented
- Technical specifications
- Code statistics
- Testing performed
- Design decisions
- Performance metrics
- Project status

**Use this when:** You need a technical summary of what was implemented.

---

## 📁 Documentation by User Type

### For Experimenters
1. Start: [GETTING_STARTED.md](GETTING_STARTED.md)
2. Reference: [QUICK_START.md](QUICK_START.md)
3. Help: [SETUP_GUIDE.md](SETUP_GUIDE.md) → Troubleshooting section

### For Developers
1. Overview: [PROJECT_OVERVIEW.md](PROJECT_OVERVIEW.md)
2. Implementation: [IMPLEMENTATION_SUMMARY.md](IMPLEMENTATION_SUMMARY.md)
3. Source Code: Review `main.cpp`, Arduino, and Unity files

### For System Administrators
1. Setup: [SETUP_GUIDE.md](SETUP_GUIDE.md)
2. Network: [PROJECT_OVERVIEW.md](PROJECT_OVERVIEW.md) → Network Protocol section
3. Testing: Run `verify_setup.bat`

### For Data Analysts
1. Quick Start: [QUICK_START.md](QUICK_START.md) → Record and Analyze
2. Analysis Script: Review `analyze_vr_data.py`
3. Data Format: [PROJECT_OVERVIEW.md](PROJECT_OVERVIEW.md) → Data Format section

---

## 🎯 Common Scenarios

### "I just want to get started"
→ [GETTING_STARTED.md](GETTING_STARTED.md)

### "I need to record VR data right now"
→ [QUICK_START.md](QUICK_START.md)

### "Something isn't working"
→ [SETUP_GUIDE.md](SETUP_GUIDE.md) → Troubleshooting

### "I want to understand the system"
→ [PROJECT_OVERVIEW.md](PROJECT_OVERVIEW.md)

### "I need to customize the code"
→ [PROJECT_OVERVIEW.md](PROJECT_OVERVIEW.md) → Customization Options

### "I need technical specifications"
→ [IMPLEMENTATION_SUMMARY.md](IMPLEMENTATION_SUMMARY.md)

---

## 📝 File Descriptions

### Source Code Files

| File | Description | Lines | Language |
|------|-------------|-------|----------|
| `main.cpp` | Console application source | 1,069 | C++ |
| `Arduino_Vibration_Controller.ino` | Arduino firmware | 181 | Arduino C++ |
| `Unity_VRDataStreamer.cs` | Unity VR script | 429 | C# |
| `analyze_vr_data.py` | Data analysis script | 370 | Python |

### Build and Utility Scripts

| File | Description | Purpose |
|------|-------------|---------|
| `build.bat` | Build console app | Compile main.cpp |
| `analyze.bat` | Run analysis | Launch Python script |
| `verify_setup.bat` | Verify setup | Check dependencies |

### Configuration Files

| File | Description | Purpose |
|------|-------------|---------|
| `requirements.txt` | Python dependencies | pip install |

### Executables

| File | Description | Size |
|------|-------------|------|
| `AIMLAB_Console.exe` | Console application | ~286 KB |

---

## 🔍 Finding Information

### By Topic

#### Setup and Installation
- System requirements → [README.md](README.md) + [SETUP_GUIDE.md](SETUP_GUIDE.md)
- Building the app → [GETTING_STARTED.md](GETTING_STARTED.md)
- Arduino setup → [SETUP_GUIDE.md](SETUP_GUIDE.md) → Arduino Setup
- Unity setup → [SETUP_GUIDE.md](SETUP_GUIDE.md) → Unity Setup

#### Usage
- First recording → [GETTING_STARTED.md](GETTING_STARTED.md)
- Testing vibration → [QUICK_START.md](QUICK_START.md)
- Analyzing data → [QUICK_START.md](QUICK_START.md)
- Menu options → [README.md](README.md)

#### Technical Details
- Network protocol → [PROJECT_OVERVIEW.md](PROJECT_OVERVIEW.md)
- Serial protocol → [PROJECT_OVERVIEW.md](PROJECT_OVERVIEW.md)
- Data format → [PROJECT_OVERVIEW.md](PROJECT_OVERVIEW.md)
- Architecture → [PROJECT_OVERVIEW.md](PROJECT_OVERVIEW.md)

#### Troubleshooting
- Common issues → [SETUP_GUIDE.md](SETUP_GUIDE.md)
- Error messages → [SETUP_GUIDE.md](SETUP_GUIDE.md)
- Network problems → [SETUP_GUIDE.md](SETUP_GUIDE.md)
- Arduino issues → [SETUP_GUIDE.md](SETUP_GUIDE.md)

---

## 📚 Recommended Reading Order

### Path 1: Quick Start (15 minutes)
1. [GETTING_STARTED.md](GETTING_STARTED.md) - Get running
2. [QUICK_START.md](QUICK_START.md) - Quick reference

### Path 2: Complete Understanding (1 hour)
1. [GETTING_STARTED.md](GETTING_STARTED.md) - Basics
2. [README.md](README.md) - Overview
3. [SETUP_GUIDE.md](SETUP_GUIDE.md) - Details
4. [PROJECT_OVERVIEW.md](PROJECT_OVERVIEW.md) - Architecture

### Path 3: Developer Track (2 hours)
1. [PROJECT_OVERVIEW.md](PROJECT_OVERVIEW.md) - System design
2. [IMPLEMENTATION_SUMMARY.md](IMPLEMENTATION_SUMMARY.md) - What's implemented
3. Source code review - `main.cpp`, Arduino, Unity
4. [SETUP_GUIDE.md](SETUP_GUIDE.md) - Advanced configuration

---

## 🎓 Learning Resources

### Tutorials Included
- **Tutorial 1**: First Recording - In [GETTING_STARTED.md](GETTING_STARTED.md)
- **Tutorial 2**: Vibration Timing - In [GETTING_STARTED.md](GETTING_STARTED.md)
- **Tutorial 3**: Multiple Trials - In [GETTING_STARTED.md](GETTING_STARTED.md)

### Example Workflows
- Basic experiment - [QUICK_START.md](QUICK_START.md)
- Complete experiment - [PROJECT_OVERVIEW.md](PROJECT_OVERVIEW.md)

### Scripts and Tools
- Build script: `build.bat`
- Analysis script: `analyze_vr_data.py`
- Verification: `verify_setup.bat`
- Analysis launcher: `analyze.bat`

---

## 🆘 Getting Help

### Self-Service
1. Check [GETTING_STARTED.md](GETTING_STARTED.md) → Common Questions
2. Run `verify_setup.bat` to check your system
3. Review [SETUP_GUIDE.md](SETUP_GUIDE.md) → Troubleshooting

### Contact Support
**Email:** pi.ko@nyu.edu

**Include:**
- What you were trying to do
- Error messages (screenshot or copy/paste)
- Output from `verify_setup.bat`
- Which documentation you've already checked

---

## ✅ Quick Checklist

Before starting your project:
- [ ] Read [GETTING_STARTED.md](GETTING_STARTED.md)
- [ ] Run `verify_setup.bat`
- [ ] Build with `build.bat`
- [ ] Test with a short recording
- [ ] Analyze test data with `analyze.bat`
- [ ] Keep [QUICK_START.md](QUICK_START.md) handy for reference

---

## 📊 Documentation Statistics

| Document | Lines | Purpose | Audience |
|----------|-------|---------|----------|
| INDEX.md | This file | Navigation | Everyone |
| GETTING_STARTED.md | 450 | Tutorial | Beginners |
| QUICK_START.md | 150 | Reference | All users |
| README.md | 250 | Overview | All users |
| SETUP_GUIDE.md | 550 | Setup | First-time users |
| PROJECT_OVERVIEW.md | 500 | Architecture | Developers |
| IMPLEMENTATION_SUMMARY.md | 600 | Status | Technical review |

**Total Documentation:** ~2,500 lines across 7 files

---

## 🎯 Quick Links

- **Official Start** → [GETTING_STARTED.md](GETTING_STARTED.md)
- **Quick Reference** → [QUICK_START.md](QUICK_START.md)
- **Troubleshooting** → [SETUP_GUIDE.md](SETUP_GUIDE.md)
- **System Details** → [PROJECT_OVERVIEW.md](PROJECT_OVERVIEW.md)
- **Project Status** → [IMPLEMENTATION_SUMMARY.md](IMPLEMENTATION_SUMMARY.md)

---

**Happy Experimenting!**

For questions or support, contact: **Pi Ko (pi.ko@nyu.edu)**

---

**Document Version:** 1.0  
**Last Updated:** 04 November 2025  
**Maintained by:** Pi Ko

