#!/usr/bin/env sh

cd public

cp index.html 404.html

git init
git add -A
git commit -m 'deploy'

git push -f git@github.com:GregoryKogan/particle-flow-2.git main:gh-pages

cd ..
