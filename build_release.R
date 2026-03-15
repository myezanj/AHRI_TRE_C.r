# build_release.R

# 1. Create the directory if it's missing
if (!dir.exists("release")) dir.create("release")

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
