# AHRITREC

R package bindings for the AHRI TRE C core.

## Prerequisites

When installing from source, this package refreshes `AHRI_TRE.C` from the
configured remote by default, builds the C core with `cmake`, and stages the
shared library for runtime use.

Required on PATH:

- `git`
- `cmake`

Optional installer overrides:

```r
# Use a local C repo instead of auto-downloading
Sys.setenv(AHRI_TRE_C_ROOT = "C:/path/to/AHRI_TRE.C")

# Override download source/ref
Sys.setenv(AHRI_TRE_C_GIT_URL = "https://github.com/AHRIORG/AHRI_TRE.C.git")
Sys.setenv(AHRI_TRE_C_REF = "main")

# Opt out of remote refresh and use a local checkout as-is
Sys.setenv(AHRI_TRE_C_PULL_LATEST = "0")
```

## Install

From this package root directory:

```r
install.packages(".", repos = NULL, type = "source")
```

## Usage

```r
library(AHRITREC)
tre_load()
version()
```
