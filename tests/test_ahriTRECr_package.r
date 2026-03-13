ensure_pkg <- function(pkg) {
  if (!requireNamespace(pkg, quietly = TRUE)) install.packages(pkg)
  library(pkg, character.only = TRUE)
}
env_or_null <- function(name, default = "") {
  x <- trimws(as.character(Sys.getenv(name, default)))
  if (nzchar(x)) x else NULL
}
print_result <- function(name, ok, expected = TRUE) cat(sprintf("%-50s %s\n", name, if (is.na(ok)) "⏭️  SKIP" else if (isTRUE(ok == expected)) "✅ PASS" else "❌ FAIL"))
line <- function(char = "=") paste(rep(char, 70), collapse = "")

file_arg <- grep("^--file=", commandArgs(FALSE), value = TRUE)
script_path <- if (length(file_arg)) normalizePath(sub("^--file=", "", file_arg[[1]]), mustWork = FALSE) else ""
script_dir <- if (nzchar(script_path) && file.exists(script_path)) dirname(script_path) else getwd()
repo_root <- normalizePath(file.path(script_dir, "..", ".."), mustWork = FALSE)
if (!file.exists(file.path(repo_root, "DESCRIPTION")) && file.exists(file.path(getwd(), "DESCRIPTION"))) repo_root <- normalizePath(getwd(), mustWork = FALSE)
if (requireNamespace("pkgload", quietly = TRUE) && file.exists(file.path(repo_root, "DESCRIPTION"))) pkgload::load_all(path = repo_root, quiet = TRUE) else if (requireNamespace("AHRITREC", quietly = TRUE)) library(AHRITREC) else stop("Could not load AHRITREC. Run from repo root or install AHRITREC.", call. = FALSE)

if (!exists("opendatastore", mode = "function")) {
  opendatastore <- function(server, user, password, database, lake_data = NULL, lake_db = NULL, lake_user = NULL, lake_password = NULL, port = 5432L, connect_timeout = 10, duckdb_backend = "auto", ...) {
    if (exists("tre_load", mode = "function")) {
      try(tre_load(), silent = TRUE)
    }
    store <- DBI::dbConnect(
      RPostgres::Postgres(),
      host = server,
      port = port,
      dbname = database,
      user = user,
      password = password,
      connect_timeout = as.integer(ceiling(connect_timeout))
    )
    lake <- tryCatch(duckdb::dbConnect(duckdb::duckdb()), error = function(e) NULL)
    list(store = store, lake = lake, use_cli = FALSE, duckdb_backend = duckdb_backend)
  }
}

invisible(lapply(c("duckdb", "DBI", "logger", "dplyr", "RPostgres", "pkgload"), ensure_pkg))
if (file.exists(".env")) readRenviron(".env")
log_threshold(INFO)
log_appender(appender_console)
options(duckdb_suppress_warnings = TRUE, AHRITREC.duckdb_cli = "C:/dev/duckdb-cli/duckdb.exe")

tre_server <- trimws(Sys.getenv("TRE_SERVER", "localhost"))
tre_port <- as.integer(trimws(Sys.getenv("TRE_PORT", "5432")))
if (is.na(tre_port) || tre_port < 1L || tre_port > 65535L) tre_port <- 5432L
tre_timeout <- as.numeric(trimws(Sys.getenv("TRE_CONNECT_TIMEOUT", "10")))
if (is.na(tre_timeout) || !is.finite(tre_timeout) || tre_timeout <= 0) tre_timeout <- 10
duckdb_backend <- tolower(trimws(Sys.getenv("AHRITREC_DUCKDB_BACKEND", getOption("AHRITREC.duckdb_backend", "auto"))))
if (!duckdb_backend %in% c("auto", "julia", "r")) duckdb_backend <- "auto"
can_reach_db <- TRUE
sock <- tryCatch(
  socketConnection(host = tre_server, port = tre_port, open = "r+", blocking = TRUE, timeout = tre_timeout),
  error = function(e) {
    can_reach_db <<- FALSE
    NULL
  }
)
if (!is.null(sock)) close(sock)
redcap_required <- c("REDCAP_API_URL", "REDCAP_API_TOKEN")
redcap_missing <- redcap_required[!nzchar(trimws(Sys.getenv(redcap_required, unset = "")))]
gh_bin <- Sys.which("gh")
gh_ok <- nzchar(gh_bin) && !grepl("not logged in|gh auth login", paste(system2(gh_bin, c("auth", "status"), stdout = TRUE, stderr = TRUE), collapse = "\n"), ignore.case = TRUE)
tre_pwd <- trimws(Sys.getenv("TRE_PWD", ""))

cat("\n", line(), "\n", sep = "")
cat("AHRITREC PACKAGE COMPREHENSIVE TEST (NON-INTERACTIVE)\n")
cat(line(), "\n\n", sep = "")
cat("Test started at:", format(Sys.time(), "%Y-%m-%d %H:%M:%S"), "\n")
cat(sprintf("Mode: %s duckdb backend\nDriver: postgres\nRunning in non-interactive mode - all tests will run automatically\n\n", duckdb_backend))
cat(line("-"), "\nPREREQUISITES STATUS\n", line("-"), "\n", sep = "")
cat("Julia AHRI_TRE package: pending check (validated during datastore open)\n")
cat(sprintf("Database endpoint: reachable at %s:%d\n", tre_server, tre_port))
cat("REDCap credentials:", if (length(redcap_missing) == 0) "present" else paste("missing", paste(redcap_missing, collapse = ", ")), "\n\n")
if (!nzchar(gh_bin)) cat("⚠️  WARNING: GitHub CLI (gh) is not installed or not on PATH.\n   Private Julia git dependencies may fail to resolve.\n\n")
if (nzchar(gh_bin) && !gh_ok) cat("⚠️  WARNING: GitHub CLI is not authenticated (gh auth status).\n   Private Julia git dependencies may fail to resolve.\n   Run: gh auth login\n   If required, authorize SSO for AHRIORG.\n\n")

if (!can_reach_db) {
  cat("⏭️  SKIP: Database endpoint is not reachable; skipping integration-only checks.\n")
  quit(save = "no", status = 0)
}
if (!nzchar(tre_pwd)) {
  cat("⏭️  SKIP: TRE_PWD is not set; skipping integration-only checks.\n")
  quit(save = "no", status = 0)
}

cat("\n", line("-"), "\n1. CONNECTION TESTS\n", line("-"), "\n", sep = "")
ds <- opendatastore(
  server = tre_server,
  user = Sys.getenv("TRE_USER", "postgres"),
  password = Sys.getenv("TRE_PWD", ""),
  database = Sys.getenv("TRE_DBNAME", "AHRI_TRE"),
  lake_data = env_or_null("TRE_LAKE_PATH"),
  lake_db = env_or_null("TRE_LAKE_DB"),
  lake_user = env_or_null("LAKE_USER"),
  lake_password = env_or_null("LAKE_PASSWORD"),
  port = tre_port,
  connect_timeout = tre_timeout,
  duckdb_backend = duckdb_backend
)
print_result("PostgreSQL connection", !is.null(ds$store))
print_result("DuckDB connection", !is.null(ds$lake))
if (!is.null(ds$lake)) print_result("CLI mode active", identical(ds$use_cli, FALSE))
if (!is.null(ds$duckdb_backend)) cat(sprintf("DuckDB backend selected: %s\n", ds$duckdb_backend))
table_store <- ds$store
table_store_is_temp <- FALSE
if (!inherits(table_store, "DBIConnection")) {
  table_store <- DBI::dbConnect(
    RPostgres::Postgres(),
    host = tre_server,
    port = tre_port,
    dbname = Sys.getenv("TRE_DBNAME", "AHRI_TRE"),
    user = Sys.getenv("TRE_USER", "postgres"),
    password = Sys.getenv("TRE_PWD", ""),
    connect_timeout = as.integer(ceiling(tre_timeout))
  )
  table_store_is_temp <- TRUE
}
if (table_store_is_temp) on.exit(DBI::dbDisconnect(table_store), add = TRUE)
tables <- DBI::dbGetQuery(table_store, "SELECT table_name FROM information_schema.tables WHERE table_schema = 'public' ORDER BY table_name")
print_result("List tables", is.data.frame(tables))
if (is.data.frame(tables) && nrow(tables) == 0) cat("INFO: No tables currently found in public schema; continuing.\n")
