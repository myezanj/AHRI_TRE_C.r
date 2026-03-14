#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#define AHRI_TRE_OK 0

typedef const char *(*fn_version)(void);
typedef int (*fn_sha256_file_hex)(const char *, char **);
typedef int (*fn_verify_sha256_file)(const char *, const char *, int *);
typedef int (*fn_path_to_file_uri)(const char *, char **);
typedef int (*fn_file_uri_to_path)(const char *, char **);
typedef int (*fn_emptydir)(const char *, int, int, int);
typedef int (*fn_is_ncname)(const char *, int, int *);
typedef int (*fn_to_ncname)(const char *, const char *, const char *, int, int, char **);
typedef int (*fn_parse_flavour)(const char *, int *);
typedef int (*fn_map_sql_type_to_tre)(const char *, int *);
typedef int (*fn_map_sql_type_to_tre_for_flavour)(const char *, int, int *);
typedef int (*fn_get_datasetname)(const char *, const char *, int, int, int, int, char **);
typedef int (*fn_get_datafilename)(const char *, int, int, int, char **);
typedef int (*fn_get_datalake_file_path)(const char *, const char *, const char *, const char *, int, int, int, char **);
typedef int (*fn_prepare_datafile_digest)(const char *, const char *, char **);
typedef int (*fn_prepare_datafile_json)(const char *, const char *, int, int, const char *, char **);
typedef int (*fn_normalise_orcid_rolename)(const char *, char **);
typedef int (*fn_makeparams_json)(int, char **);
typedef int (*fn_quote_ident)(const char *, char **);
typedef int (*fn_quote_qualified_identifier)(const char *, char **);
typedef int (*fn_quote_sql_str)(const char *, char **);
typedef int (*fn_julia_type_to_sql_string)(const char *, char **);
typedef int (*fn_tre_type_to_duckdb_sql)(int, char **);
typedef int (*fn_extract_table_from_sql)(const char *, char **);
typedef int (*fn_parse_in_list_values_json)(const char *, char **);
typedef int (*fn_parse_check_constraint_values_json)(const char *, const char *, char **);
typedef int (*fn_map_value_type)(const char *, const char *, int *, char **);
typedef int (*fn_parse_redcap_choices_json)(const char *, char **);
typedef int (*fn_strip_html)(const char *, char **);
typedef int (*fn_infer_label_from_field_name)(const char *, char **);
typedef int (*fn_get_redcap_choices_for_field_json)(const char *, const char *, char **);
typedef int (*fn_normalize_git_remote)(const char *, char **);
typedef int (*fn_canonical_path)(const char *, char **);
typedef int (*fn_git_commit_info_json)(const char *, int, const char *, char **);
typedef int (*fn_caller_file_runtime)(const char *, char **);
typedef void (*fn_free_ptr)(void *);
typedef const char *(*fn_last_error)(void);

static fn_version p_version = NULL;
static fn_sha256_file_hex p_sha256_file_hex = NULL;
static fn_verify_sha256_file p_verify_sha256_file = NULL;
static fn_path_to_file_uri p_path_to_file_uri = NULL;
static fn_file_uri_to_path p_file_uri_to_path = NULL;
static fn_emptydir p_emptydir = NULL;
static fn_is_ncname p_is_ncname = NULL;
static fn_to_ncname p_to_ncname = NULL;
static fn_parse_flavour p_parse_flavour = NULL;
static fn_map_sql_type_to_tre p_map_sql_type_to_tre = NULL;
static fn_map_sql_type_to_tre_for_flavour p_map_sql_type_to_tre_for_flavour = NULL;
static fn_get_datasetname p_get_datasetname = NULL;
static fn_get_datafilename p_get_datafilename = NULL;
static fn_get_datalake_file_path p_get_datalake_file_path = NULL;
static fn_prepare_datafile_digest p_prepare_datafile_digest = NULL;
static fn_prepare_datafile_json p_prepare_datafile_json = NULL;
static fn_normalise_orcid_rolename p_normalise_orcid_rolename = NULL;
static fn_makeparams_json p_makeparams_json = NULL;
static fn_quote_ident p_quote_ident = NULL;
static fn_quote_qualified_identifier p_quote_qualified_identifier = NULL;
static fn_quote_sql_str p_quote_sql_str = NULL;
static fn_julia_type_to_sql_string p_julia_type_to_sql_string = NULL;
static fn_tre_type_to_duckdb_sql p_tre_type_to_duckdb_sql = NULL;
static fn_extract_table_from_sql p_extract_table_from_sql = NULL;
static fn_parse_in_list_values_json p_parse_in_list_values_json = NULL;
static fn_parse_check_constraint_values_json p_parse_check_constraint_values_json = NULL;
static fn_map_value_type p_map_value_type = NULL;
static fn_parse_redcap_choices_json p_parse_redcap_choices_json = NULL;
static fn_strip_html p_strip_html = NULL;
static fn_infer_label_from_field_name p_infer_label_from_field_name = NULL;
static fn_get_redcap_choices_for_field_json p_get_redcap_choices_for_field_json = NULL;
static fn_normalize_git_remote p_normalize_git_remote = NULL;
static fn_canonical_path p_canonical_path = NULL;
static fn_git_commit_info_json p_git_commit_info_json = NULL;
static fn_caller_file_runtime p_caller_file_runtime = NULL;
static fn_free_ptr p_free_ptr = NULL;
static fn_last_error p_last_error = NULL;
static int symbols_initialized = 0;

#ifdef _WIN32
static void *lookup_symbol(const char *name) {
    const char *module_names[] = {
        "tre_c.dll",
        "libtre_c.dll",
        "ahri_tre_c.dll",
        "libahri_tre_c.dll"
    };
    HMODULE mod = NULL;
    size_t i;
    const char *explicit_path;

    for (i = 0; i < sizeof(module_names) / sizeof(module_names[0]); ++i) {
        mod = GetModuleHandleA(module_names[i]);
        if (mod != NULL) {
            break;
        }
    }
    if (mod == NULL) {
        explicit_path = getenv("AHRI_TRE_C_LIB");
        if (explicit_path != NULL && explicit_path[0] != '\0') {
            mod = LoadLibraryA(explicit_path);
        }
    }
    if (mod == NULL) {
        return NULL;
    }
    return (void *)GetProcAddress(mod, name);
}
#else
static void *lookup_symbol(const char *name) {
    return dlsym(RTLD_DEFAULT, name);
}
#endif

static void *lookup_symbol_any(const char *primary_name, const char *fallback_name) {
    void *sym = lookup_symbol(primary_name);
    if (sym != NULL) {
        return sym;
    }
    return lookup_symbol(fallback_name);
}

static const char *last_error_message(void) {
    if (p_last_error == NULL) {
        return "AHRI TRE core not loaded";
    }
    return p_last_error();
}

static const char *require_string(SEXP value, const char *name) {
    if (!Rf_isString(value) || Rf_length(value) != 1) {
        Rf_error("%s must be a single string", name);
    }
    return CHAR(STRING_ELT(value, 0));
}

static const char *optional_string(SEXP value, const char *name) {
    if (value == R_NilValue) {
        return NULL;
    }
    return require_string(value, name);
}

static int optional_int(SEXP value, int default_value, const char *name) {
    if (value == R_NilValue) {
        return default_value;
    }
    if (!Rf_isNumeric(value) || Rf_length(value) != 1) {
        Rf_error("%s must be NULL or a single numeric value", name);
    }
    return Rf_asInteger(value);
}

static int optional_flag(SEXP value, int default_value, const char *name) {
    if (value == R_NilValue) {
        return default_value;
    }
    if (TYPEOF(value) != LGLSXP || Rf_length(value) != 1) {
        Rf_error("%s must be NULL or a single logical value", name);
    }
    return LOGICAL(value)[0] == TRUE;
}

static SEXP string_result_and_free(char *value, const char *fallback) {
    SEXP result = PROTECT(Rf_mkString(value == NULL ? fallback : value));
    if (value != NULL) {
        p_free_ptr(value);
    }
    UNPROTECT(1);
    return result;
}

static void ensure_core_symbols_loaded(void) {
    if (symbols_initialized) {
        return;
    }

    p_version = (fn_version)lookup_symbol_any("version", "ahri_tre_version");
    p_sha256_file_hex = (fn_sha256_file_hex)lookup_symbol_any("sha256_file_hex", "ahri_tre_sha256_file_hex");
    p_verify_sha256_file = (fn_verify_sha256_file)lookup_symbol_any("verify_sha256_file", "ahri_tre_verify_sha256_file");
    p_path_to_file_uri = (fn_path_to_file_uri)lookup_symbol_any("path_to_file_uri", "ahri_tre_path_to_file_uri");
    p_file_uri_to_path = (fn_file_uri_to_path)lookup_symbol_any("file_uri_to_path", "ahri_tre_file_uri_to_path");
    p_emptydir = (fn_emptydir)lookup_symbol_any("emptydir", "ahri_tre_emptydir");
    p_is_ncname = (fn_is_ncname)lookup_symbol_any("is_ncname", "ahri_tre_is_ncname");
    p_to_ncname = (fn_to_ncname)lookup_symbol_any("to_ncname", "ahri_tre_to_ncname");
    p_parse_flavour = (fn_parse_flavour)lookup_symbol_any("parse_flavour", "ahri_tre_parse_flavour");
    p_map_sql_type_to_tre = (fn_map_sql_type_to_tre)lookup_symbol_any("map_sql_type_to_tre", "ahri_tre_map_sql_type_to_tre");
    p_map_sql_type_to_tre_for_flavour = (fn_map_sql_type_to_tre_for_flavour)lookup_symbol_any("map_sql_type_to_tre_for_flavour", "ahri_tre_map_sql_type_to_tre_for_flavour");
    p_get_datasetname = (fn_get_datasetname)lookup_symbol_any("get_datasetname", "ahri_tre_get_datasetname");
    p_get_datafilename = (fn_get_datafilename)lookup_symbol_any("get_datafilename", "ahri_tre_get_datafilename");
    p_get_datalake_file_path = (fn_get_datalake_file_path)lookup_symbol_any("get_datalake_file_path", "ahri_tre_get_datalake_file_path");
    p_prepare_datafile_digest = (fn_prepare_datafile_digest)lookup_symbol_any("prepare_datafile_digest", "ahri_tre_prepare_datafile_digest");
    p_prepare_datafile_json = (fn_prepare_datafile_json)lookup_symbol_any("prepare_datafile_json", "ahri_tre_prepare_datafile_json");
    p_normalise_orcid_rolename = (fn_normalise_orcid_rolename)lookup_symbol_any("normalise_orcid_rolename", "ahri_tre_normalise_orcid_rolename");
    p_makeparams_json = (fn_makeparams_json)lookup_symbol_any("makeparams_json", "ahri_tre_makeparams_json");
    p_quote_ident = (fn_quote_ident)lookup_symbol_any("quote_ident", "ahri_tre_quote_ident");
    p_quote_qualified_identifier = (fn_quote_qualified_identifier)lookup_symbol_any("quote_qualified_identifier", "ahri_tre_quote_qualified_identifier");
    p_quote_sql_str = (fn_quote_sql_str)lookup_symbol_any("quote_sql_str", "ahri_tre_quote_sql_str");
    p_julia_type_to_sql_string = (fn_julia_type_to_sql_string)lookup_symbol_any("julia_type_to_sql_string", "ahri_tre_julia_type_to_sql_string");
    p_tre_type_to_duckdb_sql = (fn_tre_type_to_duckdb_sql)lookup_symbol_any("tre_type_to_duckdb_sql", "ahri_tre_tre_type_to_duckdb_sql");
    p_extract_table_from_sql = (fn_extract_table_from_sql)lookup_symbol_any("extract_table_from_sql", "ahri_tre_extract_table_from_sql");
    p_parse_in_list_values_json = (fn_parse_in_list_values_json)lookup_symbol_any("parse_in_list_values_json", "ahri_tre_parse_in_list_values_json");
    p_parse_check_constraint_values_json = (fn_parse_check_constraint_values_json)lookup_symbol_any("parse_check_constraint_values_json", "ahri_tre_parse_check_constraint_values_json");
    p_map_value_type = (fn_map_value_type)lookup_symbol_any("map_value_type", "ahri_tre_map_redcap_value_type");
    p_parse_redcap_choices_json = (fn_parse_redcap_choices_json)lookup_symbol_any("parse_redcap_choices_json", "ahri_tre_parse_redcap_choices_json");
    p_strip_html = (fn_strip_html)lookup_symbol_any("strip_html", "ahri_tre_strip_html");
    p_infer_label_from_field_name = (fn_infer_label_from_field_name)lookup_symbol_any("infer_label_from_field_name", "ahri_tre_infer_label_from_field_name");
    p_get_redcap_choices_for_field_json = (fn_get_redcap_choices_for_field_json)lookup_symbol_any("get_redcap_choices_for_field_json", "ahri_tre_get_redcap_choices_for_field_json");
    p_normalize_git_remote = (fn_normalize_git_remote)lookup_symbol_any("normalize_git_remote", "ahri_tre_normalize_git_remote");
    p_canonical_path = (fn_canonical_path)lookup_symbol_any("canonical_path", "ahri_tre_canonical_path");
    p_git_commit_info_json = (fn_git_commit_info_json)lookup_symbol_any("git_commit_info_json", "ahri_tre_git_commit_info_json");
    p_caller_file_runtime = (fn_caller_file_runtime)lookup_symbol_any("caller_file_runtime", "ahri_tre_caller_file_runtime");
    p_free_ptr = (fn_free_ptr)lookup_symbol_any("free_ptr", "ahri_tre_free");
    p_last_error = (fn_last_error)lookup_symbol_any("last_error", "ahri_tre_last_error");

    if (p_version == NULL ||
        p_sha256_file_hex == NULL ||
        p_verify_sha256_file == NULL ||
        p_path_to_file_uri == NULL ||
        p_file_uri_to_path == NULL ||
        p_emptydir == NULL ||
        p_is_ncname == NULL ||
        p_to_ncname == NULL ||
        p_parse_flavour == NULL ||
        p_map_sql_type_to_tre == NULL ||
        p_map_sql_type_to_tre_for_flavour == NULL ||
        p_get_datasetname == NULL ||
        p_get_datafilename == NULL ||
        p_get_datalake_file_path == NULL ||
        p_prepare_datafile_digest == NULL ||
        p_prepare_datafile_json == NULL ||
        p_normalise_orcid_rolename == NULL ||
        p_makeparams_json == NULL ||
        p_quote_ident == NULL ||
        p_quote_qualified_identifier == NULL ||
        p_quote_sql_str == NULL ||
        p_julia_type_to_sql_string == NULL ||
        p_tre_type_to_duckdb_sql == NULL ||
        p_extract_table_from_sql == NULL ||
        p_parse_in_list_values_json == NULL ||
        p_parse_check_constraint_values_json == NULL ||
        p_map_value_type == NULL ||
        p_parse_redcap_choices_json == NULL ||
        p_strip_html == NULL ||
        p_infer_label_from_field_name == NULL ||
        p_get_redcap_choices_for_field_json == NULL ||
        p_normalize_git_remote == NULL ||
        p_canonical_path == NULL ||
        p_git_commit_info_json == NULL ||
        p_caller_file_runtime == NULL ||
        p_free_ptr == NULL ||
        p_last_error == NULL) {
        Rf_error("AHRI TRE core symbols are not loaded. Call tre_load() first.");
    }

    symbols_initialized = 1;
}

SEXP version_R(void) {
    ensure_core_symbols_loaded();
    return Rf_mkString(p_version());
}

SEXP sha256_file_hex_R(SEXP path) {
    const char *cpath;
    char *digest = NULL;
    int rc;
    SEXP out;

    if (!Rf_isString(path) || Rf_length(path) != 1) {
        Rf_error("path must be a single string");
    }

    cpath = CHAR(STRING_ELT(path, 0));
    ensure_core_symbols_loaded();
    rc = p_sha256_file_hex(cpath, &digest);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }

    out = PROTECT(Rf_mkString(digest));
    p_free_ptr(digest);
    UNPROTECT(1);
    return out;
}

SEXP verify_sha256_file_R(SEXP path, SEXP expected) {
    const char *cpath;
    const char *cexpected;
    int match = 0;
    int rc;

    if (!Rf_isString(path) || Rf_length(path) != 1) {
        Rf_error("path must be a single string");
    }
    if (!Rf_isString(expected) || Rf_length(expected) != 1) {
        Rf_error("expected must be a single string");
    }

    cpath = CHAR(STRING_ELT(path, 0));
    cexpected = CHAR(STRING_ELT(expected, 0));

    ensure_core_symbols_loaded();
    rc = p_verify_sha256_file(cpath, cexpected, &match);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }

    return Rf_ScalarLogical(match != 0);
}

SEXP path_to_file_uri_R(SEXP path) {
    char *out = NULL;
    int rc;
    ensure_core_symbols_loaded();
    rc = p_path_to_file_uri(require_string(path, "path"), &out);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    return string_result_and_free(out, "");
}

SEXP file_uri_to_path_R(SEXP uri) {
    char *out = NULL;
    int rc;
    ensure_core_symbols_loaded();
    rc = p_file_uri_to_path(require_string(uri, "uri"), &out);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    return string_result_and_free(out, "");
}

SEXP emptydir_R(SEXP path, SEXP create, SEXP retries, SEXP wait_millis) {
    int rc;
    ensure_core_symbols_loaded();
    rc = p_emptydir(
        require_string(path, "path"),
        optional_flag(create, 0, "create"),
        optional_int(retries, 0, "retries"),
        optional_int(wait_millis, 0, "wait_millis")
    );
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    return Rf_ScalarLogical(1);
}

SEXP is_ncname_R(SEXP value, SEXP strict) {
    int out_valid = 0;
    int rc;
    ensure_core_symbols_loaded();
    rc = p_is_ncname(require_string(value, "value"), optional_flag(strict, 1, "strict"), &out_valid);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    return Rf_ScalarLogical(out_valid != 0);
}

SEXP to_ncname_R(SEXP value, SEXP replacement, SEXP prefix, SEXP avoid_reserved, SEXP strict) {
    char *out = NULL;
    int rc;
    ensure_core_symbols_loaded();
    rc = p_to_ncname(
        require_string(value, "value"),
        optional_string(replacement, "replacement"),
        optional_string(prefix, "prefix"),
        optional_flag(avoid_reserved, 1, "avoid_reserved"),
        optional_flag(strict, 1, "strict"),
        &out
    );
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    return string_result_and_free(out, "");
}

SEXP parse_flavour_R(SEXP flavour) {
    int out_flavour = 0;
    int rc;
    if (!Rf_isString(flavour) || Rf_length(flavour) != 1) {
        Rf_error("flavour must be a single string");
    }
    ensure_core_symbols_loaded();
    rc = p_parse_flavour(CHAR(STRING_ELT(flavour, 0)), &out_flavour);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    return Rf_ScalarInteger(out_flavour);
}

SEXP map_sql_type_to_tre_R(SEXP sql_type) {
    int out_type = 0;
    int rc;
    if (!Rf_isString(sql_type) || Rf_length(sql_type) != 1) {
        Rf_error("sql_type must be a single string");
    }
    ensure_core_symbols_loaded();
    rc = p_map_sql_type_to_tre(CHAR(STRING_ELT(sql_type, 0)), &out_type);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    return Rf_ScalarInteger(out_type);
}

SEXP map_sql_type_to_tre_for_flavour_R(SEXP sql_type, SEXP flavour_id) {
    int out_type = 0;
    int rc;
    ensure_core_symbols_loaded();
    rc = p_map_sql_type_to_tre_for_flavour(
        require_string(sql_type, "sql_type"),
        optional_int(flavour_id, 0, "flavour_id"),
        &out_type
    );
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    return Rf_ScalarInteger(out_type);
}

SEXP get_datasetname_R(SEXP study_name, SEXP asset_name, SEXP major, SEXP minor, SEXP patch, SEXP include_schema) {
    char *out = NULL;
    int rc;
    ensure_core_symbols_loaded();
    rc = p_get_datasetname(
        require_string(study_name, "study_name"),
        require_string(asset_name, "asset_name"),
        optional_int(major, 1, "major"),
        optional_int(minor, 0, "minor"),
        optional_int(patch, 0, "patch"),
        optional_flag(include_schema, 0, "include_schema"),
        &out
    );
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    return string_result_and_free(out, "");
}

SEXP get_datafilename_R(SEXP asset_name, SEXP major, SEXP minor, SEXP patch) {
    char *out = NULL;
    int rc;
    ensure_core_symbols_loaded();
    rc = p_get_datafilename(
        require_string(asset_name, "asset_name"),
        optional_int(major, 1, "major"),
        optional_int(minor, 0, "minor"),
        optional_int(patch, 0, "patch"),
        &out
    );
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    return string_result_and_free(out, "");
}

SEXP get_datalake_file_path_R(SEXP lake_data, SEXP study_name, SEXP asset_name, SEXP source_file_path, SEXP major, SEXP minor, SEXP patch) {
    char *out = NULL;
    int rc;
    ensure_core_symbols_loaded();
    rc = p_get_datalake_file_path(
        require_string(lake_data, "lake_data"),
        require_string(study_name, "study_name"),
        require_string(asset_name, "asset_name"),
        require_string(source_file_path, "source_file_path"),
        optional_int(major, 1, "major"),
        optional_int(minor, 0, "minor"),
        optional_int(patch, 0, "patch"),
        &out
    );
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    return string_result_and_free(out, "");
}

SEXP prepare_datafile_digest_R(SEXP file_path, SEXP precomputed_digest) {
    char *out = NULL;
    int rc;
    ensure_core_symbols_loaded();
    rc = p_prepare_datafile_digest(
        require_string(file_path, "file_path"),
        optional_string(precomputed_digest, "precomputed_digest"),
        &out
    );
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    return string_result_and_free(out, "");
}

SEXP prepare_datafile_json_R(SEXP file_path, SEXP edam_format, SEXP compress, SEXP encrypt, SEXP precomputed_digest) {
    char *out = NULL;
    int rc;
    ensure_core_symbols_loaded();
    rc = p_prepare_datafile_json(
        require_string(file_path, "file_path"),
        require_string(edam_format, "edam_format"),
        optional_flag(compress, 0, "compress"),
        optional_flag(encrypt, 0, "encrypt"),
        optional_string(precomputed_digest, "precomputed_digest"),
        &out
    );
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    return string_result_and_free(out, "");
}

SEXP normalise_orcid_rolename_R(SEXP orcid) {
    char *out = NULL;
    int rc;
    ensure_core_symbols_loaded();
    rc = p_normalise_orcid_rolename(require_string(orcid, "orcid"), &out);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    return string_result_and_free(out, "");
}

SEXP makeparams_json_R(SEXP n) {
    char *out = NULL;
    int rc;
    if (!Rf_isNumeric(n) || Rf_length(n) != 1) {
        Rf_error("n must be a single numeric value");
    }
    ensure_core_symbols_loaded();
    rc = p_makeparams_json(Rf_asInteger(n), &out);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    return string_result_and_free(out, "");
}

SEXP quote_ident_R(SEXP name) {
    char *out = NULL;
    int rc;
    ensure_core_symbols_loaded();
    rc = p_quote_ident(require_string(name, "name"), &out);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    return string_result_and_free(out, "");
}

SEXP quote_qualified_identifier_R(SEXP name) {
    char *out = NULL;
    int rc;
    ensure_core_symbols_loaded();
    rc = p_quote_qualified_identifier(require_string(name, "name"), &out);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    return string_result_and_free(out, "");
}

SEXP quote_sql_str_R(SEXP value) {
    char *out = NULL;
    int rc;
    ensure_core_symbols_loaded();
    rc = p_quote_sql_str(require_string(value, "value"), &out);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    return string_result_and_free(out, "");
}

SEXP julia_type_to_sql_string_R(SEXP julia_type) {
    char *out = NULL;
    int rc;
    ensure_core_symbols_loaded();
    rc = p_julia_type_to_sql_string(require_string(julia_type, "julia_type"), &out);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    return string_result_and_free(out, "");
}

SEXP tre_type_to_duckdb_sql_R(SEXP value_type_id) {
    char *out = NULL;
    int rc;
    if (!Rf_isNumeric(value_type_id) || Rf_length(value_type_id) != 1) {
        Rf_error("value_type_id must be a single numeric value");
    }
    ensure_core_symbols_loaded();
    rc = p_tre_type_to_duckdb_sql(Rf_asInteger(value_type_id), &out);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    return string_result_and_free(out, "");
}

SEXP extract_table_from_sql_R(SEXP sql) {
    char *out = NULL;
    int rc;
    SEXP res;
    if (!Rf_isString(sql) || Rf_length(sql) != 1) {
        Rf_error("sql must be a single string");
    }
    ensure_core_symbols_loaded();
    rc = p_extract_table_from_sql(CHAR(STRING_ELT(sql, 0)), &out);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    res = PROTECT(Rf_mkString(out == NULL ? "" : out));
    p_free_ptr(out);
    UNPROTECT(1);
    return res;
}

SEXP parse_in_list_values_json_R(SEXP values) {
    char *out = NULL;
    int rc;
    SEXP res;
    if (!Rf_isString(values) || Rf_length(values) != 1) {
        Rf_error("values must be a single string");
    }
    ensure_core_symbols_loaded();
    rc = p_parse_in_list_values_json(CHAR(STRING_ELT(values, 0)), &out);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    res = PROTECT(Rf_mkString(out == NULL ? "[]" : out));
    p_free_ptr(out);
    UNPROTECT(1);
    return res;
}

SEXP parse_check_constraint_values_json_R(SEXP constraint_def, SEXP column_name) {
    char *out = NULL;
    int rc;
    SEXP res;
    if (!Rf_isString(constraint_def) || Rf_length(constraint_def) != 1) {
        Rf_error("constraint_def must be a single string");
    }
    if (!Rf_isString(column_name) || Rf_length(column_name) != 1) {
        Rf_error("column_name must be a single string");
    }
    ensure_core_symbols_loaded();
    rc = p_parse_check_constraint_values_json(
        CHAR(STRING_ELT(constraint_def, 0)),
        CHAR(STRING_ELT(column_name, 0)),
        &out
    );
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    res = PROTECT(Rf_mkString(out == NULL ? "[]" : out));
    p_free_ptr(out);
    UNPROTECT(1);
    return res;
}

SEXP map_value_type_R(SEXP field_type, SEXP validation) {
    int out_type = 0;
    char *out_fmt = NULL;
    int rc;
    SEXP res;
    SEXP names;
    const char *val_ptr = NULL;

    if (!Rf_isString(field_type) || Rf_length(field_type) != 1) {
        Rf_error("field_type must be a single string");
    }
    if (validation != R_NilValue) {
        if (!Rf_isString(validation) || Rf_length(validation) != 1) {
            Rf_error("validation must be NULL or a single string");
        }
        val_ptr = CHAR(STRING_ELT(validation, 0));
    }

    ensure_core_symbols_loaded();
    rc = p_map_value_type(CHAR(STRING_ELT(field_type, 0)), val_ptr, &out_type, &out_fmt);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }

    res = PROTECT(Rf_allocVector(VECSXP, 2));
    names = PROTECT(Rf_allocVector(STRSXP, 2));
    SET_STRING_ELT(names, 0, Rf_mkChar("value_type"));
    SET_STRING_ELT(names, 1, Rf_mkChar("value_format"));
    Rf_setAttrib(res, R_NamesSymbol, names);

    SET_VECTOR_ELT(res, 0, Rf_ScalarInteger(out_type));
    if (out_fmt == NULL) {
        SET_VECTOR_ELT(res, 1, R_NilValue);
    } else {
        SET_VECTOR_ELT(res, 1, Rf_mkString(out_fmt));
    }
    p_free_ptr(out_fmt);
    UNPROTECT(2);
    return res;
}

SEXP parse_redcap_choices_json_R(SEXP choices) {
    char *out = NULL;
    int rc;
    SEXP res;
    if (!Rf_isString(choices) || Rf_length(choices) != 1) {
        Rf_error("choices must be a single string");
    }
    ensure_core_symbols_loaded();
    rc = p_parse_redcap_choices_json(CHAR(STRING_ELT(choices, 0)), &out);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    res = PROTECT(Rf_mkString(out == NULL ? "[]" : out));
    p_free_ptr(out);
    UNPROTECT(1);
    return res;
}

SEXP strip_html_R(SEXP text) {
    char *out = NULL;
    int rc;
    SEXP res;
    if (!Rf_isString(text) || Rf_length(text) != 1) {
        Rf_error("text must be a single string");
    }
    ensure_core_symbols_loaded();
    rc = p_strip_html(CHAR(STRING_ELT(text, 0)), &out);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    res = PROTECT(Rf_mkString(out == NULL ? "" : out));
    p_free_ptr(out);
    UNPROTECT(1);
    return res;
}

SEXP infer_label_from_field_name_R(SEXP field_name) {
    char *out = NULL;
    int rc;
    SEXP res;
    if (!Rf_isString(field_name) || Rf_length(field_name) != 1) {
        Rf_error("field_name must be a single string");
    }
    ensure_core_symbols_loaded();
    rc = p_infer_label_from_field_name(CHAR(STRING_ELT(field_name, 0)), &out);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    res = PROTECT(Rf_mkString(out == NULL ? "" : out));
    p_free_ptr(out);
    UNPROTECT(1);
    return res;
}

SEXP get_redcap_choices_for_field_json_R(SEXP field_type, SEXP choices) {
    char *out = NULL;
    int rc;
    const char *choices_ptr = NULL;
    SEXP res;
    if (!Rf_isString(field_type) || Rf_length(field_type) != 1) {
        Rf_error("field_type must be a single string");
    }
    if (choices != R_NilValue) {
        if (!Rf_isString(choices) || Rf_length(choices) != 1) {
            Rf_error("choices must be NULL or a single string");
        }
        choices_ptr = CHAR(STRING_ELT(choices, 0));
    }
    ensure_core_symbols_loaded();
    rc = p_get_redcap_choices_for_field_json(
        CHAR(STRING_ELT(field_type, 0)),
        choices_ptr,
        &out
    );
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    res = PROTECT(Rf_mkString(out == NULL ? "[]" : out));
    p_free_ptr(out);
    UNPROTECT(1);
    return res;
}

SEXP normalize_git_remote_R(SEXP url) {
    char *out = NULL;
    int rc;
    ensure_core_symbols_loaded();
    rc = p_normalize_git_remote(require_string(url, "url"), &out);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    return string_result_and_free(out, "");
}

SEXP canonical_path_R(SEXP path) {
    char *out = NULL;
    int rc;
    ensure_core_symbols_loaded();
    rc = p_canonical_path(require_string(path, "path"), &out);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    return string_result_and_free(out, "");
}

SEXP git_commit_info_json_R(SEXP dir, SEXP short_hash, SEXP script_path) {
    char *out = NULL;
    int rc;
    ensure_core_symbols_loaded();
    rc = p_git_commit_info_json(
        require_string(dir, "dir"),
        optional_flag(short_hash, 0, "short_hash"),
        optional_string(script_path, "script_path"),
        &out
    );
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    return string_result_and_free(out, "{}");
}

SEXP caller_file_runtime_R(SEXP hint_path) {
    char *out = NULL;
    int rc;
    ensure_core_symbols_loaded();
    rc = p_caller_file_runtime(optional_string(hint_path, "hint_path"), &out);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    return string_result_and_free(out, "");
}

static const R_CallMethodDef callMethods[] = {
    {"version_R", (DL_FUNC)&version_R, 0},
    {"sha256_file_hex_R", (DL_FUNC)&sha256_file_hex_R, 1},
    {"verify_sha256_file_R", (DL_FUNC)&verify_sha256_file_R, 2},
    {"path_to_file_uri_R", (DL_FUNC)&path_to_file_uri_R, 1},
    {"file_uri_to_path_R", (DL_FUNC)&file_uri_to_path_R, 1},
    {"emptydir_R", (DL_FUNC)&emptydir_R, 4},
    {"is_ncname_R", (DL_FUNC)&is_ncname_R, 2},
    {"to_ncname_R", (DL_FUNC)&to_ncname_R, 5},
    {"parse_flavour_R", (DL_FUNC)&parse_flavour_R, 1},
    {"map_sql_type_to_tre_R", (DL_FUNC)&map_sql_type_to_tre_R, 1},
    {"map_sql_type_to_tre_for_flavour_R", (DL_FUNC)&map_sql_type_to_tre_for_flavour_R, 2},
    {"get_datasetname_R", (DL_FUNC)&get_datasetname_R, 6},
    {"get_datafilename_R", (DL_FUNC)&get_datafilename_R, 4},
    {"get_datalake_file_path_R", (DL_FUNC)&get_datalake_file_path_R, 7},
    {"prepare_datafile_digest_R", (DL_FUNC)&prepare_datafile_digest_R, 2},
    {"prepare_datafile_json_R", (DL_FUNC)&prepare_datafile_json_R, 5},
    {"normalise_orcid_rolename_R", (DL_FUNC)&normalise_orcid_rolename_R, 1},
    {"makeparams_json_R", (DL_FUNC)&makeparams_json_R, 1},
    {"quote_ident_R", (DL_FUNC)&quote_ident_R, 1},
    {"quote_qualified_identifier_R", (DL_FUNC)&quote_qualified_identifier_R, 1},
    {"quote_sql_str_R", (DL_FUNC)&quote_sql_str_R, 1},
    {"julia_type_to_sql_string_R", (DL_FUNC)&julia_type_to_sql_string_R, 1},
    {"tre_type_to_duckdb_sql_R", (DL_FUNC)&tre_type_to_duckdb_sql_R, 1},
    {"extract_table_from_sql_R", (DL_FUNC)&extract_table_from_sql_R, 1},
    {"parse_in_list_values_json_R", (DL_FUNC)&parse_in_list_values_json_R, 1},
    {"parse_check_constraint_values_json_R", (DL_FUNC)&parse_check_constraint_values_json_R, 2},
    {"map_value_type_R", (DL_FUNC)&map_value_type_R, 2},
    {"parse_redcap_choices_json_R", (DL_FUNC)&parse_redcap_choices_json_R, 1},
    {"strip_html_R", (DL_FUNC)&strip_html_R, 1},
    {"infer_label_from_field_name_R", (DL_FUNC)&infer_label_from_field_name_R, 1},
    {"get_redcap_choices_for_field_json_R", (DL_FUNC)&get_redcap_choices_for_field_json_R, 2},
    {"normalize_git_remote_R", (DL_FUNC)&normalize_git_remote_R, 1},
    {"canonical_path_R", (DL_FUNC)&canonical_path_R, 1},
    {"git_commit_info_json_R", (DL_FUNC)&git_commit_info_json_R, 3},
    {"caller_file_runtime_R", (DL_FUNC)&caller_file_runtime_R, 1},
    {NULL, NULL, 0}
};

void R_init_AHRITREC(DllInfo *dll) {
    R_registerRoutines(dll, NULL, callMethods, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}

