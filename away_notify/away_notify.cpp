/*
 * Copyright (C) 2004-2014 ZNC, see the NOTICE file for details.
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
 */

#include <znc/Client.h>
#include <znc/Modules.h>
#include <znc/IRCNetwork.h>
#include <znc/IRCSock.h>

using std::vector;

const char* m_sCapability = "away-notify";

class CAwayNotify : public CModule {
public:
	MODCONSTRUCTOR(CAwayNotify) {}

	virtual ~CAwayNotify() {}

	// Advertise that we can handle away-notify to the server
	virtual bool OnServerCapAvailable(const CString& sCap) {
		return sCap.Equals(m_sCapability);
	}

	// Later with cap-notify this may be relevant to us, but for now we can't do anything with it.
	virtual void OnServerCapResult(const CString& sCap, bool bSuccess) {}

	// At the moment we can only blindly offer the capability
	virtual void OnClientCapLs(CClient* pClient, SCString &ssCaps) {
		ssCaps.insert(m_sCapability);
	}

	// Since we later check on every incoming away-notify message if the clients connected will get it, we don't care about this.
	virtual void OnClientCapRequest(CClient* pClient, const CString& sCap, bool bState) {}

	// We sell away-notify and away-notify accessories
	virtual bool IsClientCapSupported(CClient* pClient, const CString& sCap, bool bState) {
		return sCap.Equals(m_sCapability);
	}

	virtual EModRet OnRaw(CString& sLine) {
		CString cmd = sLine.Token(1, false);
		if (cmd.Equals("AWAY")) {
			if (m_pNetwork && m_pNetwork->GetIRCSock() && m_pNetwork->GetIRCSock()->IsCapAccepted(m_sCapability)) {
				vector<CClient*> vClients = m_pNetwork->GetClients();
				for (std::vector<CClient*>::size_type i = 0; i != vClients.size(); i++) {
					if (vClients[i] != m_pClient && vClients[i]->IsCapEnabled(m_sCapability)) {
						m_pNetwork->PutUser(sLine, vClients[i], NULL);
					}
				}
			}
			return HALTCORE;
		}
		return CONTINUE;
	}
};

template<> void TModInfo<CAwayNotify>(CModInfo& Info) {
	Info.SetWikiPage("away_notify");
	Info.SetHasArgs(false);
}

GLOBALMODULEDEFS(CAwayNotify, "Provides IRCv3.1 away-notify")
