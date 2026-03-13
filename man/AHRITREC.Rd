\name{AHRITREC}
\alias{tre_load}
\alias{version}
\alias{sha256_file_hex}
\alias{verify_sha256_file}
\alias{parse_flavour}
\alias{map_sql_type_to_tre}
\alias{extract_table_from_sql}
\alias{parse_in_list_values_json}
\alias{parse_in_list_values}
\alias{parse_check_constraint_values_json}
\alias{parse_check_constraint_values}
\alias{map_value_type}
\alias{parse_redcap_choices_json}
\alias{parse_redcap_choices}
\alias{strip_html}
\alias{infer_label_from_field_name}
\alias{get_redcap_choices_for_field_json}
\alias{get_redcap_choices_for_field}
\title{AHRI TRE C Core R Bindings}
\description{
Wrapper functions for loading and calling the AHRI TRE C core from R.
}
\usage{
tre_load(core_path = Sys.getenv("AHRI_TRE_C_LIB", unset = ""))

version()

sha256_file_hex(path)

verify_sha256_file(path, expected_hex)

parse_flavour(flavour)

map_sql_type_to_tre(sql_type)

extract_table_from_sql(sql)

parse_in_list_values_json(values)
parse_in_list_values(values)

parse_check_constraint_values_json(constraint_def, column_name)
parse_check_constraint_values(constraint_def, column_name)

map_value_type(field_type, validation = NULL)

parse_redcap_choices_json(choices)
parse_redcap_choices(choices)

strip_html(text)

infer_label_from_field_name(field_name)

get_redcap_choices_for_field_json(field_type, choices = NULL)
get_redcap_choices_for_field(field_type, choices = NULL)
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
Unprefixed function names are canonical and match the Julia-facing C API naming.
The C bridge keeps internal symbol fallback support for prefixed dependency builds.
}
\examples{
\dontrun{
library(AHRITREC)
tre_load()
version()
}
}
