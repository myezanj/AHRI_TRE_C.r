# build_release.R

# 1. Create the directory if it's missing
if (!dir.exists("release")) dir.create("release")

ensure_latest_ahri_tre_c <- function() {
  bootstrap <- file.path("tools", "bootstrap-ahri-tre-c.sh")
  if (!file.exists(bootstrap)) {
    stop("Missing bootstrap script: ", bootstrap)
  }

  sh_bin <- Sys.which("sh")
  if (!nzchar(sh_bin)) {
    stop("Could not find 'sh' on PATH. Install Rtools/MSYS so bootstrap can run.")
  }

  old <- Sys.getenv(c("AHRI_TRE_C_GIT_URL", "AHRI_TRE_C_PULL_LATEST", "AHRI_TRE_C_REF"), unset = NA_character_)
  on.exit({
    for (nm in names(old)) {
      val <- old[[nm]]
      if (is.na(val)) {
        Sys.unsetenv(nm)
      } else {
        Sys.setenv(structure(val, names = nm))
      }
    }
  }, add = TRUE)

  Sys.setenv(
    AHRI_TRE_C_GIT_URL = "https://github.com/myezanj/AHRI_TRE.c.git",
    AHRI_TRE_C_PULL_LATEST = "1",
    AHRI_TRE_C_REF = "main"
  )

  message("Syncing latest AHRI_TRE.c from https://github.com/myezanj/AHRI_TRE.c.git")
  rc <- system2(sh_bin, bootstrap)
  if (!identical(rc, 0L)) {
    stop("Failed to bootstrap latest AHRI_TRE.c (exit code ", rc, ").")
  }
}

run_without_quarto_noise <- function(expr) {
  withCallingHandlers(
    expr,
    warning = function(w) {
      msg <- conditionMessage(w)
      if (grepl('system2("quarto", "-V"', msg, fixed = TRUE) ||
        grepl('quarto" TMPDIR=.* -V.*status 1', msg, ignore.case = TRUE) ||
        grepl('Unknown command "TMPDIR=', msg, ignore.case = TRUE)) {
        invokeRestart("muffleWarning")
      }
    }
  )
}

# 2. Update documentation
ensure_latest_ahri_tre_c()
devtools::document()
devtools::document(roclets = c("rd", "collate", "namespace"))
run_without_quarto_noise(devtools::check(quiet = TRUE))

# 3. Build Source Package (.tar.gz)
tarball_path <- devtools::build(path = "release", binary = FALSE)

# 4. Build Binary Package (.zip or .tgz)
# Use base R CMD INSTALL --build to avoid Windows Quarto CLI TMPDIR warning path.
rcmd <- file.path(R.home("bin"), "R")
tarball_for_install <- shQuote(normalizePath(tarball_path, winslash = "/", mustWork = TRUE))
temp_lib <- tempfile("ahritrec-build-lib-")
dir.create(temp_lib, recursive = TRUE, showWarnings = FALSE)
system2(rcmd, c("CMD", "INSTALL", "-l", shQuote(temp_lib), "--build", tarball_for_install))

message("Builds complete! Check the ./release folder.")
