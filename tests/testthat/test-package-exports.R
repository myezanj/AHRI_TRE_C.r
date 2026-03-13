test_that("core exported functions exist", {
  expect_true(exists("tre_load", where = asNamespace("AHRITREC"), inherits = FALSE))
  expect_true(exists("version", where = asNamespace("AHRITREC"), inherits = FALSE))
})
