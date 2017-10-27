/*
 * Copyright (C) 2004-2014 ZNC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Derived from https://github.com/NuclearW/znc-modules/tree/master/privmsg_auto
 */

#include <znc/Client.h>
#include <znc/Modules.h>
#include <znc/IRCNetwork.h>

#include <boost/regex.hpp>

using std::vector;

class CPrivmsgAuto : public CModule {
	void StatusCommand(const CString& sLine) {
        if (m_pClient->IsCapEnabled("znc.in/self-message")) {
            PutModule("This client will not receive privmsgs because it has requested znc.in/self-message capabilities");
		} else {
			PutModule("This client receives prefixed privmsg");
		}
	}

public:
	MODCONSTRUCTOR(CPrivmsgAuto) {
		// Commands
		AddHelpCommand();
		AddCommand("Status", static_cast<CModCommand::ModCmdFunc>(&CPrivmsgAuto::StatusCommand));
	}

	virtual EModRet OnUserMsg(CString& sTarget, CString& sMessage) {
		if (m_pNetwork && m_pNetwork->GetIRCSock() && !m_pNetwork->IsChan(sTarget)) {
			vector<CClient*> vClients = m_pNetwork->GetClients();
			for (std::vector<CClient*>::size_type i = 0; i != vClients.size(); i++) {
				if (vClients[i] != m_pClient) {
                    if (!vClients[i]->IsCapEnabled("znc.in/self-message")) {
                        m_pNetwork->PutUser(":" + sTarget + "!prefix@privmsg.znc.in PRIVMSG " + m_pNetwork->GetIRCNick().GetNick() + " :-> " + sMessage, vClients[i], NULL);
                    }
				}
			}
		}

		return CONTINUE;
	}

	virtual EModRet OnUserAction(CString& sTarget, CString& sMessage) {
		if (m_pNetwork && m_pNetwork->GetIRCSock() && !m_pNetwork->IsChan(sTarget)) {
			vector<CClient*> vClients = m_pNetwork->GetClients();
			for (std::vector<CClient*>::size_type i = 0; i != vClients.size(); i++) {
				if (vClients[i] != m_pClient) {
                    if (!vClients[i]->IsCapEnabled("znc.in/self-message")) {
                        m_pNetwork->PutUser(":" + sTarget + "!prefix@privmsg.znc.in PRIVMSG " + m_pNetwork->GetIRCNick().GetNick() + " :\x01" + "ACTION -> " + sMessage + "\x01", vClients[i], NULL);
                    }
				}
			}
		}

		return CONTINUE;
	}

template<> void TModInfo<CPrivmsgAuto>(CModInfo& Info) {
	Info.SetWikiPage("privmsg_auto");
	Info.SetHasArgs(false);
}

NETWORKMODULEDEFS(CPrivmsgAuto, "Automatically switches between privmsg and privmsg_prefix behavior")
