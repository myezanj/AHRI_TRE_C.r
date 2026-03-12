# AHRITREC

R package bindings for the AHRI TRE C core.

## Prerequisites

Build the C core first (from your AHRI_TRE.C checkout):

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
