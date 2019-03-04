using System;
using System.IO;
using System.Security.Cryptography;
using System.Text;

namespace HiddenTearConsole
{
    class Program
    {
        const string valid = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890*!=&?&/";
        static byte[] saltBytes = new byte[] { 1, 2, 3, 4, 5, 6, 7, 8 };
        const int NUMBER_THREAD = 8;
        static int THREAD_COUNT = 0;
        static System.Threading.Thread[] thread_tab = new System.Threading.Thread[NUMBER_THREAD];

        static void Main(string[] args)
        {
            if(args.Length != 3)
            {
                Console.WriteLine("usage : pgm.exe <plain_text_path> <ciphered_text_path> <time_in_seconds_to_bruteforce>\n");
                Console.ReadLine();
                Environment.Exit(1);
            }

            string plainpath = args[0];
            string path = args[1];
            int bruteforceLimit = int.Parse(args[2]);
            
            string draftdata = " ";
            int count = 0;
            string password = "";

            string data = System.Text.Encoding.UTF8.GetString(File.ReadAllBytes(plainpath));
            byte[] bytesToBeDecrypted = File.ReadAllBytes(path);

            Boolean found = false;
            string passres = "";
            int countres = -1;

            while (count <= bruteforceLimit*1000)
            {
                /*int zero = Environment.TickCount;
                password = CreatePassword(15, count);
                if(count % 50 == 0)
                    Console.WriteLine("Try number : " + count);
                int un = Environment.TickCount;
                byte[] passwordBytes = Encoding.UTF8.GetBytes(password);
                passwordBytes = SHA256.Create().ComputeHash(passwordBytes);
                int deux = Environment.TickCount;
                byte[] bytesDecrypted = AES_Decrypt(bytesToBeDecrypted, passwordBytes);
                int trois = Environment.TickCount;*/
                
                // Waiting for empty thread slot
                while(THREAD_COUNT == NUMBER_THREAD)
                {
                    for (int i = 0; i < NUMBER_THREAD; i++)
                    {
                        if (thread_tab[i] != null && !thread_tab[i].IsAlive)
                            THREAD_COUNT--;
                    }
                }

                int countForThread = count;
                System.Threading.Thread thread = new System.Threading.Thread( () =>
                {
                    // variables
                    int start = countForThread;
                    Console.WriteLine("Testing range {" + start + " - " + (start + 1000) + "]");
                    for(int i = start; i < start + 1000; i++)
                    {
                        string passwordThread = CreatePassword(15, i);
                        byte[] passwordBytes = Encoding.UTF8.GetBytes(passwordThread);
                        passwordBytes = SHA256.Create().ComputeHash(passwordBytes);
                        byte[] bytesDecrypted = AES_Decrypt(bytesToBeDecrypted, passwordBytes);
                        string draftdataThread = System.Text.Encoding.UTF8.GetString(bytesDecrypted);

                        if (data == draftdataThread)
                        {
                            found = true;
                            passres = passwordThread;
                            countres = i;
                            break;
                        }
                    }
                    Console.WriteLine("End of [" + start + " - " + (start + 1000) + "]");
                });

                // adding thread in tab if empty or if is not alive
                for (int i = 0; i < NUMBER_THREAD; i++)
                {
                    if (thread_tab[i] == null || !thread_tab[i].IsAlive)
                    {
                        thread_tab[i] = thread;
                        THREAD_COUNT++;
                        break;
                    }
                }
                thread.Start();
                count = count + 1000;

                /*draftdata = System.Text.Encoding.UTF8.GetString(bytesDecrypted);
                count++;
                if (count % 50 == 0)
                    Console.WriteLine("Un : " + (deux -un) + " deux : " + (trois- deux) + " trois : " + (un-zero));*/
            }

            while (!found && THREAD_COUNT != 0)
            {
                for (int i = 0; i < THREAD_COUNT; i++)
                {
                    if (thread_tab[i] != null && thread_tab[i].IsAlive)
                    {
                        thread_tab[i].Join();
                        break;
                    }
                }
            }

            if (!found)
            {
                Console.WriteLine("Key has not been found...\n");
                Environment.Exit(0);
            }

            for (int i = 0; i < THREAD_COUNT; i++)
            {
                if (thread_tab[i] != null && thread_tab[i].IsAlive)
                {
                    thread_tab[i].Abort();
                }
            }

            Console.WriteLine("Found : The password that has been used is : " + passres + " \nFound after:" + (countres) + " tries. \n" +
                "You can now use the password to recover your files.");
            
        }

        // Imported from the Hidden tear decrypt source code.
        public static string CreatePassword(int length, int seed)
        {
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
