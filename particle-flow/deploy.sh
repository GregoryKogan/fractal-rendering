#!/usr/bin/env sh

# abort on errors
set -e

# build cpp
cd src/cpp
rm -rf build
mkdir build
cd build
emcmake cmake ..
make
cd ..
rm -rf build
cd ../..

# build
yarn build

# navigate into the build output directory
cd dist

cp index.html 404.html

# if you are deploying to a custom domain
# echo 'www.example.com' > CNAME

git init
git add -A
git commit -m 'deploy'

# if you are deploying to https://<USERNAME>.github.io
# git push -f git@github.com:<USERNAME>/<USERNAME>.github.io.git master

# if you are deploying to https://<USERNAME>.github.io/<REPO>
git push -f git@github.com:GregoryKogan/particle-flow-2.git main:gh-pages

cd -
