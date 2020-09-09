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

#include "InstructionTracer.h"

namespace s2e {
namespace plugins {

S2E_DEFINE_PLUGIN(InstructionTracer, "Trace instruction execution", "", );

void InstructionTracer::initialize() {
    m_traceStartAddr = s2e()->getConfig()->getInt(getConfigKey() + ".traceStartAddr", -1);
    m_traceEndAddr = s2e()->getConfig()->getInt(getConfigKey() + ".traceEndAddr", -1);
    if (m_traceStartAddr == -1)
	m_traceOn = true;
    else m_traceOn = false; 

    m_traceInstructionExecution = s2e()->getConfig()->getBool(getConfigKey() + ".traceInstructionExecution");
    m_traceInstructionTranslation = s2e()->getConfig()->getBool(getConfigKey() + ".traceInstructionTranslation");

    if(!parseIgnoreRange(s2e()->getConfig(), getConfigKey() + ".traceIgnore"))
	exit(-1);

    if(!parseEnableRange(s2e()->getConfig(), getConfigKey() + ".traceEnable"))
	exit(-1);

    s2e()->getCorePlugin()->onTranslateInstructionStart.connect(sigc::mem_fun(*this, &InstructionTracer::slotTranslateInstructionStart));
}

bool InstructionTracer::parseIgnoreRange(ConfigFile *cfg, const std::string &traceIgnore){
    ConfigFile::integer_list l = cfg->getIntegerList(traceIgnore);
    if(l.size() % 2 != 0)
	return false;
    for(int i = 0; i < l.size(); i += 2){
	std::pair <uint64_t, uint64_t> p (l[i], l[i+1]);
	m_traceIgnore.push_back(p);
    }
    return true;
}

bool InstructionTracer::parseEnableRange(ConfigFile *cfg, const std::string &traceEnable){
    ConfigFile::integer_list l = cfg->getIntegerList(traceEnable);
    if(l.size() % 2 != 0)
	return false;
    for(int i = 0; i < l.size(); i += 2){
	std::pair <uint64_t, uint64_t> p (l[i], l[i+1]);
	m_traceEnable.push_back(p);
    }
    return true;
}

void InstructionTracer::slotTranslateInstructionStart(ExecutionSignal *signal, S2EExecutionState *state, TranslationBlock *tb,
                                      uint64_t pc) {
    if(pc == m_traceStartAddr){
	getDebugStream(state) << "Turn on Insntruction Tracing at " << hexval(pc) << "\n"; 
	m_traceOn = true;}
    else if(pc == m_traceEndAddr)
	m_traceOn = false;

    if (m_traceInstructionTranslation && m_traceOn) {
        getDebugStream(state) << "Translating instruction at " << hexval(pc) << "\n";
    }

    if (m_traceInstructionExecution) {
        signal->connect(sigc::mem_fun(*this, &InstructionTracer::slotExecuteInstructionStart));
    }
}

bool InstructionTracer::isIgnored(uint64_t pc){
    for(auto r: m_traceIgnore){
	if(pc >= r.first && pc <= r.second)
	    return true;
    }
    return false;
}

bool InstructionTracer::isEnabled(uint64_t pc){
    for(auto r: m_traceEnable){
	if(pc >= r.first && pc <= r.second)
	    return true;
    }
    return false;
}

void InstructionTracer::slotExecuteInstructionStart(S2EExecutionState *state, uint64_t pc) {
    if(m_traceOn && !isIgnored(pc) && isEnabled(pc))
	getDebugStream(state) << "Executing Instruction at " << hexval(pc) << "\n";
}

} // namespace plugins
} // namespace s2e
