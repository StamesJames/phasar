/******************************************************************************
 * Copyright (c) 2020 Philipp Schubert.
 * All rights reserved. This program and the accompanying materials are made
 * available under the terms of LICENSE.txt.
 *
 * Contributors:
 *     Philipp Schubert and others
 *****************************************************************************/

#include "phasar/PhasarLLVM/DataFlowSolver/Mono/Problems/IntraMonoUninitVariables.h"
#include "phasar/Config/Configuration.h"
#include "phasar/DB/LLVMProjectIRDB.h"
#include "phasar/PhasarLLVM/ControlFlow/LLVMBasedCFG.h"
#include "phasar/PhasarLLVM/DataFlowSolver/Mono/Solver/IntraMonoSolver.h"
#include "phasar/PhasarLLVM/Passes/ValueAnnotationPass.h"
#include "phasar/PhasarLLVM/Pointer/LLVMPointsToSet.h"
#include "phasar/PhasarLLVM/TypeHierarchy/LLVMTypeHierarchy.h"
#include "phasar/PhasarLLVM/Utils/LLVMShorthands.h"
#include "phasar/Utils/Logger.h"

#include "gtest/gtest.h"

#include "llvm/Support/raw_ostream.h"

#include <set>
#include <string>
#include <utility>

#include "TestConfig.h"

using namespace psr;

/* ============== TEST FIXTURE ============== */
class IntraMonoUninitVariablesTest : public ::testing::Test {
protected:
  const std::string PathToLLFiles =
      unittest::PathToLLTestFiles + "/uninitialized_variables/";

  using CompactResults_t = std::set<std::pair<size_t, std::set<std::string>>>;

  const std::vector<std::string> EntryPoints = {"main"};

  LLVMProjectIRDB *IRDB = nullptr;

  void SetUp() override {}

  void TearDown() override { delete IRDB; }

  void doAnalysisAndCompareResults(llvm::StringRef LlvmFilePath,
                                   const CompactResults_t & /*GroundTruth*/,
                                   bool PrintDump = false) {
    IRDB = new LLVMProjectIRDB(PathToLLFiles + LlvmFilePath);
    if (PrintDump) {
      IRDB->dump();
    }
    ValueAnnotationPass::resetValueID();
    LLVMTypeHierarchy TH(*IRDB);
    auto PT = LLVMPointsToSet(*IRDB);
    LLVMBasedCFG CFG;
    IntraMonoUninitVariables Uninit(IRDB, &TH, &CFG, &PT, EntryPoints);
    IntraMonoSolver_P<IntraMonoUninitVariables> Solver(Uninit);
    Solver.solve();
    if (PrintDump) {
      Solver.dumpResults();
    }
    // for (auto result :
    //      TaintSolver.getResultsAt(IRDB->getInstruction(InstId)).getAsSet()) {
    //   FoundResults.insert(getMetaDataID(result));
    // }
    // EXPECT_EQ(FoundResults, GroundTruth);
  }

}; // Test Fixture

// TEST_F(IntraMonoUninitVariablesTest, Basic_01) {
//   CompactResults_t GroundTruth;
//   doAnalysisAndCompareResults("basic_01_cpp.ll", GroundTruth, true);
// }

TEST_F(IntraMonoUninitVariablesTest, Basic_02) {
  CompactResults_t GroundTruth;
  GroundTruth.insert({13, {"%b"}});
  GroundTruth.insert({15, {"%a"}});
  GroundTruth.insert({17, {"%a", "%b"}});
  doAnalysisAndCompareResults("basic_02_cpp.ll", GroundTruth, true);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
