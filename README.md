# cconfd

This project been superseded by [rconfd](https://github.com/eburghar/rconfd).

`cconfd` is an utility for containers, written in c++, to generate config files
from [jsonnet templates](https://jsonnet.org/), and keep them in sync with
secrets fetched from a [vault server](https://www.vaultproject.io/) with [kubernetes
authentication](https://www.vaultproject.io/docs/auth/kubernetes). It can use the simple and yet effective
[startup notification](https://skarnet.org/software/s6/notifywhenup.html) mechanism of the [s6 supervision
suite](https://skarnet.org/software/s6/) to signal other services that their configuration files have been generated
and it can launch arbitrary command when configuration change.

 ## Usage

```
cconfd {OPTIONS}

    Generate files with jsonnet and vault secrets using kubernetes
    authentication

  OPTIONS:

      -h, --help                        Display this help menu
      -d[dir], --dir=[dir]              The config directory
      -u[url], --url=[url]              The vault url
      -c[path], --cacert=[path]         the vault ca certificate authority
      -t[path], --token=[path]          Path of kubernetes token
      -s[seconds], --sleep=[seconds]    time to sleep in main loop
      -v, --verbose                     Level of verbosity
      -r[readiness descriptor],
      --readiness=[readiness
      descriptor]
```

```bash
cconfd --url http://vault:8200/v1 --dir /etc/cconfd --token /var/run/secrets/kubernetes.io/serviceaccount/token --ca /var/run/secrets/kubernetes.io/serviceaccount/ca.crt
```

In an infinite loop it :
1. logs into vault with kubernetes service account token (jwt),
1. gets a list of secrets and injects them as a `secrets` variable in the preamble of a jsonnet template,
1. evaluates template in multi manifest mode,
1. saves each root keys of the returned evaluation dictionary as a file. The keys are filenames relatively to `dir`, and their values should be a strings,
1. calls the `cmd` command if any generated files have changed

```json
{
    "service": {
        "dir": "/var/lib/service",
        "tmpl": "/etc/cconfd/tmpl/service.jsonnet",
        "mode": "0644",
        "role": "demo",
        "secrets": {
            "account": "vault:secret/data/demo/account",
            "db": "vault:database/creds/demo"
        },
        "cmd": "/bin/s6-svc -h /var/run/s6/services/myservice"
    }
}
```

`secrets` is a map of variable name and secret path. The variable is accessible in jsonnet template as a key from
the `secrets` local variable and the path follows the syntax `back-end:path`. You can use variable substitutions
in path (ex: `vault:/secret/data/${NAMESPACE}/mail`)

There are 3 back-ends for secrets
- `vault:` followed by the path of the secret. The value will be a json value.
- `file:` followed by the path of the file. The value will be a string
- `env:` followed by the name of the variable. The value will be a string
 
# Building

The project use [bazel build](https://bazel.build/) system

```sh
bazel build //:all //test:all
```
