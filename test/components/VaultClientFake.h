#ifndef CCONFD_VAULTCLIENTFAKE_H
#define CCONFD_VAULTCLIENTFAKE_H

#include "components/VaultClientHttp.h"

fruit::Component<VaultClientFactory>
getVaultClientFakeComponent();

#endif // CCONFD_VAULTCLIENTFAKE_H