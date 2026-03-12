ahri_tre_load <- function(core_path = Sys.getenv("AHRI_TRE_C_LIB", unset = "")) {
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
      sibling_paths <- file.path(parent, sibling_names)
      roots <- c(roots, normalizePath(sibling_paths, mustWork = FALSE))
      if (parent == anchor) {
        break
      }
      anchor <- parent
    }

    roots <- unique(roots)
    candidates <- character(0)
    for (root in roots) {
      if (.Platform$OS.type == "windows") {
        candidates <- c(
          candidates,
          file.path(root, "c_core", "build", "Release", "ahri_tre_c.dll"),
          file.path(root, "c_core", "build", "ahri_tre_c.dll")
        )
      } else if (Sys.info()[["sysname"]] == "Darwin") {
        candidates <- c(
          candidates,
          file.path(root, "c_core", "build", "libahri_tre_c.dylib"),
          file.path(root, "c_core", "build", "Release", "libahri_tre_c.dylib")
        )
      } else {
        candidates <- c(
          candidates,
          file.path(root, "c_core", "build", "libahri_tre_c.so"),
          file.path(root, "c_core", "build", "Release", "libahri_tre_c.so")
        )
      }
    }

    found <- candidates[file.exists(candidates)]
    if (length(found) > 0) {
      core_path <- found[[1]]
    }
  }

  if (nzchar(core_path) && file.exists(core_path)) {
    dyn.load(core_path)
    return(invisible(core_path))
  }

  stop("Could not locate AHRI TRE C shared library. Set AHRI_TRE_C_LIB.")
}

ahri_tre_version <- function() {
  .Call("ahri_tre_version_R")
}

ahri_tre_sha256_file_hex <- function(path) {
  .Call("ahri_tre_sha256_file_hex_R", as.character(path))
}

ahri_tre_verify_sha256_file <- function(path, expected_hex) {
  .Call("ahri_tre_verify_sha256_file_R", as.character(path), as.character(expected_hex))
}

ahri_tre_parse_flavour <- function(flavour) {
  .Call("ahri_tre_parse_flavour_R", as.character(flavour))
}

ahri_tre_map_sql_type_to_tre <- function(sql_type) {
  .Call("ahri_tre_map_sql_type_to_tre_R", as.character(sql_type))
}

ahri_tre_extract_table_from_sql <- function(sql) {
  .Call("ahri_tre_extract_table_from_sql_R", as.character(sql))
}

ahri_tre_parse_in_list_values_json <- function(values) {
  .Call("ahri_tre_parse_in_list_values_json_R", as.character(values))
}

parse_in_list_values <- function(values) {
  ahri_tre_parse_in_list_values_json(values)
}

ahri_tre_parse_check_constraint_values_json <- function(constraint_def, column_name) {
  .Call("ahri_tre_parse_check_constraint_values_json_R", as.character(constraint_def), as.character(column_name))
}

parse_check_constraint_values <- function(constraint_def, column_name) {
  ahri_tre_parse_check_constraint_values_json(constraint_def, column_name)
}

ahri_tre_map_redcap_value_type <- function(field_type, validation = NULL) {
  if (is.null(validation)) {
    .Call("ahri_tre_map_redcap_value_type_R", as.character(field_type), NULL)
  } else {
    .Call("ahri_tre_map_redcap_value_type_R", as.character(field_type), as.character(validation))
  }
}

map_value_type <- function(field_type, validation = NULL) {
  ahri_tre_map_redcap_value_type(field_type, validation)
}

ahri_tre_parse_redcap_choices_json <- function(choices) {
  .Call("ahri_tre_parse_redcap_choices_json_R", as.character(choices))
}

parse_redcap_choices <- function(choices) {
  ahri_tre_parse_redcap_choices_json(choices)
}

ahri_tre_strip_html <- function(text) {
  .Call("ahri_tre_strip_html_R", as.character(text))
}

ahri_tre_infer_label_from_field_name <- function(field_name) {
  .Call("ahri_tre_infer_label_from_field_name_R", as.character(field_name))
}

ahri_tre_get_redcap_choices_for_field_json <- function(field_type, choices = NULL) {
  if (is.null(choices)) {
    .Call("ahri_tre_get_redcap_choices_for_field_json_R", as.character(field_type), NULL)
  } else {
    .Call("ahri_tre_get_redcap_choices_for_field_json_R", as.character(field_type), as.character(choices))
  }
}

get_redcap_choices_for_field <- function(field_type, choices = NULL) {
  ahri_tre_get_redcap_choices_for_field_json(field_type, choices)
}
