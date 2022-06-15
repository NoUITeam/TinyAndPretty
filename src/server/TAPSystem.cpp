#include <service/TAPSystem.h>
#include <service/User.h>

using namespace std;
using namespace NEDBSTD;
using namespace BTREESTD;

NEDB __DATABASE("/");
string PROJECT_DIR = "/";
string SYS_DIR = "/";
string USER_DIR = "/";
string SRC_DIR = "/";
string TIME_STAMP = "NAN";

string TokenSign(string userid){
    return md5(userid + TIME_STAMP);
}

void TimeStampReset(){
    srand((unsigned)time(nullptr));
    time_t p;
    time(&p);
    p += rand();
    TIME_STAMP = md5(to_string(p));
}

int TokenCheck(string userid, string token){
    string res = md5(userid + TIME_STAMP);
    if(res == token) return TOKEN_ACCESS;
    return TOKEN_DENIED;
}

int ServicePreload(){
    NEDB_SETTING(32, 400, 50, 5);
    NEDB_DEBUG(2);
    NEDB_TIME_FLAG(true);
    __DATABASE.SetDir(SYS_DIR.c_str());
    if(__DATABASE.DirInit() != 0){
        return UTILSTD::CONSOLE_LOG(3, 1, 1, "Dir Error\n");
    }
    int num;
    __DATABASE.MountAll(num);
    if(__DATABASE.Open(SYS_DIR + "/token/token") == NO_ERROR) num++;
    if(__DATABASE.Open(SYS_DIR + "/test/database_test") == NO_ERROR) num++;
    if(__DATABASE.Open(SRC_DIR + "/major/majors") == NO_ERROR) num++;
    if(__DATABASE.Open(SRC_DIR + "/school/schools") == NO_ERROR) num++;
    if(__DATABASE.Open(SRC_DIR + "/school/classes") == NO_ERROR) num++;
    if(__DATABASE.Open(SRC_DIR + "/map/landmark") == NO_ERROR) num++;
    if(__DATABASE.Open(SRC_DIR + "/course/courses") == NO_ERROR) num++;
    if(__DATABASE.Open(USER_DIR + "/users") == NO_ERROR) num++;
    return UTILSTD::CONSOLE_LOG(0, 1, 1, " %d Table Mounted\n", num);
}

void DirectoryInit(){
    PROJECT_DIR = get_current_dir_name();
    std::cerr << PROJECT_DIR << std::endl;
    //PROJECT_DIR = PROJECT_DIR.substr(0,PROJECT_DIR.find("/TAP")+4);
    USER_DIR = PROJECT_DIR + "/data/user";
    SRC_DIR = PROJECT_DIR + "/data/src";
    SYS_DIR = PROJECT_DIR + "/data/sys";
}

def_HttpEntry(Main,req){
    return new FileResponse{"data/src/index.html" , "text/html"};
}