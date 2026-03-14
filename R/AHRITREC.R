.tre_sysname <- function() {
  Sys.info()[["sysname"]]
}

.tre_lib_names <- function() {
  if (.Platform$OS.type == "windows") {
    return(c("tre_c.dll", "libtre_c.dll", "ahri_tre_c.dll", "libahri_tre_c.dll"))
  }
  if (.tre_sysname() == "Darwin") {
    return(c("libtre_c.dylib", "libahri_tre_c.dylib"))
  }
  c("libtre_c.so", "libahri_tre_c.so")
}

.tre_build_candidates <- function(root) {
  lib_names <- .tre_lib_names()
  c(
    file.path(root, "c_core", "build", lib_names),
    file.path(root, "c_core", "build", "Release", lib_names)
  )
}

.tre_state <- new.env(parent = emptyenv())
.tre_state$core_loaded <- FALSE

.tre_ensure_loaded <- function(symbol = NULL) {
  if (!isTRUE(.tre_state$core_loaded)) {
    tre_load()
  }
}

.tre_call_chr1 <- function(symbol, arg1) {
  .tre_ensure_loaded(symbol)
  .Call(symbol, as.character(arg1))
}

.tre_call_chr2 <- function(symbol, arg1, arg2) {
  .tre_ensure_loaded(symbol)
  .Call(symbol, as.character(arg1), as.character(arg2))
}

.tre_call_chr_nullable2 <- function(symbol, arg1, arg2 = NULL) {
  .tre_ensure_loaded(symbol)
  if (is.null(arg2)) {
    .Call(symbol, as.character(arg1), NULL)
  } else {
    .Call(symbol, as.character(arg1), as.character(arg2))
  }
}

#' Load the AHRI TRE C shared library
#'
#' Locates and loads the compiled `AHRI_TRE.c` shared library for the current
#' R session.
#'
#' Lookup order is:
#' 1. `core_path`, if provided.
#' 2. The staged library bundled with the installed package.
#' 3. Candidate build outputs discovered from `AHRI_TRE_C_ROOT` and nearby
#'    sibling checkouts.
#'
#' @param core_path Optional path to a compiled AHRI TRE shared library.
#'
#' @returns Invisibly returns the loaded library path.
tre_load <- function(core_path = Sys.getenv("AHRI_TRE_C_LIB", unset = "")) {
  if (!nzchar(core_path)) {
    staged_core_dir <- system.file("tre_core", package = "AHRITREC")
    if (nzchar(staged_core_dir)) {
      staged_candidates <- file.path(staged_core_dir, .tre_lib_names())
      staged_found <- staged_candidates[file.exists(staged_candidates)]
      if (length(staged_found) > 0) {
        core_path <- staged_found[[1]]
      }
    }
  }

  if (!nzchar(core_path)) {
    root_env <- Sys.getenv("AHRI_TRE_C_ROOT", unset = "")
    roots <- character(0)
    if (nzchar(root_env)) {
      roots <- c(roots, normalizePath(root_env, mustWork = FALSE))
    }

    anchor <- normalizePath(file.path(system.file(package = "AHRITREC"), "..", "..", ".."), mustWork = FALSE)
    sibling_names <- c("AHRI_TRE.C", "AHRI_TRE.c", "AHRI_TRE.jl")
    repeat {
      parent <- dirname(anchor)
      roots <- c(roots, normalizePath(file.path(parent, sibling_names), mustWork = FALSE))
      if (parent == anchor) {
        break
      }
      anchor <- parent
    }

    roots <- unique(roots)
    candidates <- unlist(lapply(roots, .tre_build_candidates), use.names = FALSE)
    found <- candidates[file.exists(candidates)]
    if (length(found) > 0) {
      core_path <- found[[1]]
    }
  }

  if (nzchar(core_path) && file.exists(core_path)) {
    dyn.load(core_path, local = FALSE)
    .tre_state$core_loaded <- TRUE
    return(invisible(core_path))
  }

  stop("Could not locate AHRI TRE C shared library. Reinstall package or set AHRI_TRE_C_LIB.")
}

#' Return the core library version
#'
#' Calls into the AHRI TRE C core and returns its version string.
#'
#' @returns A single character string containing the library version.
version <- function() {
  .tre_ensure_loaded("version_R")
  .Call("version_R")
}

#' Compute a SHA-256 digest for a file
#'
#' Produces the hexadecimal SHA-256 digest for `path`.
#'
#' @param path Path to the file to hash.
#'
#' @returns A lowercase hexadecimal SHA-256 digest string.
sha256_file_hex <- function(path) .tre_call_chr1("sha256_file_hex_R", path)

#' Verify a file SHA-256 digest
#'
#' Compares the SHA-256 digest of `path` against `expected_hex`.
#'
#' @param path Path to the file to verify.
#' @param expected_hex Expected SHA-256 digest in hexadecimal form.
#'
#' @returns A logical scalar indicating whether the digest matches.
verify_sha256_file <- function(path, expected_hex) {
  .tre_call_chr2("verify_sha256_file_R", path, expected_hex)
}

#' Parse a database flavour identifier
#'
#' Normalises a database flavour string such as `"MSSQL"` or `"DuckDB"` into
#' the canonical value used by the C core.
#'
#' @param flavour Database flavour string.
#'
#' @returns A canonical flavour string.
parse_flavour <- function(flavour) .tre_call_chr1("parse_flavour_R", flavour)

#' Map a SQL type to a TRE type
#'
#' Converts a SQL column type into the TRE type name used by AHRI TRE.
#'
#' @param sql_type SQL type text to map.
#'
#' @returns A TRE type string.
map_sql_type_to_tre <- function(sql_type) .tre_call_chr1("map_sql_type_to_tre_R", sql_type)

#' Extract the table name from SQL
#'
#' Parses a SQL statement and extracts the table reference identified by the
#' C core parser.
#'
#' @param sql SQL statement text.
#'
#' @returns A character value describing the extracted table.
extract_table_from_sql <- function(sql) .tre_call_chr1("extract_table_from_sql_R", sql)

#' Parse `IN (...)` values as JSON
#'
#' Parses a string containing SQL-style `IN (...)` values and returns the parsed
#' result as JSON text.
#'
#' @param values Text containing values to parse.
#'
#' @returns A JSON string describing the parsed values.
parse_in_list_values_json <- function(values) .tre_call_chr1("parse_in_list_values_json_R", values)

#' Parse `IN (...)` values
#'
#' Convenience wrapper around [parse_in_list_values_json()] that returns the
#' same JSON payload.
#'
#' @inheritParams parse_in_list_values_json
#'
#' @returns A JSON string describing the parsed values.
parse_in_list_values <- function(values) parse_in_list_values_json(values)

#' Parse check constraint values as JSON
#'
#' Extracts allowable values from a SQL `CHECK` constraint definition for a
#' specific column.
#'
#' @param constraint_def Constraint definition text.
#' @param column_name Column name referenced by the constraint.
#'
#' @returns A JSON string describing the parsed constraint values.
parse_check_constraint_values_json <- function(constraint_def, column_name) {
  .tre_call_chr2("parse_check_constraint_values_json_R", constraint_def, column_name)
}

#' Parse check constraint values
#'
#' Convenience wrapper around [parse_check_constraint_values_json()] that
#' returns the same JSON payload.
#'
#' @inheritParams parse_check_constraint_values_json
#'
#' @returns A JSON string describing the parsed constraint values.
parse_check_constraint_values <- function(constraint_def, column_name) {
  parse_check_constraint_values_json(constraint_def, column_name)
}

#' Map a REDCap field type to a value type
#'
#' Determines the value type for a REDCap field using its field type and,
#' optionally, a validation rule.
#'
#' @param field_type REDCap field type.
#' @param validation Optional REDCap validation rule.
#'
#' @returns A character value describing the mapped value type.
map_value_type <- function(field_type, validation = NULL) {
  .tre_call_chr_nullable2("map_value_type_R", field_type, validation)
}

#' Parse REDCap choices as JSON
#'
#' Parses REDCap `choices, calculations, OR slider labels` text into JSON.
#'
#' @param choices REDCap choices string.
#'
#' @returns A JSON string describing the parsed choices.
parse_redcap_choices_json <- function(choices) .tre_call_chr1("parse_redcap_choices_json_R", choices)

#' Parse REDCap choices
#'
#' Convenience wrapper around [parse_redcap_choices_json()] that returns the
#' same JSON payload.
#'
#' @inheritParams parse_redcap_choices_json
#'
#' @returns A JSON string describing the parsed choices.
parse_redcap_choices <- function(choices) parse_redcap_choices_json(choices)

#' Strip HTML from text
#'
#' Removes HTML tags and decodes supported HTML entities from a text value.
#'
#' @param text Character text that may contain HTML.
#'
#' @returns A cleaned character string.
strip_html <- function(text) .tre_call_chr1("strip_html_R", text)

#' Infer a human-readable label from a field name
#'
#' Converts a machine-oriented field name into a more readable label.
#'
#' @param field_name Field name to transform.
#'
#' @returns A human-readable label string.
infer_label_from_field_name <- function(field_name) .tre_call_chr1("infer_label_from_field_name_R", field_name)

#' Get REDCap field choices as JSON
#'
#' Returns the default or parsed REDCap choices for a field type as JSON.
#'
#' @param field_type REDCap field type.
#' @param choices Optional REDCap choices string.
#'
#' @returns A JSON string describing the available field choices.
get_redcap_choices_for_field_json <- function(field_type, choices = NULL) {
  .tre_call_chr_nullable2("get_redcap_choices_for_field_json_R", field_type, choices)
}

#' Get REDCap field choices
#'
#' Convenience wrapper around [get_redcap_choices_for_field_json()] that
#' returns the same JSON payload.
#'
#' @inheritParams get_redcap_choices_for_field_json
#'
#' @returns A JSON string describing the available field choices.
get_redcap_choices_for_field <- function(field_type, choices = NULL) {
  get_redcap_choices_for_field_json(field_type, choices)
}
