// LibDogecoin 0.1.2
#include "dogecoin/libdogecoin.h"


//#define PRIVKEYWIFLEN 51 //WIF length for uncompressed keys is 51 and should start with Q. This can be 52 also for compressed keys. 53 internally to lib (+stringterm)
#define PRIVKEYWIFLEN 53 //Function takes 53 but needs to be fixed to take 51.

//#define MASTERKEYLEN 111 //should be chaincode + privkey; starts with dgpv51eADS3spNJh8 or dgpv51eADS3spNJh9 (112 internally including stringterm? often 128. check this.)
#define MASTERKEYLEN 128 // Function expects 128 but needs to be fixed to take 111.

//#define PUBKEYLEN 34 //our mainnet addresses are 34 chars if p2pkh and start with D.  Internally this is cited as 35 for strings that represent it because +stringterm.
#define PUBKEYLEN 35 // Function expects 35, but needs to be fixed to take 34.

// Example of how to use libdogecoin API functions:
// gcc ./libdogecoin-php/libdogecoin-json.c -I./include -L./lib -ldogecoin -o ./libdogecoin-php/libdogecoin-json-php

int main(int argc, char *argv[]) {
   // qputenv("DYLD_LIBRARY_PATH",  QApplication::applicationDirPath().toUtf8()    + "/");
    dogecoin_ecc_start();

    char wif_privkey[PRIVKEYWIFLEN];
    char p2pkh_pubkey[PUBKEYLEN];
    char wif_master_privkey[MASTERKEYLEN];
    char p2pkh_master_pubkey[PUBKEYLEN];
    char p2pkh_child_pubkey[PUBKEYLEN];

    // JSON keypair generation
   if (strcmp(argv[1], "generatePrivPubKeypair") == 0){
        if (generatePrivPubKeypair(wif_privkey, p2pkh_pubkey, 0)) {
            printf("{\"private\":\"%s\",\"public\":\" %s\"}", wif_privkey, p2pkh_pubkey);
        } else {
            printf("{\"error\":\"1\"}");
            return -1;
        }
    };


    if (strcmp(argv[1], "generateHDMasterPubKeypair") == 0){
        if (generateHDMasterPubKeypair(wif_master_privkey, p2pkh_master_pubkey, 0)) {
            printf("{\"private\":\"%s\",\"public\":\" %s\"}", wif_master_privkey, p2pkh_master_pubkey);
        } else {
            printf("{\"error\":\"1\"}");
            return -1;
        }
    }


    if (strcmp(argv[1], "generateDerivedHDPubkey") == 0){
        if (generateDerivedHDPubkey((const char*)wif_master_privkey, (char*)p2pkh_child_pubkey)) {
            printf("{\"private\":\"%s\",\"public\":\" %s\"}", wif_master_privkey, p2pkh_child_pubkey);
        } else {
            printf("{\"error\":\"1\"}");
            return -1;
        }
    }


  // keypair verification

      //argv[1] = command
      //argv[2] = privat key
      //argv[3] = public key

    if (strcmp(argv[1], "verifyPrivPubKeypair") == 0){
        if (verifyPrivPubKeypair(argv[2], argv[3], 0)) {
            printf("{\"valid\":\"true\"}");
        }
        else {
            printf("{\"valid\":\"false\"}");
            return -1;
        }
    }

    if (strcmp(argv[1], "verifyHDMasterPubKeypair") == 0){
        if (verifyHDMasterPubKeypair(argv[2], argv[3], 0)) {
            printf("{\"valid\":\"true\"}");
        }
        else {
            printf("{\"valid\":\"false\"}");
            return -1;
        }
    }

    // address verification

    //argv[1] = command
    //argv[2] = dp2pkh public key

    if (strcmp(argv[1], "verifyP2pkhAddress") == 0){
        if (verifyP2pkhAddress(argv[2], strlen(argv[2]))) {
            printf("{\"valid\":\"true\"}");
        }
        else {
            printf("{\"valid\":\"false\"}");
            return -1;
        }
    }

    // build transaction

    //argv[1] = command
    //argv[2] = hash_doge (tx hash with doge)
    //argv[3] = external_p2pkh_addr
    //argv[4] = 5.0 (doge to send)
    //argv[5] = 0.00226 (fees)
    //argv[6] = 12 (tx index)
    //argv[7] = mypubkey
    //argv[8] = myscriptpubkey
    //argv[9] = myprivkey

    if (strcmp(argv[1], "start_transaction") == 0){

        int idx = start_transaction();
        add_utxo(idx, argv[2], 1);
        add_output(idx, argv[3], argv[4]);
        finalize_transaction(idx, argv[3], argv[5], argv[6], argv[7]);
        sign_transaction(idx, argv[8], argv[9]);
        printf("{\"rawtx\":\"%s\"}", get_raw_transaction(idx));
        remove_all();
    }

    // Get the moon
    if (strcmp(argv[1], "moon") == 0){
    //    printf("{\"moon\":\"%s\"}", moon());
    }

    dogecoin_ecc_stop();
}
