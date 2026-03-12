test_that("core exported functions exist", {
  expect_true(exists("ahri_tre_load", where = asNamespace("AHRITREC"), inherits = FALSE))
  expect_true(exists("ahri_tre_version", where = asNamespace("AHRITREC"), inherits = FALSE))
})
