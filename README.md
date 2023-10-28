# Zpods

## project structure
- ZpodsLib
    - src
    - test
- client
    - cli
        - src
        - test
    - gui
        - src
        - test
- server
    - src
    - test

## configure
``` sh
make configure
```

## test
``` sh
make test
```

## install cli
``` sh
# the following command with install cli in /usr/local/bin (which is in $PATH),
# so you can directly use 'zpods.cli' without specify the path
sudo make install_cli
```

## uninstall
``` sh
sudo make uninstall
```


## CLI usage

### help
``` shell
zpods.cli --help
zpods.cli backup --help
zpods.cli restore --help
zpods.cli register --help
```

### backup

#### backup a single path
```shell
zpods.cli backup --src-list test_data/single --target ./tmp
# or specify a target backup filename
zpods.cli backup --src-list test_data/single --target ./tmp/haha.pods
```

#### backup multiple paths
```shell
# in this case, you must specify the target backup filename!
zpods.cli backup --src-list test_data/tiny test_data/middle --target ./tmp/multiple.pods
```

#### backup with compression
```shell
# use -c to enable compression
zpods.cli backup --src-list test_data/single --target ./tmp -c
```

#### backup with encryption
```shell
# use -p to specify a password for encryption
zpods.cli backup --src-list test_data/single --target ./tmp -c -p 1234
```

### restore

```shell
# the restore can automatically uncompress if needed
zpods.cli restore --src ./tmp/single.pods --target ./tmp
# if your backup is encrypted, you must use '-p' to specify the password used in encryption
zpods.cli restore --src ./tmp/single.pods --target ./tmp -p 1234
```