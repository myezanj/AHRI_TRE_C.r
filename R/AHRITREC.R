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

.tre_call_chr1 <- function(symbol, arg1) {
  .Call(symbol, as.character(arg1))
}

.tre_call_chr2 <- function(symbol, arg1, arg2) {
  .Call(symbol, as.character(arg1), as.character(arg2))
}

.tre_call_chr_nullable2 <- function(symbol, arg1, arg2 = NULL) {
  if (is.null(arg2)) {
    .Call(symbol, as.character(arg1), NULL)
  } else {
    .Call(symbol, as.character(arg1), as.character(arg2))
  }
}

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
    return(invisible(core_path))
  }

  stop("Could not locate AHRI TRE C shared library. Reinstall package or set AHRI_TRE_C_LIB.")
}

version <- function() .Call("version_R")

sha256_file_hex <- function(path) .tre_call_chr1("sha256_file_hex_R", path)

verify_sha256_file <- function(path, expected_hex) {
  .tre_call_chr2("verify_sha256_file_R", path, expected_hex)
}

parse_flavour <- function(flavour) .tre_call_chr1("parse_flavour_R", flavour)

map_sql_type_to_tre <- function(sql_type) .tre_call_chr1("map_sql_type_to_tre_R", sql_type)

extract_table_from_sql <- function(sql) .tre_call_chr1("extract_table_from_sql_R", sql)

parse_in_list_values_json <- function(values) .tre_call_chr1("parse_in_list_values_json_R", values)
parse_in_list_values <- function(values) parse_in_list_values_json(values)

parse_check_constraint_values_json <- function(constraint_def, column_name) {
  .tre_call_chr2("parse_check_constraint_values_json_R", constraint_def, column_name)
}
parse_check_constraint_values <- function(constraint_def, column_name) {
  parse_check_constraint_values_json(constraint_def, column_name)
}

map_value_type <- function(field_type, validation = NULL) {
  .tre_call_chr_nullable2("map_value_type_R", field_type, validation)
}

parse_redcap_choices_json <- function(choices) .tre_call_chr1("parse_redcap_choices_json_R", choices)
parse_redcap_choices <- function(choices) parse_redcap_choices_json(choices)

strip_html <- function(text) .tre_call_chr1("strip_html_R", text)

infer_label_from_field_name <- function(field_name) .tre_call_chr1("infer_label_from_field_name_R", field_name)

get_redcap_choices_for_field_json <- function(field_type, choices = NULL) {
  .tre_call_chr_nullable2("get_redcap_choices_for_field_json_R", field_type, choices)
}
get_redcap_choices_for_field <- function(field_type, choices = NULL) {
  get_redcap_choices_for_field_json(field_type, choices)
}
