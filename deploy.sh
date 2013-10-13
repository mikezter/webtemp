#!/bin/bash
coffee -c html/*.coffee
scp -r html/* arduino:/www/webtemp/
