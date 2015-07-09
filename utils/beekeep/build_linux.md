### beekeep linux build

building on linux should be pretty easy.

the main dependencies are pkg-config, gtk+ 3, and jansson (a jason parsing library.)

here are the complete steps for installing from a fresh install of vanilla xubuntu:

```
# get packages
sudo apt-get install git curl pkg-config libgtk-3-dev

# get and install jansson
curl http://www.digip.org/jansson/releases/jansson-2.7.tar.gz > jansson-2.7.tar.gz
tar -xcvf jansson-2.7.tar.gz
cd jansson-2.7
./configure
make
sudo make install

# get aleph repository and build beekeep
git clone https://github.com/tehn/aleph.git
cd aleph/utils/beekeep
make

```

that should do it.