# Backward-compatible script helpers.
ahri_tre_load <- function(core_path = Sys.getenv("AHRI_TRE_C_LIB", unset = "")) {
	if (!nzchar(core_path)) {
		root <- normalizePath(file.path("..", ".."), mustWork = FALSE)
		if (.Platform$OS.type == "windows") {
			candidates <- c(
				file.path(root, "c_core", "build", "Release", "ahri_tre_c.dll"),
				file.path(root, "c_core", "build", "ahri_tre_c.dll")
			)
		} else if (Sys.info()[["sysname"]] == "Darwin") {
			candidates <- c(file.path(root, "c_core", "build", "libahri_tre_c.dylib"))
		} else {
			candidates <- c(file.path(root, "c_core", "build", "libahri_tre_c.so"))
		}
		found <- candidates[file.exists(candidates)]
		if (length(found) > 0) {
			core_path <- found[[1]]
		}
	}

	if (!nzchar(core_path) || !file.exists(core_path)) {
		stop("Could not locate AHRI TRE C shared library. Set AHRI_TRE_C_LIB.")
	}

	dyn.load(core_path)
	invisible(core_path)
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
