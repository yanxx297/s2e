///
/// Copyright (C) 2014-2020, Cyberhaven
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

#ifndef SIMPLECFIREPORTCOLLECTOR

#define SIMPLECFIREPORTCOLLECTOR

#include <s2e/s2e.h>

typedef enum S2E_CFI_COMMAND
{
    DONE,
    FYI,
    TAKE_SCREENSHOT,
    WINDOW_TEXT,
} S2E_CFI_COMMAND;

typedef struct S2E_CFI
{
    S2E_CFI_COMMAND command;
    UINT64 ptr_info; //pointer to the info string
} S2E_CFI;

//XXX: This must be sent to the screenshot plugin
static VOID TakeScreenShot()
{
    S2E_CFI cmd;
    //instruct S2E to take screenshot
    cmd.command = TAKE_SCREENSHOT;
    S2EMessage("Tickler: taking screenshot\n");
    S2EInvokePlugin("SimpleCFIChecker", &cmd, sizeof(cmd));
}

typedef enum S2E_CFI_REPORT_COLLECTOR_COMMANDS
{
    CPU_USAGE,
    AUTOSCROLL_DONE,
    MAIN_WINDOW_OPEN
} S2E_CFI_REPORT_COLLECTOR_COMMANDS;

typedef struct S2E_CFI_RC_CPU
{
    UINT32 TotalCpuUsage;
    UINT32 ProgramCpuUsage;
} S2E_CFI_RC_CPU;

typedef struct S2E_CFI_REPORT_COLLECTOR_COMMAND
{
    S2E_CFI_REPORT_COLLECTOR_COMMANDS Command;
    S2E_CFI_RC_CPU CpuUsage;
} S2E_CFI_REPORT_COLLECTOR_COMMAND;


static void S2EReportCpuUsage(UINT32 Total, UINT32 Program)
{
    S2E_CFI_REPORT_COLLECTOR_COMMAND Cmd;
    Cmd.Command = CPU_USAGE;
    Cmd.CpuUsage.TotalCpuUsage = Total;
    Cmd.CpuUsage.ProgramCpuUsage = Program;
    S2EInvokePlugin("SimpleCFIReportCollector", &Cmd, sizeof(Cmd));
}

static void S2ENotifyAutoscrollDone(VOID)
{
    S2E_CFI_REPORT_COLLECTOR_COMMAND Cmd;
    Cmd.Command = AUTOSCROLL_DONE;
    S2EInvokePlugin("SimpleCFIReportCollector", &Cmd, sizeof(Cmd));
}

static void S2ENotifyMainWindowOpen(VOID)
{
    S2E_CFI_REPORT_COLLECTOR_COMMAND Cmd;
    Cmd.Command = MAIN_WINDOW_OPEN;
    S2EInvokePlugin("SimpleCFIReportCollector", &Cmd, sizeof(Cmd));
}

#endif
