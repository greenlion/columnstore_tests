#!/bin/bash
git branch $1
git checkout $1
git add .
git commit -m "$1"
git push -u origin $1
