// Copyright (c) 2010-2021, Lawrence Livermore National Security, LLC. Produced
// at the Lawrence Livermore National Laboratory. All Rights reserved. See files
// LICENSE and NOTICE for details. LLNL-CODE-806117.
//
// This file is part of the MFEM library. For more information and source code
// availability visit https://mfem.org.
//
// MFEM is free software; you can redistribute it and/or modify it under the
// terms of the BSD-3 license. We welcome feedback and contributions, see file
// CONTRIBUTING.md for details.

#define CATCH_CONFIG_RUNNER
#include "mfem.hpp"
#include "unit_tests.hpp"

bool launch_all_non_regression_tests = false;

#ifdef MFEM_USE_MPI
mfem::MPI_Session *GlobalMPISession;
#else
#error "This test should be disabled without MFEM_USE_MPI!"
#endif

int main(int argc, char *argv[])
{
   // There must be exactly one instance.
   Catch::Session session;

   // Build a new command line parser on top of Catch's
   using namespace Catch::clara;
   auto cli = session.cli() |
              Opt(launch_all_non_regression_tests) ["--all"] ("all tests");
   session.cli(cli);

   // For floating point comparisons, print 8 digits for single precision
   // values, and 16 digits for double precision values.
   Catch::StringMaker<float>::precision = 8;
   Catch::StringMaker<double>::precision = 16;

   // Apply provided command line arguments.
   int r = session.applyCommandLine(argc, argv);
   if (r != 0) { return r; }

#ifdef MFEM_USE_MPI
   mfem::MPI_Session mpi;
   GlobalMPISession = &mpi;

   // Exclude all tests that are not labeled with Parallel.
   auto cfg = session.configData();
   cfg.testsOrTags.push_back("[Parallel]");
   session.useConfigData(cfg);

   // NOTE: tests marked with "[CUDA]" (in addition to "[Parallel]") are still
   //       run with the default device.

   if (mpi.Root())
   {
      std::cout << "INFO: Test filter: [Parallel]" << std::endl;
   }
#endif

   int result = session.run();

   return result;
}
