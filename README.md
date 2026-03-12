# AHRITREC

R package bindings for the AHRI TRE C core.

## Prerequisites

<<<<<<< HEAD
Build the C core first (from your AHRI_TRE.C checkout):
=======
## C ABI symbol policy

The R bridge now uses Julia-style unprefixed C symbols as the primary binding targets (for example: `version`, `last_error`, `map_value_type`).

Prefixed symbols (for example: `ahri_tre_version`, `ahri_tre_last_error`) are still exported by the C core as compatibility aliases and remain safe for existing code.

## Build C core first
>>>>>>> fc2e963b67ebb664176554cfadfa715565811bb2

```bash
cmake -S c_core -B c_core/build
cmake --build c_core/build --config Release
```

If the C repository is not discoverable as a sibling checkout, set one of:

```r
Sys.setenv(AHRI_TRE_C_ROOT = "C:/path/to/AHRI_TRE.C")
# or
Sys.setenv(AHRI_TRE_C_LIB = "C:/path/to/ahri_tre_c.dll")
```

## Install

From this package root directory:

```r
install.packages(".", repos = NULL, type = "source")
```

## Usage

```r
library(AHRITREC)
ahri_tre_load()
ahri_tre_version()
```
