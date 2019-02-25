using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Security;
using System.Net;
using Microsoft.Win32;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;

namespace HiddenTearConsole
{
    class Program
    {
        static void Main(string[] args)
        {
            if(args.Length != 2)
            {
                Console.WriteLine("usage : pgm.exe <plain_text_path> <ciphered_text_path>\n");
                Console.ReadLine();
                Environment.Exit(1);
            }

            string plainpath = args[0];
            string path = args[1];
            
            string draftdata = " ";
            var timestamp = File.GetLastWriteTime(path) - DateTime.Now.AddMilliseconds(-Environment.TickCount);
            int ms = (int)timestamp.TotalMilliseconds;
            int count = 0;
            string password = "";
            int diff = 0;
            int i = 0;
            string data = System.Text.Encoding.UTF8.GetString(File.ReadAllBytes(plainpath));
            while (data != draftdata && count < 500)
            {
                password = CreatePassword(15, ms - diff);
                Console.WriteLine("Trying: " + password + " " + "Count: " + count);
                byte[] bytesToBeDecrypted = File.ReadAllBytes(path);
                byte[] passwordBytes = Encoding.UTF8.GetBytes(password);
                passwordBytes = SHA256.Create().ComputeHash(passwordBytes);
                byte[] bytesDecrypted = AES_Decrypt(bytesToBeDecrypted, passwordBytes);
                draftdata = System.Text.Encoding.UTF8.GetString(bytesDecrypted);
                diff++;
                count++;
                i++;
            }

            if(count >= 500)
            {
                Console.WriteLine("Key has not been found...\n");
                Console.ReadLine();
                Environment.Exit(1);
            }

            Console.WriteLine("Found : The password that has been used is : " + password + " \nFound after:" + count + " tries. \nHere are the data : " + draftdata);
            Console.ReadLine();

        }

        // Imported from the Hidden tear decrypt source code.
        public static string CreatePassword(int length, int seed)
        {
            const string valid = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890*!=&?&/";
            StringBuilder res = new StringBuilder();
            Random rnd = new Random(seed);
            while (0 < length--)
            {
                res.Append(valid[rnd.Next(valid.Length)]);
            }
            return res.ToString();
        }


        // Imported from the Hidden tear decrypt source code.
        public static byte[] AES_Decrypt(byte[] bytesToBeDecrypted, byte[] passwordBytes)
        {
            byte[] decryptedBytes = null;
            
            byte[] saltBytes = new byte[] { 1, 2, 3, 4, 5, 6, 7, 8 };

            using (MemoryStream ms = new MemoryStream())
            {
                using (RijndaelManaged AES = new RijndaelManaged())
                {

                    AES.KeySize = 256;
                    AES.BlockSize = 128;

                    var key = new Rfc2898DeriveBytes(passwordBytes, saltBytes, 1000);
                    AES.Key = key.GetBytes(AES.KeySize / 8);
                    AES.IV = key.GetBytes(AES.BlockSize / 8);

                    AES.Mode = CipherMode.CBC;
                    AES.Padding = PaddingMode.PKCS7;

                    try
                    {
                        using (var cs = new CryptoStream(ms, AES.CreateDecryptor(), CryptoStreamMode.Write))
                        {

                            cs.Write(bytesToBeDecrypted, 0, bytesToBeDecrypted.Length);
                            cs.FlushFinalBlock();
                            cs.Close();

                        }
                    } catch (Exception e)
                    {

                    }
                    decryptedBytes = ms.ToArray();
                }
            }

            return decryptedBytes;
        }
    }
}
