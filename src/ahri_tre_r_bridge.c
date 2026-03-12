#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#define AHRI_TRE_OK 0

typedef const char *(*fn_ahri_tre_version)(void);
typedef int (*fn_ahri_tre_sha256_file_hex)(const char *, char **);
typedef int (*fn_ahri_tre_verify_sha256_file)(const char *, const char *, int *);
typedef int (*fn_ahri_tre_parse_flavour)(const char *, int *);
typedef int (*fn_ahri_tre_map_sql_type_to_tre)(const char *, int *);
typedef int (*fn_ahri_tre_extract_table_from_sql)(const char *, char **);
typedef int (*fn_ahri_tre_parse_in_list_values_json)(const char *, char **);
typedef int (*fn_ahri_tre_parse_check_constraint_values_json)(const char *, const char *, char **);
typedef int (*fn_ahri_tre_map_redcap_value_type)(const char *, const char *, int *, char **);
typedef int (*fn_ahri_tre_parse_redcap_choices_json)(const char *, char **);
typedef int (*fn_ahri_tre_strip_html)(const char *, char **);
typedef int (*fn_ahri_tre_infer_label_from_field_name)(const char *, char **);
typedef int (*fn_ahri_tre_get_redcap_choices_for_field_json)(const char *, const char *, char **);
typedef void (*fn_ahri_tre_free)(void *);
typedef const char *(*fn_ahri_tre_last_error)(void);

static fn_ahri_tre_version p_ahri_tre_version = NULL;
static fn_ahri_tre_sha256_file_hex p_ahri_tre_sha256_file_hex = NULL;
static fn_ahri_tre_verify_sha256_file p_ahri_tre_verify_sha256_file = NULL;
static fn_ahri_tre_parse_flavour p_ahri_tre_parse_flavour = NULL;
static fn_ahri_tre_map_sql_type_to_tre p_ahri_tre_map_sql_type_to_tre = NULL;
static fn_ahri_tre_extract_table_from_sql p_ahri_tre_extract_table_from_sql = NULL;
static fn_ahri_tre_parse_in_list_values_json p_ahri_tre_parse_in_list_values_json = NULL;
static fn_ahri_tre_parse_check_constraint_values_json p_ahri_tre_parse_check_constraint_values_json = NULL;
static fn_ahri_tre_map_redcap_value_type p_ahri_tre_map_redcap_value_type = NULL;
static fn_ahri_tre_parse_redcap_choices_json p_ahri_tre_parse_redcap_choices_json = NULL;
static fn_ahri_tre_strip_html p_ahri_tre_strip_html = NULL;
static fn_ahri_tre_infer_label_from_field_name p_ahri_tre_infer_label_from_field_name = NULL;
static fn_ahri_tre_get_redcap_choices_for_field_json p_ahri_tre_get_redcap_choices_for_field_json = NULL;
static fn_ahri_tre_free p_ahri_tre_free = NULL;
static fn_ahri_tre_last_error p_ahri_tre_last_error = NULL;
static int symbols_initialized = 0;

#ifdef _WIN32
static void *lookup_symbol(const char *name) {
    HMODULE mod = GetModuleHandleA("ahri_tre_c.dll");
    if (mod == NULL) {
        mod = GetModuleHandleA("libahri_tre_c.dll");
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

static const char *last_error_message(void) {
    if (p_ahri_tre_last_error == NULL) {
        return "AHRI TRE core not loaded";
    }
    return p_ahri_tre_last_error();
}

static void ensure_core_symbols_loaded(void) {
    if (symbols_initialized) {
        return;
    }

    p_ahri_tre_version = (fn_ahri_tre_version)lookup_symbol("ahri_tre_version");
    p_ahri_tre_sha256_file_hex = (fn_ahri_tre_sha256_file_hex)lookup_symbol("ahri_tre_sha256_file_hex");
    p_ahri_tre_verify_sha256_file = (fn_ahri_tre_verify_sha256_file)lookup_symbol("ahri_tre_verify_sha256_file");
    p_ahri_tre_parse_flavour = (fn_ahri_tre_parse_flavour)lookup_symbol("ahri_tre_parse_flavour");
    p_ahri_tre_map_sql_type_to_tre = (fn_ahri_tre_map_sql_type_to_tre)lookup_symbol("ahri_tre_map_sql_type_to_tre");
    p_ahri_tre_extract_table_from_sql = (fn_ahri_tre_extract_table_from_sql)lookup_symbol("ahri_tre_extract_table_from_sql");
    p_ahri_tre_parse_in_list_values_json = (fn_ahri_tre_parse_in_list_values_json)lookup_symbol("ahri_tre_parse_in_list_values_json");
    p_ahri_tre_parse_check_constraint_values_json = (fn_ahri_tre_parse_check_constraint_values_json)lookup_symbol("ahri_tre_parse_check_constraint_values_json");
    p_ahri_tre_map_redcap_value_type = (fn_ahri_tre_map_redcap_value_type)lookup_symbol("ahri_tre_map_redcap_value_type");
    p_ahri_tre_parse_redcap_choices_json = (fn_ahri_tre_parse_redcap_choices_json)lookup_symbol("ahri_tre_parse_redcap_choices_json");
    p_ahri_tre_strip_html = (fn_ahri_tre_strip_html)lookup_symbol("ahri_tre_strip_html");
    p_ahri_tre_infer_label_from_field_name = (fn_ahri_tre_infer_label_from_field_name)lookup_symbol("ahri_tre_infer_label_from_field_name");
    p_ahri_tre_get_redcap_choices_for_field_json = (fn_ahri_tre_get_redcap_choices_for_field_json)lookup_symbol("ahri_tre_get_redcap_choices_for_field_json");
    p_ahri_tre_free = (fn_ahri_tre_free)lookup_symbol("ahri_tre_free");
    p_ahri_tre_last_error = (fn_ahri_tre_last_error)lookup_symbol("ahri_tre_last_error");

    if (p_ahri_tre_version == NULL ||
        p_ahri_tre_sha256_file_hex == NULL ||
        p_ahri_tre_verify_sha256_file == NULL ||
        p_ahri_tre_parse_flavour == NULL ||
        p_ahri_tre_map_sql_type_to_tre == NULL ||
        p_ahri_tre_extract_table_from_sql == NULL ||
        p_ahri_tre_parse_in_list_values_json == NULL ||
        p_ahri_tre_parse_check_constraint_values_json == NULL ||
        p_ahri_tre_map_redcap_value_type == NULL ||
        p_ahri_tre_parse_redcap_choices_json == NULL ||
        p_ahri_tre_strip_html == NULL ||
        p_ahri_tre_infer_label_from_field_name == NULL ||
        p_ahri_tre_get_redcap_choices_for_field_json == NULL ||
        p_ahri_tre_free == NULL ||
        p_ahri_tre_last_error == NULL) {
        Rf_error("AHRI TRE core symbols are not loaded. Call ahri_tre_load() first.");
    }

    symbols_initialized = 1;
}

SEXP ahri_tre_version_R(void) {
<<<<<<< HEAD
    ensure_core_symbols_loaded();
    return Rf_mkString(p_ahri_tre_version());
=======
    return Rf_mkString(version());
>>>>>>> fc2e963b67ebb664176554cfadfa715565811bb2
}

SEXP ahri_tre_sha256_file_hex_R(SEXP path) {
    const char *cpath;
    char *digest = NULL;
    int rc;
    SEXP out;

    if (!Rf_isString(path) || Rf_length(path) != 1) {
        Rf_error("path must be a single string");
    }

    cpath = CHAR(STRING_ELT(path, 0));
<<<<<<< HEAD
    ensure_core_symbols_loaded();
    rc = p_ahri_tre_sha256_file_hex(cpath, &digest);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }

    out = PROTECT(Rf_mkString(digest));
    p_ahri_tre_free(digest);
=======
    rc = sha256_file_hex(cpath, &digest);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error());
    }

    out = PROTECT(Rf_mkString(digest));
    free_ptr(digest);
>>>>>>> fc2e963b67ebb664176554cfadfa715565811bb2
    UNPROTECT(1);
    return out;
}

SEXP ahri_tre_verify_sha256_file_R(SEXP path, SEXP expected) {
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

<<<<<<< HEAD
    ensure_core_symbols_loaded();
    rc = p_ahri_tre_verify_sha256_file(cpath, cexpected, &match);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
=======
    rc = verify_sha256_file(cpath, cexpected, &match);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error());
>>>>>>> fc2e963b67ebb664176554cfadfa715565811bb2
    }

    return Rf_ScalarLogical(match != 0);
}

SEXP ahri_tre_parse_flavour_R(SEXP flavour) {
    int out_flavour = 0;
    int rc;
    if (!Rf_isString(flavour) || Rf_length(flavour) != 1) {
        Rf_error("flavour must be a single string");
    }
<<<<<<< HEAD
    ensure_core_symbols_loaded();
    rc = p_ahri_tre_parse_flavour(CHAR(STRING_ELT(flavour, 0)), &out_flavour);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
=======
    rc = parse_flavour(CHAR(STRING_ELT(flavour, 0)), &out_flavour);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error());
>>>>>>> fc2e963b67ebb664176554cfadfa715565811bb2
    }
    return Rf_ScalarInteger(out_flavour);
}

SEXP ahri_tre_map_sql_type_to_tre_R(SEXP sql_type) {
    int out_type = 0;
    int rc;
    if (!Rf_isString(sql_type) || Rf_length(sql_type) != 1) {
        Rf_error("sql_type must be a single string");
    }
<<<<<<< HEAD
    ensure_core_symbols_loaded();
    rc = p_ahri_tre_map_sql_type_to_tre(CHAR(STRING_ELT(sql_type, 0)), &out_type);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
=======
    rc = map_sql_type_to_tre(CHAR(STRING_ELT(sql_type, 0)), &out_type);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error());
>>>>>>> fc2e963b67ebb664176554cfadfa715565811bb2
    }
    return Rf_ScalarInteger(out_type);
}

SEXP ahri_tre_extract_table_from_sql_R(SEXP sql) {
    char *out = NULL;
    int rc;
    SEXP res;
    if (!Rf_isString(sql) || Rf_length(sql) != 1) {
        Rf_error("sql must be a single string");
    }
<<<<<<< HEAD
    ensure_core_symbols_loaded();
    rc = p_ahri_tre_extract_table_from_sql(CHAR(STRING_ELT(sql, 0)), &out);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    res = PROTECT(Rf_mkString(out == NULL ? "" : out));
    p_ahri_tre_free(out);
=======
    rc = extract_table_from_sql(CHAR(STRING_ELT(sql, 0)), &out);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error());
    }
    res = PROTECT(Rf_mkString(out == NULL ? "" : out));
    free_ptr(out);
>>>>>>> fc2e963b67ebb664176554cfadfa715565811bb2
    UNPROTECT(1);
    return res;
}

SEXP ahri_tre_parse_in_list_values_json_R(SEXP values) {
    char *out = NULL;
    int rc;
    SEXP res;
    if (!Rf_isString(values) || Rf_length(values) != 1) {
        Rf_error("values must be a single string");
    }
<<<<<<< HEAD
    ensure_core_symbols_loaded();
    rc = p_ahri_tre_parse_in_list_values_json(CHAR(STRING_ELT(values, 0)), &out);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    res = PROTECT(Rf_mkString(out == NULL ? "[]" : out));
    p_ahri_tre_free(out);
=======
    rc = parse_in_list_values_json(CHAR(STRING_ELT(values, 0)), &out);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error());
    }
    res = PROTECT(Rf_mkString(out == NULL ? "[]" : out));
    free_ptr(out);
>>>>>>> fc2e963b67ebb664176554cfadfa715565811bb2
    UNPROTECT(1);
    return res;
}

SEXP ahri_tre_parse_check_constraint_values_json_R(SEXP constraint_def, SEXP column_name) {
    char *out = NULL;
    int rc;
    SEXP res;
    if (!Rf_isString(constraint_def) || Rf_length(constraint_def) != 1) {
        Rf_error("constraint_def must be a single string");
    }
    if (!Rf_isString(column_name) || Rf_length(column_name) != 1) {
        Rf_error("column_name must be a single string");
    }
<<<<<<< HEAD
    ensure_core_symbols_loaded();
    rc = p_ahri_tre_parse_check_constraint_values_json(
=======
    rc = parse_check_constraint_values_json(
>>>>>>> fc2e963b67ebb664176554cfadfa715565811bb2
        CHAR(STRING_ELT(constraint_def, 0)),
        CHAR(STRING_ELT(column_name, 0)),
        &out
    );
    if (rc != AHRI_TRE_OK) {
<<<<<<< HEAD
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    res = PROTECT(Rf_mkString(out == NULL ? "[]" : out));
    p_ahri_tre_free(out);
=======
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error());
    }
    res = PROTECT(Rf_mkString(out == NULL ? "[]" : out));
    free_ptr(out);
>>>>>>> fc2e963b67ebb664176554cfadfa715565811bb2
    UNPROTECT(1);
    return res;
}

SEXP ahri_tre_map_redcap_value_type_R(SEXP field_type, SEXP validation) {
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

<<<<<<< HEAD
    ensure_core_symbols_loaded();
    rc = p_ahri_tre_map_redcap_value_type(CHAR(STRING_ELT(field_type, 0)), val_ptr, &out_type, &out_fmt);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
=======
    rc = map_value_type(CHAR(STRING_ELT(field_type, 0)), val_ptr, &out_type, &out_fmt);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error());
>>>>>>> fc2e963b67ebb664176554cfadfa715565811bb2
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
<<<<<<< HEAD
    p_ahri_tre_free(out_fmt);
=======
    free_ptr(out_fmt);
>>>>>>> fc2e963b67ebb664176554cfadfa715565811bb2
    UNPROTECT(2);
    return res;
}

SEXP ahri_tre_parse_redcap_choices_json_R(SEXP choices) {
    char *out = NULL;
    int rc;
    SEXP res;
    if (!Rf_isString(choices) || Rf_length(choices) != 1) {
        Rf_error("choices must be a single string");
    }
<<<<<<< HEAD
    ensure_core_symbols_loaded();
    rc = p_ahri_tre_parse_redcap_choices_json(CHAR(STRING_ELT(choices, 0)), &out);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    res = PROTECT(Rf_mkString(out == NULL ? "[]" : out));
    p_ahri_tre_free(out);
=======
    rc = parse_redcap_choices_json(CHAR(STRING_ELT(choices, 0)), &out);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error());
    }
    res = PROTECT(Rf_mkString(out == NULL ? "[]" : out));
    free_ptr(out);
>>>>>>> fc2e963b67ebb664176554cfadfa715565811bb2
    UNPROTECT(1);
    return res;
}

SEXP ahri_tre_strip_html_R(SEXP text) {
    char *out = NULL;
    int rc;
    SEXP res;
    if (!Rf_isString(text) || Rf_length(text) != 1) {
        Rf_error("text must be a single string");
    }
<<<<<<< HEAD
    ensure_core_symbols_loaded();
    rc = p_ahri_tre_strip_html(CHAR(STRING_ELT(text, 0)), &out);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    res = PROTECT(Rf_mkString(out == NULL ? "" : out));
    p_ahri_tre_free(out);
=======
    rc = strip_html(CHAR(STRING_ELT(text, 0)), &out);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error());
    }
    res = PROTECT(Rf_mkString(out == NULL ? "" : out));
    free_ptr(out);
>>>>>>> fc2e963b67ebb664176554cfadfa715565811bb2
    UNPROTECT(1);
    return res;
}

SEXP ahri_tre_infer_label_from_field_name_R(SEXP field_name) {
    char *out = NULL;
    int rc;
    SEXP res;
    if (!Rf_isString(field_name) || Rf_length(field_name) != 1) {
        Rf_error("field_name must be a single string");
    }
<<<<<<< HEAD
    ensure_core_symbols_loaded();
    rc = p_ahri_tre_infer_label_from_field_name(CHAR(STRING_ELT(field_name, 0)), &out);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    res = PROTECT(Rf_mkString(out == NULL ? "" : out));
    p_ahri_tre_free(out);
=======
    rc = infer_label_from_field_name(CHAR(STRING_ELT(field_name, 0)), &out);
    if (rc != AHRI_TRE_OK) {
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error());
    }
    res = PROTECT(Rf_mkString(out == NULL ? "" : out));
    free_ptr(out);
>>>>>>> fc2e963b67ebb664176554cfadfa715565811bb2
    UNPROTECT(1);
    return res;
}

SEXP ahri_tre_get_redcap_choices_for_field_json_R(SEXP field_type, SEXP choices) {
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
<<<<<<< HEAD
    ensure_core_symbols_loaded();
    rc = p_ahri_tre_get_redcap_choices_for_field_json(
=======
    rc = get_redcap_choices_for_field_json(
>>>>>>> fc2e963b67ebb664176554cfadfa715565811bb2
        CHAR(STRING_ELT(field_type, 0)),
        choices_ptr,
        &out
    );
    if (rc != AHRI_TRE_OK) {
<<<<<<< HEAD
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error_message());
    }
    res = PROTECT(Rf_mkString(out == NULL ? "[]" : out));
    p_ahri_tre_free(out);
=======
        Rf_error("AHRI_TRE C error %d: %s", rc, last_error());
    }
    res = PROTECT(Rf_mkString(out == NULL ? "[]" : out));
    free_ptr(out);
>>>>>>> fc2e963b67ebb664176554cfadfa715565811bb2
    UNPROTECT(1);
    return res;
}

static const R_CallMethodDef callMethods[] = {
    {"ahri_tre_version_R", (DL_FUNC)&ahri_tre_version_R, 0},
    {"ahri_tre_sha256_file_hex_R", (DL_FUNC)&ahri_tre_sha256_file_hex_R, 1},
    {"ahri_tre_verify_sha256_file_R", (DL_FUNC)&ahri_tre_verify_sha256_file_R, 2},
    {"ahri_tre_parse_flavour_R", (DL_FUNC)&ahri_tre_parse_flavour_R, 1},
    {"ahri_tre_map_sql_type_to_tre_R", (DL_FUNC)&ahri_tre_map_sql_type_to_tre_R, 1},
    {"ahri_tre_extract_table_from_sql_R", (DL_FUNC)&ahri_tre_extract_table_from_sql_R, 1},
    {"ahri_tre_parse_in_list_values_json_R", (DL_FUNC)&ahri_tre_parse_in_list_values_json_R, 1},
    {"ahri_tre_parse_check_constraint_values_json_R", (DL_FUNC)&ahri_tre_parse_check_constraint_values_json_R, 2},
    {"ahri_tre_map_redcap_value_type_R", (DL_FUNC)&ahri_tre_map_redcap_value_type_R, 2},
    {"ahri_tre_parse_redcap_choices_json_R", (DL_FUNC)&ahri_tre_parse_redcap_choices_json_R, 1},
    {"ahri_tre_strip_html_R", (DL_FUNC)&ahri_tre_strip_html_R, 1},
    {"ahri_tre_infer_label_from_field_name_R", (DL_FUNC)&ahri_tre_infer_label_from_field_name_R, 1},
    {"ahri_tre_get_redcap_choices_for_field_json_R", (DL_FUNC)&ahri_tre_get_redcap_choices_for_field_json_R, 2},
    {NULL, NULL, 0}
};

void R_init_AHRITREC(DllInfo *dll) {
    R_registerRoutines(dll, NULL, callMethods, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
