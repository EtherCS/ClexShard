#!/bin/bash

BFT_SMART_LIB=lib/bft-smart-1.2.1-UCL.jar
CLIENT_API_DB=../chainspacecore-$1-$2/database
CLIENT_API_PORT=$((5000+$1*10+$2))

rm config/currentView
screen -dmS client-$1-$2 java -Dclient.api.database=${CLIENT_API_DB} -Dclient.api.port=${CLIENT_API_PORT} -cp ${BFT_SMART_LIB}:target/chainspace-1.0-SNAPSHOT-jar-with-dependencies.jar uk.ac.ucl.cs.sec.chainspace.Client ChainSpaceClientConfig/config.txt
