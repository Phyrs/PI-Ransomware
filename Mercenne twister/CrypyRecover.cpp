#include "CrypyRecover.h"

CrypyRecover::CrypyRecover(string pathRacine)
: extensions({"txt", "exe", "php", "pl", "7z", "rar", "m4a", "wma", "avi", "wmv", "csv", "d3dbsp", "sc2save", "sie", "sum", "ibank", "t13", "t12", "qdf", "gdb", "tax", "pkpass", "bc6", "bc7", "bkp",
              "qic", "bkf", "sidn", "sidd", "mddata", "itl", "itdb", "icxs", "hvpl", "hplg", "hkdb", "mdbackup", "syncdb", "gho", "cas", "svg", "map", "wmo", "itm", "sb", "fos", "mcgame", "vdf",
              "ztmp", "sis", "sid", "ncf", "menu", "layout", "dmp", "blob", "esm", "001", "vtf", "dazip", "fpk", "mlx", "kf", "iwd", "vpk", "tor", "psk", "rim", "w3x", "fsh", "ntl", "arch00", "lvl",
              "snx", "cfr", "ff", "vpp_pc", "lrf", "m2", "mcmeta", "vfs0", "mpqge", "kdb", "db0", "mp3", "upx", "rofl", "hkx", "bar", "upk", "das", "iwi", "litemod", "asset", "forge", "ltx", "bsa",
              "apk", "re4", "sav", "lbf", "slm", "bik", "epk", "rgss3a", "pak", "big", "unity3d", "wotreplay", "xxx", "desc", "py", "m3u", "flv", "js", "css", "rb", "png", "jpeg", "p7c", "p7b", "p12",
              "pfx", "pem", "crt", "cer", "der", "x3f", "srw", "pef", "ptx", "r3d", "rw2", "rwl", "raw", "raf", "orf", "nrw", "mrwref", "mef", "erf", "kdc", "dcr", "cr2", "crw", "bay", "sr2", "srf",
              "arw", "3fr", "dng", "jpeg", "jpg", "cdr", "indd", "ai", "eps", "pdf", "pdd", "psd", "dbfv", "mdf", "wb2", "rtf", "wpd", "dxg", "xf", "dwg", "pst", "accdb", "mdb", "pptm", "pptx", "ppt",
              "xlk", "xlsb", "xlsm", "xlsx", "xls", "wps", "docm", "docx", "doc", "odb", "odc", "odm", "odp", "ods", "odt", "sql", "zip", "tar", "tar.gz", "tgz", "biz", "ocx", "html", "htm", "3gp",
              "srt", "cpp", "mid", "mkv", "mov", "asf", "mpeg", "vob", "mpg", "fla", "swf", "wav", "qcow2", "vdi", "vmdk", "vmx", "gpg", "aes", "ARC", "PAQ", "tar.bz2", "tbk", "bak", "djv", "djvu",
              "bmp", "cgm", "tif", "tiff", "NEF", "cmd", "class", "jar", "java", "asp", "brd", "sch", "dch", "dip", "vbs", "asm", "pas", "ldf", "ibd", "MYI", "MYD", "frm", "dbf", "SQLITEDB", "SQLITE3",
              "asc", "lay6", "lay", "ms11 (Security copy)", "sldm", "sldx", "ppsm", "ppsx", "ppam", "docb", "mml", "sxm", "otg", "slk", "xlw", "xlt", "xlm", "xlc", "dif", "stc", "sxc", "ots", "ods",
              "hwp", "dotm", "dotx", "docm", "DOT", "max", "xml", "uot", "stw", "sxw", "ott", "csr", "key"}),
   mercenneSlayer(32, 624, 397, 31, 0x9908B0DF, 11, 0xFFFFFFFF, 7, 0x9D2C5680, 15, 0xEFC60000, 18)
{
    uint8_t randints[624*4];

    mercenneSlayer.sEtatSCrypy(randints);
}

void CrypyRecover::decipher(string pathRacine) const
{
    string path = pathRacine;

    while (1)
    {
        decipher(path, mercenneSlayer.gKeyCrypy());
    }
}

void CrypyRecover::decipher(string pathFile, const char key[16]) const
{

}


//Fonctions statiques
void CrypyRecover::tester()
{
    CrypyRecover("tests/C");
}
