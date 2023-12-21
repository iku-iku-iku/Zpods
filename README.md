# Zpods

## project structure

- ZpodsLib
- network
  - server
  - client

## configure

```sh
make configure
```

## test

```sh
make test
```

## how to deploy?

### gen keys

You need to generate keys and credentials first.

For local use, just run `make gen_key_local`, and you will see `client.key`, `client.crt`, `server.key`, `server.crt` in the root directory of the project.

For remote use, you must modify the IP in `san.cnf`, and then run `make gen_key`.

NOTE: the client and the server share the same version of keys and credentials. So please only do this step once, and reuse generated files for the following steps.

### make appimage

1. You need to run `python3 scripts/gen_cmd_for_dep.py deploy/zpods_cli AppDir/usr/lib` to get the commands and replace them in `scripts/build_appimage.sh`
2. You need to get appimagetool from `https://appimage.github.io/appimagetool/` and install it.
3. Make sure you have run `make gen_key` or `make gen_key_local` first.
4. `make AppImage`

### deploy with docker

1. You need to run `python3 scripts/gen_cmd_for_dep.py deploy/zpods_server server_image` to get the commands and replace them in `scripts/docker_build.sh` and `Docker.server`.
2. Run `make run_server` to build docker image and run it locally.
3. For remote deployment, `make push_docker`. Then copy the `./scripts/run_server.sh` to your server and run it.

## install cli

```sh
# the following command will install cli in /usr/local/bin (which is in $PATH),
# so you can directly use 'zpods.cli' without specifying the path
sudo make install_cli
```

## uninstall

```sh
sudo make uninstall
```

## CLI usage

### help

```shell
zpods.cli --help
zpods.cli backup --help
zpods.cli restore --help
zpods.cli register --help
```

### backup

#### backup a single path

```shell
zpods.cli backup --src-list test_data/single --target ./tmp
```

#### backup multiple paths

```shell
# in this case, you must specify the target backup filename!
zpods.cli backup --src-list test_data/tiny test_data/middle --target ./tmp/multiple.map
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
zpods.cli restore --src ./tmp/single.map --target ./tmp
# if your backup is encrypted,
# you must use '-p' to specify the password used in encryption
zpods.cli restore --src ./tmp/single.map --target ./tmp -p 1234
```

### remote backup

```shell
# set you remote server address
zpods.cli set_server_addr IP:PORT
zpods.cli register
zpods.cli login
# use -r in backup to backup to remote server
# the mapping will be recorded
zpods.cli backup ... -r
# run daemon for synchronization continuously
zpods.cli daemon
```
