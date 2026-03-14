#!/bin/sh
set -eu

PKG_DIR=$(cd "$(dirname "$0")/.." && pwd)
STAGE_DIR="$PKG_DIR/inst/tre_core"
REF="${AHRI_TRE_C_REF:-main}"
PULL_LATEST="${AHRI_TRE_C_PULL_LATEST:-1}"

if [ "${AHRI_TRE_C_SKIP_BOOTSTRAP:-0}" = "1" ]; then
  echo "Skipping AHRI_TRE.C bootstrap because AHRI_TRE_C_SKIP_BOOTSTRAP=1"
  exit 0
fi

resolve_origin_url() {
  repo_path="$1"

  if command -v git >/dev/null 2>&1 && [ -d "$repo_path/.git" ]; then
    git -C "$repo_path" config --get remote.origin.url 2>/dev/null || true
  fi
}

clone_latest_checkout() {
  checkout_dir="$1"
  clone_urls="$2"

  rm -rf "$checkout_dir"

  CLONED=0
  for u in $clone_urls; do
    echo "Trying to clone AHRI_TRE.C from $u (ref: $REF)"
    if git clone --depth 1 --branch "$REF" "$u" "$checkout_dir"; then
      CLONED=1
      break
    fi
  done

  if [ "$CLONED" -ne 1 ]; then
    echo "ERROR: Could not clone AHRI_TRE.C. Set AHRI_TRE_C_GIT_URL or disable refresh with AHRI_TRE_C_PULL_LATEST=0." >&2
    exit 1
  fi
}

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

WORK_DIR="${AHRI_TRE_C_WORK_DIR:-${TMPDIR:-${TEMP:-${TMP:-/tmp}}}/ahri_tre_c_work}"
SYNC_ROOT="$WORK_DIR/AHRI_TRE.C"
DEFAULT_CLONE_URLS="https://github.com/myezanj/AHRI_TRE.c.git git@github.com:myezanj/AHRI_TRE.c.git https://github.com/AHRIORG/AHRI_TRE.C.git git@github.com:AHRIORG/AHRI_TRE.C.git"

if [ "$PULL_LATEST" = "1" ]; then
  if ! command -v git >/dev/null 2>&1; then
    echo "ERROR: git is required to refresh AHRI_TRE.C during package install." >&2
    exit 1
  fi

  mkdir -p "$WORK_DIR"
  CLONE_URLS="${AHRI_TRE_C_GIT_URL:-}"

  if [ -n "${AHRI_TRE_C_ROOT:-}" ]; then
    ROOT_REMOTE=$(resolve_origin_url "$AHRI_TRE_C_ROOT")
    if [ -n "$ROOT_REMOTE" ]; then
      CLONE_URLS="$ROOT_REMOTE ${CLONE_URLS:-}"
    fi
  else
    for local_candidate in "$PKG_DIR/../AHRI_TRE.C" "$PKG_DIR/../AHRI_TRE.c"; do
      if [ -d "$local_candidate/c_core" ]; then
        CANDIDATE_REMOTE=$(resolve_origin_url "$local_candidate")
        if [ -n "$CANDIDATE_REMOTE" ]; then
          CLONE_URLS="$CANDIDATE_REMOTE ${CLONE_URLS:-}"
        fi
        break
      fi
    done
  fi

  CLONE_URLS="${CLONE_URLS:-$DEFAULT_CLONE_URLS} $DEFAULT_CLONE_URLS"
  clone_latest_checkout "$SYNC_ROOT" "$CLONE_URLS"
  SRC_ROOT="$SYNC_ROOT"
  echo "Using refreshed AHRI_TRE.C checkout: $SRC_ROOT"
elif [ -n "${AHRI_TRE_C_ROOT:-}" ]; then
  SRC_ROOT="$AHRI_TRE_C_ROOT"
  echo "Using AHRI_TRE.C from AHRI_TRE_C_ROOT without remote refresh: $SRC_ROOT"
else
  for local_candidate in "$PKG_DIR/../AHRI_TRE.C" "$PKG_DIR/../AHRI_TRE.c"; do
    if [ -d "$local_candidate/c_core" ]; then
      SRC_ROOT="$local_candidate"
      echo "Using sibling AHRI_TRE.C checkout without remote refresh: $SRC_ROOT"
      break
    fi
  done

  if [ -z "${SRC_ROOT:-}" ]; then
    echo "ERROR: AHRI_TRE_C_PULL_LATEST=0 requires AHRI_TRE_C_ROOT or a sibling AHRI_TRE.C checkout." >&2
    exit 1
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

BUILD_DIR="${AHRI_TRE_C_BUILD_DIR:-$STAGE_DIR/.build}"

# Recreate build dir to avoid stale CMakeCache.txt conflicts when source path changes
# (e.g., package checks in temporary directories).
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

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

rm -rf "$BUILD_DIR"
