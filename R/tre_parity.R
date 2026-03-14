#' Database Flavour Identifiers
#'
#' Numeric identifiers used by the AHRI TRE C core for database flavour-specific
#' helpers such as `map_sql_type_to_tre_for_flavour()`.
#'
#' @format An integer vector with named entries.
#' @export
DatabaseFlavour <- c(
  MSSQL = 1L,
  DUCKDB = 2L,
  POSTGRESQL = 3L,
  SQLITE = 4L,
  MYSQL = 5L
)

#' Convert a Path to a File URI
#'
#' @param path Local filesystem path.
#'
#' @return A file URI string.
#' @export
path_to_file_uri <- function(path) {
  .Call("path_to_file_uri_R", path)
}

#' Convert a File URI to a Path
#'
#' @param uri File URI string.
#'
#' @return A local filesystem path.
#' @export
file_uri_to_path <- function(uri) {
  .Call("file_uri_to_path_R", uri)
}

#' Empty a Directory
#'
#' @param path Directory path to empty.
#' @param create Whether to create the directory if it does not exist.
#' @param retries Number of retries for transient filesystem failures.
#' @param wait_millis Milliseconds to wait between retries.
#'
#' @return `TRUE` when the operation succeeds.
#' @export
emptydir <- function(path, create = FALSE, retries = 0L, wait_millis = 0L) {
  .Call("emptydir_R", path, create, retries, wait_millis)
}

#' Check XML NCName Validity
#'
#' @param value Candidate value.
#' @param strict Whether to apply strict validation rules.
#'
#' @return `TRUE` when `value` is a valid NCName.
#' @export
is_ncname <- function(value, strict = TRUE) {
  .Call("is_ncname_R", value, strict)
}

#' Normalize a Value to an NCName
#'
#' @param value Candidate value.
#' @param replacement Replacement text used for invalid characters.
#' @param prefix Prefix used if the name would otherwise start invalidly.
#' @param avoid_reserved Whether to rewrite reserved names.
#' @param strict Whether to apply strict NCName validation rules.
#'
#' @return A valid NCName string.
#' @export
to_ncname <- function(
  value,
  replacement = "_",
  prefix = "x",
  avoid_reserved = TRUE,
  strict = TRUE
) {
  .Call("to_ncname_R", value, replacement, prefix, avoid_reserved, strict)
}

#' Map SQL Type to TRE Type for a Flavour
#'
#' @param sql_type SQL type name.
#' @param flavour_id A value from `DatabaseFlavour`.
#'
#' @return Integer TRE type identifier.
#' @export
map_sql_type_to_tre_for_flavour <- function(sql_type, flavour_id) {
  .Call("map_sql_type_to_tre_for_flavour_R", sql_type, flavour_id)
}

#' Build a TRE Dataset Name
#'
#' @param study_name Study identifier.
#' @param asset_name Asset identifier.
#' @param major Major version number.
#' @param minor Minor version number.
#' @param patch Patch version number.
#' @param include_schema Whether to include schema qualification.
#'
#' @return Dataset name string.
#' @export
get_datasetname <- function(
  study_name,
  asset_name,
  major = 1L,
  minor = 0L,
  patch = 0L,
  include_schema = FALSE
) {
  .Call(
    "get_datasetname_R",
    study_name,
    asset_name,
    major,
    minor,
    patch,
    include_schema
  )
}

#' Build a TRE Data Filename
#'
#' @param asset_name Asset identifier.
#' @param major Major version number.
#' @param minor Minor version number.
#' @param patch Patch version number.
#'
#' @return Filename string.
#' @export
get_datafilename <- function(asset_name, major = 1L, minor = 0L, patch = 0L) {
  .Call("get_datafilename_R", asset_name, major, minor, patch)
}

#' Build a Datalake File Path
#'
#' @param lake_data Root lake data path.
#' @param study_name Study identifier.
#' @param asset_name Asset identifier.
#' @param source_file_path Source filename or path.
#' @param major Major version number.
#' @param minor Minor version number.
#' @param patch Patch version number.
#'
#' @return Datalake file path.
#' @export
get_datalake_file_path <- function(
  lake_data,
  study_name,
  asset_name,
  source_file_path,
  major = 1L,
  minor = 0L,
  patch = 0L
) {
  .Call(
    "get_datalake_file_path_R",
    lake_data,
    study_name,
    asset_name,
    source_file_path,
    major,
    minor,
    patch
  )
}

#' Prepare a Datafile Digest Payload
#'
#' @param file_path File path.
#' @param precomputed_digest Optional precomputed SHA-256 digest.
#'
#' @return JSON string payload.
#' @export
prepare_datafile_digest <- function(file_path, precomputed_digest = NULL) {
  .Call("prepare_datafile_digest_R", file_path, precomputed_digest)
}

#' Prepare Datafile Metadata JSON
#'
#' @param file_path File path.
#' @param edam_format EDAM format identifier.
#' @param compress Whether the file is compressed.
#' @param encrypt Whether the file is encrypted.
#' @param precomputed_digest Optional precomputed SHA-256 digest.
#'
#' @return JSON string payload.
#' @export
prepare_datafile_json <- function(
  file_path,
  edam_format,
  compress = FALSE,
  encrypt = FALSE,
  precomputed_digest = NULL
) {
  .Call(
    "prepare_datafile_json_R",
    file_path,
    edam_format,
    compress,
    encrypt,
    precomputed_digest
  )
}

#' Prepare Datafile Metadata
#'
#' Parity alias for the current C-core JSON helper.
#'
#' @inheritParams prepare_datafile_json
#'
#' @return JSON string payload.
#' @export
prepare_datafile <- prepare_datafile_json

#' Normalize an ORCID Role Name
#'
#' @param orcid ORCID or ORCID-based role string.
#'
#' @return Normalized role name.
#' @export
normalise_orcid_rolename <- function(orcid) {
  .Call("normalise_orcid_rolename_R", orcid)
}

#' Create Positional SQL Parameters as JSON
#'
#' @param n Number of parameters.
#'
#' @return JSON string representing placeholder parameters.
#' @export
makeparams_json <- function(n) {
  .Call("makeparams_json_R", n)
}

#' Create Positional SQL Parameters
#'
#' Parity alias for `makeparams_json()`.
#'
#' @inheritParams makeparams_json
#'
#' @return JSON string representing placeholder parameters.
#' @export
makeparams <- makeparams_json

#' Quote a SQL Identifier
#'
#' @param name Identifier to quote.
#'
#' @return Quoted identifier.
#' @export
quote_ident <- function(name) {
  .Call("quote_ident_R", name)
}

#' Quote a Qualified SQL Identifier
#'
#' @param name Qualified identifier to quote.
#'
#' @return Quoted identifier.
#' @export
quote_qualified_identifier <- function(name) {
  .Call("quote_qualified_identifier_R", name)
}

#' Quote a SQL String Literal
#'
#' @param value String literal to quote.
#'
#' @return Quoted SQL string literal.
#' @export
quote_sql_str <- function(value) {
  .Call("quote_sql_str_R", value)
}

#' Quote an Identifier
#'
#' Parity alias for `quote_ident()`.
#'
#' @inheritParams quote_ident
#'
#' @return Quoted identifier.
#' @export
quote_identifier <- quote_ident

#' Map a Julia Type Name to SQL
#'
#' @param julia_type Julia type name.
#'
#' @return SQL type string.
#' @export
julia_type_to_sql_string <- function(julia_type) {
  .Call("julia_type_to_sql_string_R", julia_type)
}

#' Map a TRE Type to DuckDB SQL
#'
#' @param value_type_id TRE type identifier.
#'
#' @return DuckDB SQL type string.
#' @export
tre_type_to_duckdb_sql <- function(value_type_id) {
  .Call("tre_type_to_duckdb_sql_R", value_type_id)
}

#' Normalize a Git Remote URL
#'
#' @param url Remote URL.
#'
#' @return Normalized remote string.
#' @export
normalize_git_remote <- function(url) {
  .Call("normalize_git_remote_R", url)
}

#' Canonicalize a Filesystem Path
#'
#' @param path Filesystem path.
#'
#' @return Canonicalized path.
#' @export
canonical_path <- function(path) {
  .Call("canonical_path_R", path)
}

#' Get Git Commit Metadata as JSON
#'
#' @param dir Directory inside the git working tree.
#' @param short_hash Whether to request a short commit hash.
#' @param script_path Optional script path to include in the response.
#'
#' @return JSON string with commit metadata.
#' @export
git_commit_info_json <- function(dir = ".", short_hash = FALSE, script_path = NULL) {
  .Call("git_commit_info_json_R", dir, short_hash, script_path)
}

#' Get the Runtime Caller File
#'
#' @param hint_path Optional hint path.
#'
#' @return Caller file path.
#' @export
caller_file_runtime <- function(hint_path = NULL) {
  .Call("caller_file_runtime_R", hint_path)
}

# These aliases preserve older Julia-facing function names for the helpers that
# now live under C-core-oriented names in the R package.

#' @rdname normalize_git_remote
#' @export
`_normalize_remote` <- normalize_git_remote

#' @rdname strip_html
#' @export
`_strip_html` <- strip_html

#' @rdname sha256_file_hex
#' @export
sha256_digest_hex <- sha256_file_hex

#' @rdname verify_sha256_file
#' @export
verify_sha256_digest <- verify_sha256_file

#' @rdname git_commit_info_json
#' @export
git_commit_info <- git_commit_info_json

#' @rdname parse_check_constraint_values
#' @export
get_check_constraint_values <- parse_check_constraint_values