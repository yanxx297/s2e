///
/// Copyright (C) 2015-2020, Cyberhaven
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
///
#ifndef S2E_PLUGINS_SyscallReportCollector_H
#define S2E_PLUGINS_SyscallReportCollector_H

#include <llvm/ADT/DenseMap.h>

#include <s2e/CorePlugin.h>
#include <s2e/Plugin.h>
#include <s2e/Plugins/Core/Vmi.h>
#include <s2e/Plugins/OSMonitors/Support/ModuleMap.h>
#include <s2e/Plugins/OSMonitors/Support/ProcessExecutionDetector.h>
#include <s2e/S2EExecutionState.h>
#include "SimpleCFIChecker.h"

namespace s2e {
namespace plugins {

class WindowsMonitor;

class SyscallReportCollector : public Plugin {
    S2E_PLUGIN
public:
    SyscallReportCollector(S2E *s2e) : Plugin(s2e) {
    }

    void initialize();

private:
    ProcessExecutionDetector *m_detector;
    WindowsMonitor *m_monitor;
    Vmi *m_vmi;
    SimpleCFIChecker *m_cfi;
    ModuleMap *m_modules;

    bool m_filterByModule;
    std::string m_filterModuleName;

    llvm::DenseMap<uint64_t, unsigned> m_syscallCountStats;
    unsigned m_counter;
    bool m_changed;

    void onProcessLoad(S2EExecutionState *state, uint64_t pageDir, uint64_t pid, const std::string &ImageFileName);
    void onWindowsSyscall(S2EExecutionState *state, uint64_t pc, uint64_t syscallId, uint64_t stack);

    void onTimer();
    void reportSyscalls();
};

} // namespace plugins
} // namespace s2e

#endif // S2E_PLUGINS_SyscallReportCollector_H
