#!/bin/sh
#1.Create a 2048-bit private key.
# Private key recommendations by 2048-bit,less than this may soon be unsafe.
# You can add the '-des3' option before '-out' to encrypt the key file,then 
# whenever need to read this key file are required # to enter a password.I'm 
# eliminating this option for convenient.
openssl genrsa -out privkey.pem 2048
#2.Create a certificate.
#	openssl req -new -key privkey.pem -out cert.csr
#this command will create a certificate request file 'cert.csr',You can take 
#this file to the digital certificate authority (ie CA) to apply for a digital 
#certificate.CA will give you a new file cacert.pem, that is your digital certificate.This is just for a test,so use the following:
openssl req -new -x509 -key privkey.pem -out cacert.pem -days 1095
