#include "VaultClientFake.h"
#include "HttpClientFake.h"
#include "components/HttpClientCurl.h"

fruit::Component<VaultClientFactory>
getVaultClientFakeComponent()
{
  return fruit::createComponent()
    .replace(getHttpClientCurlComponent)
    .with(getHttpClientFakeComponent)
    .install(getVaultClientHttpComponent);
}