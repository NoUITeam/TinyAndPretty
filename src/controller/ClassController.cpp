#include <service/User.h>
#include <service/TAPSystem.h>
#include <service/Event.h>
using namespace std;
using namespace NEDBSTD;
using namespace UTILSTD;

Class::Class(string classid){
    this->id = classid;
}

int Class::Query(){
    int count,errCode;
    string retVal;
    errCode = __DATABASE.Select("classes","school","id = " + id,count,retVal);
    if(count == 0) schoolid = "0";
    else    schoolid = retVal.substr(retVal.find(';')+1);
    return errCode;
}

Json Class::getTimeTable(string schoolid,string prof){
    int count,len,length;
    string retVal;
    if(schoolid.length() == 0){
        Query();
        schoolid = this->schoolid;
    }
    NEDB _DB("/");
    if(prof.length()==0){
        _DB.SetDir(USER_DIR + "/" + schoolid); _DB.Mount("timetable");
        _DB.Select("timetable", "*", "class="+id, count, retVal);
    }else{
        _DB.SetDir(USER_DIR+"/"+schoolid+"/0/"+prof);   _DB.Mount("timetable");
        _DB.Select("timetable", "*", "", count, retVal);
        cout << retVal << endl;
    }
    _DB.Close();
    
    Json J;
    if(count == 0) return J;
    vector<SimpleJson::Object> courses;

    string* str = Split(retVal, ';', len);
    for(int i = 1; i < len; i++){
        string* info = Split(str[i], ',', length);
        Course course(info[2]); course.Query(false);
        User user(info[3]); user.Query(false);
        vector<int> Tcode;
        for(int i = 4;i<=8;i++){
            Tcode.push_back(stoi(info[i]));
        }
        __DATABASE.Select("landmark","name","id="+info[9],count,retVal);
        string location = retVal.substr(retVal.find(';')+1);
        courses.push_back({
            {"class",stoi(info[1])},
            {"id",stoi(info[2])},
            {"name",course.getName()},
            {"profid",stoi(info[3])},
            {"profname",user.getName()},
            {"wcode",course.getTime()},
            {"tcode",Tcode},
            {"location",location},
            {"room",info[10]}
        });
        delete [] info;
    }
    J.push_back({"courses",courses});
    delete[] str;
    return J;
}

Json Class::getList(){
    string ret;int count,len,length;
    int errCode = __DATABASE.Select("classes","*","",count,ret);
    Json J;
    if(errCode == NO_ERROR){
        ret = ret.substr(ret.find_first_of(";")+1);
        ret = ret.substr(ret.find_first_of(";")+1);
        string* str = Split(ret,';',len);
        vector<SimpleJson::Object> js;
        for(int i = 0;i<len; i++){
            string* temp =Split(str[i],',',length);
            js.push_back({
                {"id",stoi(temp[0])},
                {"school",stoi(temp[1])}
            });
            delete [] temp;
        }
        delete [] str;
        J.push_back({"data",js});
    }
    return J;
}

int Class::AddCourse(std::string& courseid,std::string& prof,std::string& detail){
    int errCode,count; string ret;
    if(__DATABASE.Select("courses","id","id="+courseid,count,ret) != NO_ERROR){
        return DATA_NOT_FOUND;
    }
    Query();
    NEDB _DB(USER_DIR + "/"+schoolid);
    errCode = _DB.Mount("timetable");
    if(errCode == FILE_NOT_FOUND){    
        _DB.DirInit();
        _DB.SetDefaultPageSize(800);
        _DB.Create("timetable","id int64,class int,course int,prof int,D1 int,D2 int,D3 int,D4 int,D5 int,loc int,room int");
    }
    time_t t; time(&t);
    string info = to_string(t)+","+this->id+","+courseid+","+prof+","+detail;
    errCode = _DB.Insert("timetable","",info);
    _DB.Close();
    
    User user(prof);
    user.Query();
    NEDB DB(USER_DIR+"/"+user.getSchool()+"/0/"+prof);
    errCode = DB.Mount("timetable");
    if(errCode == FILE_NOT_FOUND){    
        DB.DirInit();
        DB.SetDefaultPageSize(800);
        DB.Create("timetable","id int64,class int,course int,prof int,D1 int,D2 int,D3 int,D4 int,D5 int,loc int,room int");
    }
    errCode = DB.Insert("timetable","",info);

    if(errCode == NO_ERROR){
        DB.SetDir(SRC_DIR + "/course/"+courseid + "/"+prof);
        DB.DirInit();
    }
    DB.Close();
    return errCode;
}


int Class::AddNew(string school){
    NEDB _DB(SRC_DIR+"/school/");
    _DB.Mount("classes");
    int errCode = _DB.Insert("classes","",id+","+school);
    if(errCode != NO_ERROR){
        return errCode;
    }
    _DB.SetDir(USER_DIR+"/"+school+"/"+id);
    errCode = _DB.DirInit();
    _DB.Close();
    return errCode;
}

int Class::getMemberNum(){
    Query();
    string path = USER_DIR+"/"+schoolid+"/"+id+"/";
    DIR *pDir;
    struct dirent* ptr;
    cout << path << endl;
    if(!(pDir = opendir(path.c_str()))) return 0;
    int num = 0;
    while((ptr=readdir(pDir))!=0){
        if(strcmp(ptr->d_name,".")!=0 && strcmp(ptr->d_name,"..")!=0)
           num++;
    }
    closedir(pDir);
    return num;

}