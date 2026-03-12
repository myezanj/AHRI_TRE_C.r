# AHRITREC (R)

R package bindings for the AHRI TRE C core.

This package is designed to call the C core from the AHRI_TRE.C project.

## C ABI symbol policy

The R bridge now uses Julia-style unprefixed C symbols as the primary binding targets (for example: `version`, `last_error`, `map_value_type`).

Prefixed symbols (for example: `ahri_tre_version`, `ahri_tre_last_error`) are still exported by the C core as compatibility aliases and remain safe for existing code.

## Build C core first

```bash
cmake -S c_core -B c_core/build
cmake --build c_core/build --config Release
```

If AHRI_TRE.C is a sibling repository, set:

```r
Sys.setenv(AHRI_TRE_C_ROOT = "C:/path/to/AHRI_TRE.C")
```

## Install package

```r
install.packages("wrappers/R", repos = NULL, type = "source")
library(AHRITREC)
ahri_tre_load()
ahri_tre_version()
```

Set `AHRI_TRE_C_LIB` to point directly to the C core shared library if needed.
