//===- Hello.cpp - Example code from "Writing an LLVM Pass" ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements two versions of the LLVM "Hello World" pass described
// in docs/WritingAnLLVMPass.html
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/User.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/Support/raw_ostream.h"
#include "../RangeAnalysis/RangeAnalysis.h"
#include "llvm/IR/LLVMContext.h" 
#include "llvm/IR/Metadata.h"
#include "llvm/IR/InstIterator.h"
using namespace llvm;

#define DEBUG_TYPE "hello"

STATISTIC(HelloCounter, "Counts number of functions greeted");

namespace {
  // Hello - The first implementation, without getAnalysisUsage.
  struct Hello : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    Hello() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      ++HelloCounter;
      errs() << "Hello: ";
      errs().write_escaped(F.getName()) << '\n';
      return false;
    }
  };
}

char Hello::ID = 0;
static RegisterPass<Hello> X("hello", "Hello World Pass");

namespace {
  // Hello2 - The second implementation with getAnalysisUsage implemented.
  struct Hello2 : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    Hello2() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      ++HelloCounter;
      errs() << "Hello: ";
      errs().write_escaped(F.getName()) << '\n';
      return false;
    }

    // We don't modify the program, so we preserve all analyses.
    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesAll();
    }
  };
}

char Hello2::ID = 0;
static RegisterPass<Hello2>
Y("hello2", "Hello World Pass (with getAnalysisUsage implemented)");


namespace {
  struct VarRangeToolPass : public FunctionPass{
    static char ID;
    VarRangeToolPass():FunctionPass(ID){}
    bool runOnFunction(Function &F) override {
      errs() << "VarRangeToolPass: ";
      errs().write_escaped("F.") << '\n';
      //IntraProceduralRA<int> &ra = Pass::getAnalysis<IntraProceduralRA<int> >();
      errs() << "\nCousot Intra Procedural analysis (Values =-> Ranges) of "<< F.getName() << ":\n";
/*        for(Function::iterator bb =F.begin(), bbEnd = F.end(); bb != bbEnd; ++bb){
            for(BasicBlock::iterator I = bb->begin(), IEnd = bb->end(); I != IEnd; ++I){
                const Value *v = &(*I);
                Range r = ra.getRange(v);
                if(!r.isUnknown()){
                    r.print(errs());
                    I->dump();
                }

            }
        }

*/
      IntraProceduralRA<Cousot> &ra = getAnalysis<IntraProceduralRA<Cousot> >(F);
      for(inst_iterator I = inst_begin(F),E = inst_end(F);I!=E;++I){
          Instruction & Inst = *I;
          LLVMContext& context = Inst.getContext();
          MDNode* N =MDNode::get(context,MDString::get(context,"My String Context"));
          StringRef s = cast<MDString>(Inst.getMetadata("my.md.name")->getOperand(0))->getString();
          errs()<<*I<<"  :md string :"<<s;
          const Value *v = &(*I);
          Range r = ra.getRange(v);
          if(!r.isUnknown()){
              r.print(errs());
              I->dump();
          }
      }


//stackoverflow.com

//LLVMContext& context = F->getContext();
//llvm::LLVMContext& context = llvm::getGlobalContext();

//BasicBlock *LoopBB = BasicBlock::Create(TheContext, "loop", TheFunction);
//PHINode *PN = Builder.CreatePHI(Type::getDoubleTy(TheContext), 2, "iftmp");
//StepVal = ConstantFP::get(TheContext, APFloat(1.0));
//Constant::getNullValue(Type::getDoubleTy(TheContext));
//C.setMetadata("My String Context");
////Metadata md = new Metadata("F.",Metadata::StorageType::Temporary);
//md.MDStringKind
//int mdid = context.getMDKindID("my");

////unsigned mdid = Metadata::getMetadataID();
//cast<MDString>(Inst->getMetadata("my.md.name")->getOPerand(0))->getString();

      return false;
    }
    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesAll();
      AU.addRequired<IntraProceduralRA<Cousot> >();
      //AU.addRequired<SDG>();
      //AU.addRequired< InterProceduralRA<Cousot> >();
    }

  };		
}

char VarRangeToolPass::ID = 0;
static RegisterPass<VarRangeToolPass> VarRangeTool("VarRangeToolPass", "Calculate Variable Range Pass");
