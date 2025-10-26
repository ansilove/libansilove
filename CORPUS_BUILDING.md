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

#### Testing Download URLs
- HYPOTHESIS: Files named acid0996.zip, acid1096.zip exist in repository
- EXPERIMENT: Attempted to download these files
- RESULT: Downloaded HTML 404 pages instead of ZIP files
- CONCLUSION: These file names don't exist in the archive

#### Discovered Actual File Names (via GitHub API)
**1996 ACID packs:**
- acid-50a.zip, acid-50b.zip, acid-50c.zip
- acid-51a.zip, acid-51b.zip
- acid-52.zip, acid-53a.zip, acid-53b.zip

**1996 iCE packs:**
- ice9601a.zip, ice9601b.zip
- ice9602a.zip, ice9602b.zip
- ice9603a.zip, ice9603b.zip
- ice9604a.zip, ice9604b.zip

**1996 Fire packs:**
- fire0296.zip, fire0396.zip, fire0496.zip
- fire0596.zip, fire0696.zip, fire0796.zip
- fire0896.zip, fire0996.zip, fire1096.zip

#### Next Action
- Clean up invalid downloads
- Download actual artpack files

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
