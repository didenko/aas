#!/bin/bash

java \
  -classpath "$(dirname ${0})/aeron-driver.jar" \
  io.aeron.driver.MediaDriver
