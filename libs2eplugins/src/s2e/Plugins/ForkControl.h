///
/// Copyright (C) 2010-2013, Dependable Systems Laboratory, EPFL
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

#ifndef S2E_PLUGINS_ForkControl_H
#define S2E_PLUGINS_ForkControl_H

#include <s2e/CorePlugin.h>
#include <s2e/Plugin.h>
#include <s2e/S2EExecutionState.h>

namespace s2e {
namespace plugins {

class ForkControl: public Plugin {
    S2E_PLUGIN
public:
    ForkControl(S2E *s2e) : Plugin(s2e) {
    }

    void initialize();

    void slotTranslateInstructionStart(ExecutionSignal *, S2EExecutionState *state, TranslationBlock *tb, uint64_t pc);
    void slotProgStart(S2EExecutionState *state, uint64_t pc);
    void onStateForkDecide(S2EExecutionState *state, bool *doFork);
    void slotSymbolicVariableCreation(S2EExecutionState *state, 
	    const std::string &name, 
	    const std::vector<klee::ref<klee::Expr>>& expr, 
	    const klee::ArrayPtr& ptr);
    void slotInitializationComplete(S2EExecutionState *state);
    void parseForkStartAddr(ConfigFile *cfg, const std::string &forkStartAddr);
    void slotStateKill(S2EExecutionState *state); 

private:
    bool m_hasSymData;
    bool m_progStart;
    std::map<uint64_t, int> m_forkStartAddr;
    int m_progStartAddr;
};

} // namespace plugins
} // namespace s2e

#endif // S2E_PLUGINS_ForkControl_H
