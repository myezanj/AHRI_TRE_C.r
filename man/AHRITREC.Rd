\name{AHRITREC}
\alias{ahri_tre_load}
\alias{ahri_tre_version}
\alias{ahri_tre_sha256_file_hex}
\alias{ahri_tre_verify_sha256_file}
\alias{ahri_tre_parse_flavour}
\alias{ahri_tre_map_sql_type_to_tre}
\alias{ahri_tre_extract_table_from_sql}
\alias{ahri_tre_parse_in_list_values_json}
\alias{ahri_tre_parse_check_constraint_values_json}
\alias{ahri_tre_map_redcap_value_type}
\alias{ahri_tre_parse_redcap_choices_json}
\alias{ahri_tre_strip_html}
\alias{ahri_tre_infer_label_from_field_name}
\alias{ahri_tre_get_redcap_choices_for_field_json}
\title{AHRI TRE C Core R Bindings}
\description{
Wrapper functions for loading and calling the AHRI TRE C core from R.
}
\usage{
ahri_tre_load(core_path = Sys.getenv("AHRI_TRE_C_LIB", unset = ""))

ahri_tre_version()

ahri_tre_sha256_file_hex(path)

ahri_tre_verify_sha256_file(path, expected_hex)

ahri_tre_parse_flavour(flavour)

ahri_tre_map_sql_type_to_tre(sql_type)

ahri_tre_extract_table_from_sql(sql)

ahri_tre_parse_in_list_values_json(values)

ahri_tre_parse_check_constraint_values_json(constraint_def, column_name)

ahri_tre_map_redcap_value_type(field_type, validation = NULL)

ahri_tre_parse_redcap_choices_json(choices)

ahri_tre_strip_html(text)

ahri_tre_infer_label_from_field_name(field_name)

ahri_tre_get_redcap_choices_for_field_json(field_type, choices = NULL)
}
\arguments{
  \item{core_path}{Optional absolute path to the AHRI TRE C shared library.}

  \item{path}{Path to a file.}

  \item{expected_hex}{Expected SHA-256 digest in hexadecimal format.}

  \item{flavour}{Database flavour string to parse.}

  \item{sql_type}{SQL type string to map.}

  \item{sql}{SQL statement to inspect.}

  \item{values}{Text containing values for IN-list style parsing.}

  \item{constraint_def}{Constraint definition string.}

  \item{column_name}{Column name used while parsing constraint values.}

  \item{field_type}{REDCap field type string.}

  \item{validation}{Optional REDCap field validation rule.}

  \item{choices}{Choice string for REDCap parsing helpers.}

  \item{text}{Text string that may contain HTML markup.}

  \item{field_name}{Field name used to infer a human-readable label.}
}
\value{
Function return values are provided by the AHRI TRE C core API and include
character strings, logical values, integers, JSON text, and small named lists.
}
\details{
\code{ahri_tre_load()} attempts to locate the core shared library using
\code{AHRI_TRE_C_LIB}, \code{AHRI_TRE_C_ROOT}, and common sibling build paths.
}
\examples{
\dontrun{
Sys.setenv(AHRI_TRE_C_ROOT = "C:/path/to/AHRI_TRE.C")
library(AHRITREC)
ahri_tre_load()
ahri_tre_version()
}
}
