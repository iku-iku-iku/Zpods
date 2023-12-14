#!/bin/bash
# make sure that we are in the root directory of the project
if [ ! -f "scripts/light_deploy.sh" ]; then
	echo "Please run this script from the root directory of the project"
	exit 1
fi

DEPLOY_PATH=deploy

rm -rf $DEPLOY_PATH/zpods_cli
rm -rf $DEPLOY_PATH/zpods_server
cp build/network/client/cli/src/zpods_cli $DEPLOY_PATH
cp build/network/server/src/zpods_server $DEPLOY_PATH
