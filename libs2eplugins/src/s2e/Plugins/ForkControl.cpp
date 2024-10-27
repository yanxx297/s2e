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

class ForkControlState: public PluginState {
    private: 
	std::map<uint64_t, int> m_forkStartAddrCnt;

    public:
	ForkControlState(){}

	virtual ~ForkControlState(){}

	static PluginState *factory(Plugin*, S2EExecutionState*){
	    return new ForkControlState();
	}

	ForkControlState *clone() const {
	    return new ForkControlState(*this);
	}

	void setCounter(ConfigFile::integer_list l){
	    if(l.size() % 2 != 0)
		exit(-1);
	    for(int i = 0; i < l.size(); i += 2){
		m_forkStartAddrCnt[l[i]] = 0;
	    }
	}

	bool hitForkStartAddr(uint64_t pc, int target){
	    if(m_forkStartAddrCnt.find(pc) == m_forkStartAddrCnt.end())
		return false;
	    m_forkStartAddrCnt[pc] ++;
	    if (m_forkStartAddrCnt[pc] >= target){
		return true;
	    }
	    return false;
	}

	int getCount(uint64_t pc){
	    return m_forkStartAddrCnt[pc];
	}

	bool isForkStartAddr(uint64_t pc){
	    return (m_forkStartAddrCnt.find(pc) != m_forkStartAddrCnt.end());
	}

	void resetForkStartAddr(){
	    for(std::map<uint64_t, int>::iterator it = m_forkStartAddrCnt.begin(); it != m_forkStartAddrCnt.end(); ++it)
		it->second = 0;
	}
};

void ForkControl::initialize() {
    m_progStartAddr = s2e()->getConfig()->getInt(getConfigKey() + ".progStartAddr");
    m_hasSymData = false;
    m_progStart = false;

    parseForkStartAddr(s2e()->getConfig(), getConfigKey() + ".forkStartAddr");

    s2e()->getCorePlugin()->onTranslateInstructionStart.connect(sigc::mem_fun(*this, &ForkControl::slotTranslateInstructionStart));
    s2e()->getCorePlugin()->onSymbolicVariableCreation.connect(sigc::mem_fun(*this, &ForkControl::slotSymbolicVariableCreation));
    s2e()->getCorePlugin()->onInitializationComplete.connect(sigc::mem_fun(*this, &ForkControl::slotInitializationComplete));
    s2e()->getCorePlugin()->onStateKill.connect(sigc::mem_fun(*this, &ForkControl::slotStateKill));
    s2e()->getCorePlugin()->onStateForkDecide.connect(sigc::mem_fun(*this, &ForkControl::onStateForkDecide));
}

void ForkControl::parseForkStartAddr(ConfigFile *cfg, const std::string &forkStartAddr){
    ConfigFile::integer_list l = cfg->getIntegerList(forkStartAddr);
    if(l.size() % 2 != 0)
	exit(-1);
    for(int i = 0; i < l.size(); i += 2){
	m_forkStartAddr[l[i]] = l[i+1];
    }
} 

void ForkControl::slotInitializationComplete(S2EExecutionState *state){
    DECLARE_PLUGINSTATE(ForkControlState, state);
    ConfigFile::integer_list l = s2e()->getConfig()->getIntegerList(getConfigKey() + ".forkStartAddr");
    plgState->setCounter(l);
}

void ForkControl::slotSymbolicVariableCreation(S2EExecutionState *state, 
	const std::string &name, 
	const std::vector<klee::ref<klee::Expr>> &expr, 
	const klee::ArrayPtr &ptr){
    if(!m_hasSymData){
	m_hasSymData = true;
	state->disableForking();
    }
}

void ForkControl::onStateForkDecide(S2EExecutionState *state, 
	const klee::ref<klee::Expr> &condition, bool &allowForking) {
    uint64_t pc = state->regs()->getPc();
    DECLARE_PLUGINSTATE(ForkControlState, state);
    if(m_progStart && plgState->isForkStartAddr(pc)){
	getDebugStream(state) << "hit "<<hexval(pc)<<" by "<<plgState->getCount(pc)+1<<"\n";
	if(plgState->hitForkStartAddr(pc, m_forkStartAddr[pc])){
	    getDebugStream(state) << "Enable forking at " << hexval(pc) << "\n";
	    state->enableForking();
	}
    }
}

void ForkControl::slotTranslateInstructionStart(ExecutionSignal *signal, S2EExecutionState *state, TranslationBlock *tb,
                                      uint64_t pc) {
    if(pc == m_progStartAddr){
	signal->connect(sigc::mem_fun(*this, &ForkControl::slotProgStart));
    }
}

void ForkControl::slotProgStart(S2EExecutionState *state, uint64_t pc){
    if (pc == m_progStartAddr){
	getDebugStream(state)<<"Program start at "<<hexval(pc)<<"\n";
	m_progStart = true;
    }
}

void ForkControl::slotStateKill(S2EExecutionState *state) {
    DECLARE_PLUGINSTATE(ForkControlState, state);
    plgState->resetForkStartAddr();
}

} // namespace plugins
} // namespace s2e
