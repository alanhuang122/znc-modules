away_notify
===========

away_notify is a global module that blindly offers clients the [away-notify](https://github.com/ircv3/ircv3-specifications/blob/master/extensions/away-notify-3.1) capability, while also requesting it from all IRC servers.

**In it's current form it should only be used on installations where all networks are known to support away-notify.**

Caveats
=

At present modules which provide capabilities to clients can only be global modules, which can limit the application for some providers.  Additionally, while having ZNC receive away-notify messages from servers is not a problem, during connection there is no way to know which network a client is connecting to, and thus no way to determine if away-notify is activated on the network and if it ought to be offered to the client.

Namely, the problem is thus: We may tell a client away-notify is available when it is not on the network, disabling it's normal `WHO` polling to determine if users are away.

These problems will be addressable properly once [cap-notify](https://github.com/ircv3/ircv3-specifications/blob/master/extensions/cap-notify-3.2.md) is finalized and adopted by IRCds.

Note
=
If there is a demand for it (or if I get time) I may find a way to have the module do it's own `WHO` polling (and also piggy-back on any connected client doing so anyway) to produce `AWAY` messages for clients which have it enabled on networks that do not.

away\_notify\_unsolicited
=======================

away\_notify\_unsolicited is a global module that requests the  [away-notify](https://github.com/ircv3/ircv3-specifications/blob/master/extensions/away-notify-3.1) capability from all IRC servers, and offers it unsolicited **after initial capability negotiation** to clients that connect to networks which have it enabled.

**This module should be used when it is expected or known that clients will behave in a useful manner to unsolicited `CAP LS` messages.**

Caveats
=
This version of the module attempts to work around above issues for away_notify, however this implementation relies no non-standard client behavior to be useful.  If a client does not enable capabilities after initial negotiation, or disables ones sent in later `CAP LS` that were previously enabled **this module should not be used**.