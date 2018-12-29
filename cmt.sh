#!/bin/bash

if [ -e /etc/linux_init ];then
  exit 0
fi
sudo touch /etc/linux_init

echo "set nu" >> ~/.vimrc
echo "alias gg=\"grep -R --color=auto \"" >> ~/.bashrc
git config --global user.email "meeho@meeho"
git config --global user.name "meeho"
if [ ! -e dlx/hd.img ];then
    cp -fr dlx/hd10m.img dlx/hd.img
fi
