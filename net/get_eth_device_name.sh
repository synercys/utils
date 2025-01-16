#!/bin/bash

ETH_DEV=`ip -o -4 route show to default | awk '{print $5}'`
ifconfig ${ETH_DEV}
