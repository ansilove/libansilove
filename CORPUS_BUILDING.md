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

#### Actual Downloads
- Downloaded 3 ACID artpacks (3.8M total)
- Downloaded 3 iCE artpacks (3.3M total)
- Downloaded 3 Fire artpacks (4.5M total)

#### Extraction & Organization
- Extracted all 9 artpacks successfully
- Organized 137 ANSI/ASCII files into ansi_files/ directory
- Note: iCE packs appear to use different format (.iCE extension) - only 1 .ans/.asc file found

#### Cleanup Plan
- Keep: ansi_files/ directory (137 organized ANSI files)
- Keep: Original .zip files for reference
- Remove: Extracted directories (acid-*/, ice*/, fire*/) to save space

#### Cleanup Completed
- Removed extracted directories (saved 20MB)
- Final corpus size: 15MB (down from 35MB)
- Kept original .zip files and organized ansi_files/

#### Reorganization Request
- User requested organizing files like 16colo.rs website
- 16colors structure: year/pack-name/files (preserves original pack context)
- Current structure: group/ansi_files/ (loses pack context)

#### Reorganization Implementation
- MISTAKE: Accidentally deleted downloaded artpacks during reorganization
- ACTION: Re-downloading artpacks to rebuild with proper structure

#### Reorganization Completed
- Re-downloaded all 9 artpacks into 1996/ directory
- Extracted each pack into its own subdirectory (e.g., 1996/acid-50a/)
- Structure now matches 16colo.rs: corpus/year/pack-name/files
- All original files preserved (executables, docs, art files)

## Final Status
**SUCCESS**: Corpus organized following 16colo.rs structure with 142 ANSI/ASCII art files from 1996.

### Usage Examples
```bash
# Fuzz with all packs
./fuzz-build/ansi -runs=10000 corpus/1996/

# Fuzz specific pack
./fuzz-build/ansi -runs=10000 corpus/1996/acid-50a/

# Test individual file
./build/example/ansilove_example corpus/1996/acid-50a/BS-ROCK1.ANS

# Browse like 16colors.net
ls corpus/1996/                    # List all packs from 1996
ls corpus/1996/acid-50a/           # List files in acid-50a pack
cat corpus/1996/acid-50a/FILE_ID.DIZ  # Read pack description
```

## Downloaded Artpacks

### ACID (ACiD Productions)
- [x] acid-50a.zip (1.3M)
- [x] acid-51a.zip (1.3M)
- [x] acid-52.zip (1.2M)

### iCE (Insane Creators Enterprise)
- [x] ice9601a.zip (1.1M)
- [x] ice9602a.zip (990K)
- [x] ice9603a.zip (1.2M)

### Fire
- [x] fire0296.zip (875K)
- [x] fire0496.zip (1.4M)
- [x] fire0696.zip (2.2M)

## Directory Structure
Organized to match 16colo.rs website structure:
```
corpus/
└── 1996/               # Year-based organization
    ├── acid-50a.zip    # Original artpack archive
    ├── acid-50a/       # Extracted pack (all files preserved)
    │   ├── FILE_ID.DIZ
    │   ├── NEWS-50.ANS
    │   ├── BS-ROCK1.ANS
    │   └── ... (28 ANSI files, 42 total files)
    ├── acid-51a.zip
    ├── acid-51a/       # (8 ANSI files, 27 total files)
    ├── acid-52.zip
    ├── acid-52/        # (4 ANSI files, 29 total files)
    ├── fire0296.zip
    ├── fire0296/       # (24 ANSI files, 52 total files)
    ├── fire0496.zip
    ├── fire0496/       # (27 ANSI files, 53 total files)
    ├── fire0696.zip
    ├── fire0696/       # (50 ANSI files, 78 total files)
    ├── ice9601a.zip
    ├── ice9601a/       # (1 ANSI file, 71 total files)
    ├── ice9602a.zip
    ├── ice9602a/       # (0 ANSI files, 75 total files)
    ├── ice9603a.zip
    └── ice9603a/       # (0 ANSI files, 86 total files)
```

## Statistics
- Total artpacks: 9 (3 ACID, 3 iCE, 3 Fire)
- Year: 1996
- Total files extracted: 513 files
- Total ANSI/ASCII files: 142 (.ans, .asc)
  - ACID packs (acid-50a, acid-51a, acid-52): 40 ANSI files
  - iCE packs (ice9601a, ice9602a, ice9603a): 1 ANSI file
  - Fire packs (fire0296, fire0496, fire0696): 101 ANSI files
- Disk space: 30 MB
  - Original .zip files: ~12 MB
  - Extracted files: ~18 MB

## Notes
- Focusing on .ANS, .ASC, .NFO files
- Excluding executables, images, and other binary formats
