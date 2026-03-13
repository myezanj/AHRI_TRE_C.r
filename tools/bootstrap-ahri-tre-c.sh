#!/bin/sh
set -eu

PKG_DIR=$(cd "$(dirname "$0")/.." && pwd)
STAGE_DIR="$PKG_DIR/inst/tre_core"

if [ "${AHRI_TRE_C_SKIP_BOOTSTRAP:-0}" = "1" ]; then
  echo "Skipping AHRI_TRE.C bootstrap because AHRI_TRE_C_SKIP_BOOTSTRAP=1"
  exit 0
fi

mkdir -p "$STAGE_DIR"
rm -f \
  "$STAGE_DIR/tre_c.dll" \
  "$STAGE_DIR/libtre_c.dll" \
  "$STAGE_DIR/libtre_c.so" \
  "$STAGE_DIR/libtre_c.dylib" \
  "$STAGE_DIR/ahri_tre_c.dll" \
  "$STAGE_DIR/libahri_tre_c.dll" \
  "$STAGE_DIR/libahri_tre_c.so" \
  "$STAGE_DIR/libahri_tre_c.dylib"

if [ -n "${AHRI_TRE_C_ROOT:-}" ]; then
  SRC_ROOT="$AHRI_TRE_C_ROOT"
  echo "Using AHRI_TRE.C from AHRI_TRE_C_ROOT: $SRC_ROOT"
else
  # Prefer a sibling checkout when available.
  for local_candidate in "$PKG_DIR/../AHRI_TRE.C" "$PKG_DIR/../AHRI_TRE.c"; do
    if [ -d "$local_candidate/c_core" ]; then
      SRC_ROOT="$local_candidate"
      echo "Using sibling AHRI_TRE.C checkout: $SRC_ROOT"
      break
    fi
  done

  if [ -z "${SRC_ROOT:-}" ]; then
    if ! command -v git >/dev/null 2>&1; then
      echo "ERROR: git is required to download AHRI_TRE.C during package install." >&2
      exit 1
    fi

    REF="${AHRI_TRE_C_REF:-main}"
    WORK_DIR="$PKG_DIR/tools/.ahri_tre_c_work"
    SRC_ROOT="$WORK_DIR/AHRI_TRE.C"
    mkdir -p "$WORK_DIR"

    if [ -d "$SRC_ROOT/.git" ]; then
      echo "Updating existing AHRI_TRE.C checkout at $SRC_ROOT"
      git -C "$SRC_ROOT" fetch --depth 1 origin "$REF"
      git -C "$SRC_ROOT" checkout --detach FETCH_HEAD
    else
      CLONE_URLS="${AHRI_TRE_C_GIT_URL:-https://github.com/AHRIORG/AHRI_TRE.C.git git@github.com:AHRIORG/AHRI_TRE.C.git}"
      CLONED=0
      for u in $CLONE_URLS; do
        echo "Trying to clone AHRI_TRE.C from $u (ref: $REF)"
        if git clone --depth 1 --branch "$REF" "$u" "$SRC_ROOT"; then
          CLONED=1
          break
        fi
      done
      if [ "$CLONED" -ne 1 ]; then
        echo "ERROR: Could not clone AHRI_TRE.C. Set AHRI_TRE_C_ROOT or AHRI_TRE_C_GIT_URL." >&2
        exit 1
      fi
    fi
  fi
fi

if [ ! -d "$SRC_ROOT/c_core" ]; then
  echo "ERROR: Could not find c_core under $SRC_ROOT" >&2
  exit 1
fi

if ! command -v cmake >/dev/null 2>&1; then
  echo "ERROR: cmake is required to compile AHRI_TRE.C during package install." >&2
  exit 1
fi

BUILD_DIR="${AHRI_TRE_C_BUILD_DIR:-$SRC_ROOT/c_core/build}"

echo "Configuring AHRI_TRE.C core in $BUILD_DIR"
cmake -S "$SRC_ROOT/c_core" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release

echo "Building AHRI_TRE.C core"
cmake --build "$BUILD_DIR" --config Release

FOUND_LIB=""
for candidate in \
  "$BUILD_DIR/Release/tre_c.dll" \
  "$BUILD_DIR/tre_c.dll" \
  "$BUILD_DIR/Release/libtre_c.dll" \
  "$BUILD_DIR/libtre_c.dll" \
  "$BUILD_DIR/Release/ahri_tre_c.dll" \
  "$BUILD_DIR/ahri_tre_c.dll" \
  "$BUILD_DIR/Release/libahri_tre_c.dll" \
  "$BUILD_DIR/libahri_tre_c.dll" \
  "$BUILD_DIR/libtre_c.so" \
  "$BUILD_DIR/Release/libtre_c.so" \
  "$BUILD_DIR/libahri_tre_c.so" \
  "$BUILD_DIR/Release/libahri_tre_c.so" \
  "$BUILD_DIR/libtre_c.dylib" \
  "$BUILD_DIR/Release/libtre_c.dylib" \
  "$BUILD_DIR/libahri_tre_c.dylib" \
  "$BUILD_DIR/Release/libahri_tre_c.dylib"
do
  if [ -f "$candidate" ]; then
    FOUND_LIB="$candidate"
    break
  fi
done

if [ -z "$FOUND_LIB" ]; then
  echo "ERROR: Could not find built AHRI_TRE.C shared library under $BUILD_DIR" >&2
  exit 1
fi

BASENAME=$(basename "$FOUND_LIB")
cp "$FOUND_LIB" "$STAGE_DIR/$BASENAME"

if [ "$BASENAME" = "libahri_tre_c.dll" ]; then
  cp "$FOUND_LIB" "$STAGE_DIR/ahri_tre_c.dll"
fi
if [ "$BASENAME" = "ahri_tre_c.dll" ]; then
  cp "$FOUND_LIB" "$STAGE_DIR/libahri_tre_c.dll"
fi
if [ "$BASENAME" = "libtre_c.dll" ]; then
  cp "$FOUND_LIB" "$STAGE_DIR/tre_c.dll"
fi
if [ "$BASENAME" = "tre_c.dll" ]; then
  cp "$FOUND_LIB" "$STAGE_DIR/libtre_c.dll"
fi

echo "Staged AHRI_TRE.C shared library: $STAGE_DIR/$BASENAME"
