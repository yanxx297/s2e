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

#ifndef S2E_PLUGINS_EXAMPLE_H
#define S2E_PLUGINS_EXAMPLE_H

#include <s2e/CorePlugin.h>
#include <s2e/Plugin.h>
#include <s2e/S2EExecutionState.h>

namespace s2e {
namespace plugins {

class InstructionTracer : public Plugin {
    S2E_PLUGIN
public:
    InstructionTracer(S2E *s2e) : Plugin(s2e) {
    }

    void initialize();

    void slotTranslateInstructionStart(ExecutionSignal *, S2EExecutionState *state, TranslationBlock *tb, uint64_t pc);

    void slotExecuteInstructionStart(S2EExecutionState *state, uint64_t pc);

private:
    bool parseIgnoreRange(ConfigFile *cfg, const std::string &traceIgnore);
    bool parseEnableRange(ConfigFile *cfg, const std::string &traceEnable);
    bool isIgnored(uint64_t pc);
    bool isEnabled(uint64_t pc);
    bool m_traceInstructionTranslation;
    bool m_traceInstructionExecution;
    int m_traceStartAddr;
    int m_traceEndAddr;
    bool m_traceOn;
    std::vector<std::pair <uint64_t, uint64_t>>  m_traceIgnore;
    std::vector<std::pair <uint64_t, uint64_t>>  m_traceEnable;
};

} // namespace plugins
} // namespace s2e

#endif // S2E_PLUGINS_EXAMPLE_H
