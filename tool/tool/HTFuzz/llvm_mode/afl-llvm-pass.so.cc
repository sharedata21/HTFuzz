/*
   american fuzzy lop - LLVM-mode instrumentation pass
   ---------------------------------------------------

   Written by Laszlo Szekeres <lszekeres@google.com> and
              Michal Zalewski <lcamtuf@google.com>

   LLVM integration design comes from Laszlo Szekeres. C bits copied-and-pasted
   from afl-as.c are Michal's fault.

   Copyright 2015, 2016 Google Inc. All rights reserved.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at:

     http://www.apache.org/licenses/LICENSE-2.0

   This library is plugged into LLVM when invoking clang through afl-clang-fast.
   It tells the compiler to add code roughly equivalent to the bits discussed
   in ../afl-as.h.

 */

#define AFL_LLVM_PASS

#include "../config.h"
#include "../debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <map>

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Debug.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

  class AFLCoverage : public ModulePass {

    public:

      static char ID;
      AFLCoverage() : ModulePass(ID) { }

      bool runOnModule(Module &M) override;

      // StringRef getPassName() const override {
      //  return "American Fuzzy Lop Instrumentation";
      // }

  };

}

std::vector<std::string> alloccall_routines = {
  "malloc", "valloc", "safe_malloc", "safemalloc", "safexmalloc",
  "realloc",
  "calloc", "memalign", "aligned_alloc", "safe_calloc", "safecalloc", "safexcalloc",
  "_Znwm", "_Znam", "_Znaj", "_Znwj",
};

std::vector<std::string> dealloccall_routines = {
  "free", "cfree", "safe_cfree", "safe_free", "safefree", "safexfree",
  "_ZdaPv", "_ZdlPv",
};

bool is_alloccall(std::string fn_name1){
  for(std::vector<std::string>::size_type i = 0; i < alloccall_routines.size(); i++){
    if(fn_name1.compare(0, alloccall_routines[i].size(), alloccall_routines[i]) == 0)
      return true;
  }
  return false;
}

bool is_dealloccall(std::string fn_name){
  for(std::vector<std::string>::size_type i = 0; i < dealloccall_routines.size(); i++){
    if(fn_name.compare(0, dealloccall_routines[i].size(), dealloccall_routines[i]) == 0)
      return true;
  }
  return false;
}

char AFLCoverage::ID = 0;


bool AFLCoverage::runOnModule(Module &M) {

  LLVMContext &C = M.getContext();

  IntegerType *Int8Ty  = IntegerType::getInt8Ty(C);
  //IntegerType *Int16Ty  = IntegerType::getInt16Ty(C);
  IntegerType *Int32Ty = IntegerType::getInt32Ty(C);

  /* Show a banner */

  char be_quiet = 0;

  if (isatty(2) && !getenv("AFL_QUIET")) {

    SAYF(cCYA "afl-llvm-pass " cBRI VERSION cRST " by <lszekeres@google.com>\n");

  } else be_quiet = 1;

  /* Decide instrumentation ratio */

  char* inst_ratio_str = getenv("AFL_INST_RATIO");
  unsigned int inst_ratio = 100;

  if (inst_ratio_str) {

    if (sscanf(inst_ratio_str, "%u", &inst_ratio) != 1 || !inst_ratio ||
        inst_ratio > 100)
      FATAL("Bad value of AFL_INST_RATIO (must be between 1 and 100)");

  }

  /* Get globals for the SHM region and the previous location. Note that
     __afl_prev_loc is thread-local. */

  GlobalVariable *AFLMapPtr =
      new GlobalVariable(M, PointerType::get(Int8Ty, 0), false,
                         GlobalValue::ExternalLinkage, 0, "__afl_area_ptr");

  //pointer ptr share memory
  GlobalVariable *AFLPointerPtr =
      new GlobalVariable(M, PointerType::get(Int32Ty, 0), false,
                         GlobalValue::ExternalLinkage, 0, "__afl_pointer_ptr");                     

  //seq ptr share memory
  GlobalVariable *AFLMemseqPtr =
      new GlobalVariable(M, PointerType::get(Int8Ty, 0), false,
                         GlobalValue::ExternalLinkage, 0, "__afl_memseq_ptr"); 

  GlobalVariable *AFLPrevLoc = new GlobalVariable(
      M, Int32Ty, false, GlobalValue::ExternalLinkage, 0, "__afl_prev_loc",
      0, GlobalVariable::GeneralDynamicTLSModel, 0, false);

  GlobalVariable *AFLMemPath = new GlobalVariable(
      M, Int32Ty, false, GlobalValue::ExternalLinkage, 0, "__afl_mem_path",
      0, GlobalVariable::GeneralDynamicTLSModel, 0, false);

  ConstantInt* SequenceMask = ConstantInt::get(Int32Ty, 0x00000007);

  /* Instrument all the things! */

  int inst_blocks = 0;

  for (auto &F : M)
    for (auto &BB : F) {
      int sequence_op = 0;
      int mem_read_cnt = 0;
      int mem_write_cnt= 0;  
      int elementptr_cnt = 0;


      BasicBlock::iterator IP = BB.getFirstInsertionPt();
      IRBuilder<> IRB(&(*IP));

      if (AFL_R(100) >= inst_ratio) continue;

      /* Make up cur_loc */

      unsigned int cur_loc = AFL_R(MAP_SIZE);

      // pointer count and mem instrument
      for (auto Inst = BB.begin(); Inst != BB.end(); Inst++) {
        Instruction &inst = *Inst;

        //outs() << "inst:" << inst << "\n";

        // filter the call llvm.*, most about -g debug information
        if(CallInst* call_inst = dyn_cast<CallInst>(&inst)) {
          Function* fn = call_inst->getCalledFunction();

          if(fn == NULL){
            Value *v = call_inst->getCalledValue();
            fn = dyn_cast<Function>(v->stripPointerCasts());
            if(fn == NULL)
              continue;
          }

          std::string fn_name1 = fn->getName();
          if(fn_name1.compare(0, 5, "llvm.") == 0)
            continue;

          //function args has pointer
          int opnt_cnt = inst.getNumOperands();
          Value *opnd;
          std::string o;
          std::string tmp_str;
          raw_string_ostream OS(tmp_str);

          for (int j = 0; j < opnt_cnt - 1; j++){
              opnd = inst.getOperand(j);
              opnd->printAsOperand(OS,false);
              if(strstr(OS.str().c_str(),"getelementptr"))
              {
                  elementptr_cnt++;
                  //outs() << "func_arg_pointer: " << OS.str() << "\n";
              }
          }
        }

        //instr has pointer
        std::string instr_OpcodeName = inst.getOpcodeName();
        if(instr_OpcodeName.compare(0, 13, "getelementptr") == 0 )
        {
          elementptr_cnt++;
          //outs() << "inst_pointer:" << inst << "\n";
        }

        if(inst.mayReadFromMemory()){
          mem_read_cnt++;
          //outs() << "read mem inst:" << inst << "\n";
        }

        if(inst.mayWriteToMemory()){
          mem_write_cnt++;
          //outs() << "write mem inst:" << inst << "\n";
        }
      }

      //outs() << "elementptr_cnt:" << elementptr_cnt << "\n";

      ConstantInt *CurLoc = ConstantInt::get(Int32Ty, cur_loc);

      /* Load prev_loc */

      LoadInst *PrevLoc = IRB.CreateLoad(AFLPrevLoc);
      PrevLoc->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));
      Value *PrevLocCasted = IRB.CreateZExt(PrevLoc, IRB.getInt32Ty());

      // Load mem_path 
      LoadInst *MemPathSeq = IRB.CreateLoad(AFLMemPath);
      MemPathSeq->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));
      Value *tmpMemPathSeqCasted = IRB.CreateZExt(MemPathSeq, IRB.getInt32Ty());

	    //mem_path and 0x0000FFFF
	    Value *MemPathSeqCasted = IRB.CreateAnd(tmpMemPathSeqCasted,SequenceMask);

      /* Load SHM pointer */
      LoadInst *MapPtr = IRB.CreateLoad(AFLMapPtr);
      MapPtr->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));

      if(mem_read_cnt > 0 || mem_write_cnt > 0 )
      {
        /* Load memseq SHM pointer */
        LoadInst *MemseqPtr = IRB.CreateLoad(AFLMemseqPtr);
        MemseqPtr->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));

        Value *tmpAfterXorPath = IRB.CreateXor(PrevLocCasted, MemPathSeqCasted);
        Value *MemseqPtrIdx =
            IRB.CreateGEP(MemseqPtr, IRB.CreateXor(tmpAfterXorPath, CurLoc));
        //Value *MemseqPtrIdx =
            //IRB.CreateGEP(MemseqPtr, IRB.CreateXor(PrevLocCasted, CurLoc));

        /* Update bitmap */
        LoadInst *Counter = IRB.CreateLoad(MemseqPtrIdx);
        Counter->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));
        //Value *Incr = IRB.CreateAdd(Counter, MemPathSeqCasted);
        Value *Incr = IRB.CreateAdd(Counter, ConstantInt::get(Int8Ty, 1));
        IRB.CreateStore(Incr, MemseqPtrIdx)
            ->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));
      }

      Value *MapPtrIdx;
      if(mem_read_cnt > 0 || mem_write_cnt > 0 )
      {  
        Value *tmpAfterXorPath = IRB.CreateXor(PrevLocCasted, MemPathSeqCasted);
        MapPtrIdx =
            IRB.CreateGEP(MapPtr, IRB.CreateXor(tmpAfterXorPath, CurLoc));
      }
      else
      {
        MapPtrIdx =
            IRB.CreateGEP(MapPtr, IRB.CreateXor(PrevLocCasted, CurLoc));
      }

      /* Update bitmap */

      LoadInst *Counter = IRB.CreateLoad(MapPtrIdx);
      Counter->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));
      Value *Incr = IRB.CreateAdd(Counter, ConstantInt::get(Int8Ty, 1));
      IRB.CreateStore(Incr, MapPtrIdx)
          ->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));

      /* Set prev_loc to cur_loc >> 1 */

      StoreInst *Store =
          IRB.CreateStore(ConstantInt::get(Int32Ty, cur_loc >> 1), AFLPrevLoc);
      Store->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));

      // add pointer amount
      LoadInst *PointerPtr = IRB.CreateLoad(AFLPointerPtr);
      PointerPtr->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));
      Value *PointerPtrIdx = IRB.CreateGEP(PointerPtr, ConstantInt::get(Int32Ty, 0));

      LoadInst *PCounter = IRB.CreateLoad(PointerPtrIdx);
      PCounter->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));

      Value *Incr1 = IRB.CreateAdd(PCounter, ConstantInt::get(Int32Ty, elementptr_cnt));

      IRB.CreateStore(Incr1, PointerPtrIdx)
          ->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));
          

      //find the mem func to instrument
      for (BasicBlock::iterator Inst = BB.begin(); Inst != BB.end(); Inst++) {
        sequence_op = 0 ;
        IRBuilder<> MemPathBuilder(&(*Inst));

        Instruction &inst = *Inst;

        if(CallInst* call_inst = dyn_cast<CallInst>(&inst)) {
          Function* fn = call_inst->getCalledFunction();

          if(fn == NULL){
            Value *v = call_inst->getCalledValue();
            fn = dyn_cast<Function>(v->stripPointerCasts());
            if(fn == NULL)
              continue;
          }

          std::string fn_name = fn->getName();

          if(fn_name.compare(0, 5, "llvm.") == 0)
            continue;

          //outs() << fn_name << " : ";
          if(is_alloccall(fn_name)){
            //load AFLMemPath - __afl_mem_path << 1  and  +1 ; store AFLMemPath

            sequence_op = 1;
            /* Load mem_path */
            LoadInst *MemPath = MemPathBuilder.CreateLoad(AFLMemPath);
            MemPath->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));
            Value *tmpMemPathCasted = MemPathBuilder.CreateZExt(MemPath, MemPathBuilder.getInt32Ty());
			
		        //mem_path and 0x0000FFFF
	          Value *MemPathCasted = MemPathBuilder.CreateAnd(tmpMemPathCasted,SequenceMask);

            /* Set mem_path to mem_path << 1 + 1 */
            Value *tmpAfterShl = MemPathBuilder.CreateShl(MemPathCasted, 1);
            Value *MemPathSeq = MemPathBuilder.CreateAdd(tmpAfterShl, ConstantInt::get(Int32Ty, sequence_op));

            StoreInst *Store =
                MemPathBuilder.CreateStore(MemPathSeq, AFLMemPath);
            Store->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));

          }
         
          if(is_dealloccall(fn_name)){
            //load AFLMemPath - __afl_mem_path << 1  and  +0 ; store AFLMemPath

            sequence_op = 0;
            // Load mem_path 
            LoadInst *MemPath = MemPathBuilder.CreateLoad(AFLMemPath);
            MemPath->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));
            Value *tmpMemPathCasted = MemPathBuilder.CreateZExt(MemPath, MemPathBuilder.getInt32Ty());
			
		        //mem_path and 0x0000FFFF
	          Value *MemPathCasted = MemPathBuilder.CreateAnd(tmpMemPathCasted,SequenceMask);

            // Set mem_path to mem_path << 1 + 0 
            Value *tmpAfterShl = MemPathBuilder.CreateShl(MemPathCasted, 1);
            Value *MemPathSeq = MemPathBuilder.CreateAdd(tmpAfterShl, ConstantInt::get(Int32Ty, sequence_op));

            StoreInst *Store =
                MemPathBuilder.CreateStore(MemPathSeq, AFLMemPath);
            Store->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));

          }
          
          // outs() << "\n";
        }
      }

      inst_blocks++;

    }

  /* Say something nice. */

  if (!be_quiet) {

    if (!inst_blocks) WARNF("No instrumentation targets found.");
    else OKF("Instrumented %u locations (%s mode, ratio %u%%).",
             inst_blocks, getenv("AFL_HARDEN") ? "hardened" :
             ((getenv("AFL_USE_ASAN") || getenv("AFL_USE_MSAN")) ?
              "ASAN/MSAN" : "non-hardened"), inst_ratio);

  }

  return true;

}


static void registerAFLPass(const PassManagerBuilder &,
                            legacy::PassManagerBase &PM) {

  PM.add(new AFLCoverage());

}


static RegisterStandardPasses RegisterAFLPass(
    PassManagerBuilder::EP_OptimizerLast, registerAFLPass);

static RegisterStandardPasses RegisterAFLPass0(
    PassManagerBuilder::EP_EnabledOnOptLevel0, registerAFLPass);
