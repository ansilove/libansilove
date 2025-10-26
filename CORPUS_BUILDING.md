# ANSI/ASCII Art Corpus Building Progress

**Source**: https://github.com/sixteencolors/sixteencolors-archive  
**Date Started**: 2025-10-26  
**Target Groups**: ACID, iCE, Fire

## Objectives
- Download sample artpacks from major ANSI art groups (ACID, iCE, Fire)
- Extract and organize ANSI art files
- Build a test corpus for libansilove fuzzing and validation

## Progress Log

### 2025-10-26

#### Initial Setup
- Created progress tracking file
- Set up todo list for corpus building
- Researched sixteencolors-archive structure (organized by year: 1990-2024)

#### Repository Access
- Attempted full clone - repository too large (timed out)
- Strategy: Download individual artpacks via direct URLs from GitHub

#### Directory Setup
- Created corpus/ directory with subdirectories: acid/, ice/, fire/

#### Downloads - ACID
- Downloaded 3 ACID artpacks (acid0996, acid1096, acid1295) - total 861K

#### Next Action
- Downloading iCE artpacks

## Downloaded Artpacks

### ACID (ACiD Productions)
- [x] acid0996.zip (287K)
- [x] acid1096.zip (287K)
- [x] acid1295.zip (287K)

### iCE (Insane Creators Enterprise)
- [ ] To be downloaded

### Fire
- [ ] To be downloaded

## Directory Structure
```
corpus/
├── acid/
├── ice/
└── fire/
```

## Statistics
- Total artpacks downloaded: 0
- Total ANSI files extracted: 0
- Disk space used: 0 MB

## Notes
- Focusing on .ANS, .ASC, .NFO files
- Excluding executables, images, and other binary formats
