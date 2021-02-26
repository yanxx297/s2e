///
/// Copyright (C) 2010-2015, Dependable Systems Laboratory, EPFL
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///

#include <s2e/ConfigFile.h>
#include <s2e/S2E.h>
#include <s2e/Utils.h>

#include "ForkControl.h"

namespace s2e {
namespace plugins {

S2E_DEFINE_PLUGIN(ForkControl, "Turn forking on/off at eip", "", );

void ForkControl::initialize() {
    m_traceInstruction = s2e()->getConfig()->getBool(getConfigKey() + ".traceInstruction");
    m_forkStartAddr = s2e()->getConfig()->getInt(getConfigKey() + ".forkStartAddr");
    m_progStartAddr = s2e()->getConfig()->getInt(getConfigKey() + ".progStartAddr");
    m_hasSymData = false;

    s2e()->getCorePlugin()->onTranslateInstructionStart.connect(sigc::mem_fun(*this, &ForkControl::slotTranslateInstructionStart));
    s2e()->getCorePlugin()->onSymbolicVariableCreation.connect(sigc::mem_fun(*this, &ForkControl::slotSymbolicVariableCreation));
}

void ForkControl::slotSymbolicVariableCreation(S2EExecutionState *state, 
	const std::string &name, 
	const std::vector<klee::ref<klee::Expr>> &expr, 
	const klee::ArrayPtr &ptr){
    if(!m_hasSymData){
	m_hasSymData = true;
    }
}

void ForkControl::slotTranslateInstructionStart(ExecutionSignal *signal, S2EExecutionState *state, TranslationBlock *tb,
                                      uint64_t pc) {
    if (m_traceInstruction) {
	getDebugStream(state) << "TranslateBlockStart at " << hexval(pc) << "\n";
    }
    if (pc == m_forkStartAddr || pc == m_progStartAddr) {
        signal->connect(sigc::mem_fun(*this, &ForkControl::slotExecuteInstructionStart));
    }
}

void ForkControl::slotExecuteInstructionStart(S2EExecutionState *state, uint64_t pc) {
    if (pc == m_forkStartAddr && m_hasSymData){
	state->enableForking();
    }
    else if (pc == m_progStartAddr){
	state->disableForking();
    }
}

} // namespace plugins
} // namespace s2e
