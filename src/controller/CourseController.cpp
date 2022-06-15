#include <service/Event.h>
#include <service/TAPSystem.h>
using namespace std;
using namespace NEDBSTD;
using namespace UTILSTD;

Course::Course(string id){
    this->id = id;
}

int Course::Query(bool intro){
    int count,len;
    string retVal;
    int res = __DATABASE.Select("courses","*","id=" + id,count,retVal) != NO_ERROR;
    if(res != NO_ERROR) return res;
    string *str = Split(retVal,';',len);
    if(retVal == "" || count == 0 || str == nullptr){
        delete [] str;
        return PARAM_FORM_ERROR;
    }
    string *info = Split(str[1],',',len);
    delete [] str;
    if(len != 3)    return PARAM_FORM_ERROR;

    /* Fill Details */
    this->name = info[1];
    this->time = info[2];
    delete [] info;

    if(!intro){
        intro = "null";
        return NO_ERROR;
    }

    ifstream ifs((SRC_DIR + "/course/" + this->id + "/intro.txt").c_str());
    if(!ifs.is_open()){
        intro = "null";
    }else{
        ifs >> noskipws;
        unsigned char c; this->intro = "";
        while(!ifs.eof()){
            ifs >> c;
            this->intro += c;
        }
    }

    return NO_ERROR;
}

SimpleJson::Object Course::Format(){
    SimpleJson::Object J({
        {"id",id.c_str()},
        {"name",name.c_str()},
        {"time",stoi(time)},
        {"intro",intro.c_str()},
    });
    return J;
}

int Course::AddNew(string& detail,string& intro){
    //System DataBase
    int errCode = __DATABASE.Insert("courses","",detail);
    if(errCode != NO_ERROR) return errCode;

    string dir = SRC_DIR + "/course/" + detail.substr(0,detail.find_first_of(",")) + "/";
    NEDB _DB(dir);
    _DB.DirInit();
    _DB.Close();

    //Introduction
    ofstream ofs(dir + "/intro.txt",ios::trunc|ios::out);
    ofs << intro;
    ofs.close();

    return NO_ERROR;
}

int Course::Remove(){
    int count;
    int errCode = __DATABASE.Delete("course","id="+this->id,count);
    if(errCode != NO_ERROR) return errCode;
    string dir = SRC_DIR + "/course/" + this->id;
    if(rmdir(dir.c_str()) != 0) return DIR_ERROR;
    return NO_ERROR;
}

int Course::AddWork(string& prof,string& classid,string& detail){
    string dir = SRC_DIR + "/course/" + id + "/" + prof + "/" + classid;
    NEDB _DB(dir);
    if(_DB.Mount("homework") == FILE_NOT_FOUND){
        _DB.DirInit();
        _DB.Create("homework","start int64,end int64,name text");
    }
    int errCode = _DB.Insert("homework","",detail);
    if(errCode == NO_ERROR){
        string id = detail.substr(0,detail.find_first_of(','));
        _DB.SetDir(dir +"/"+id);
        _DB.DirInit();
    }
    _DB.Close();
    return errCode;
}

Json Course::getWork(string& prof,string& classid){
    string dir = SRC_DIR + "/course/" + id + "/" + prof +"/"+classid;
    NEDB _DB(dir);
    int errCode,count,len,length;string ret;
    errCode = _DB.Mount("homework");
    Json J;
    errCode = _DB.Select("homework","*","",count,ret);
    if(count == 0) return J;
    ret = ret.substr(ret.find_first_of(';')+1);
    string* str = Split(ret,';',len);
    vector<SimpleJson::Object> works;
    for(int i = 0; i < len; i++){
        string* temp = Split(str[i],',',length);
        works.push_back({
            {"start",temp[0]},
            {"end",temp[1]},
            {"name",temp[2]}
        });
    }
    J.push_back({"homework",works});
    return J;

}

int Course::AddExam(string& detail){
    string dir = SRC_DIR + "/course/" + id;
    NEDB _DB(dir);
    if(_DB.Mount("exam") == FILE_NOT_FOUND){
        _DB.SetDefaultPageSize(400);
        _DB.Create("exam","id int,school int,name text,start int,len int,location int,room int");
    }
    int errCode = _DB.Insert("exam","",detail);
    _DB.Close();
    return errCode;
}


/*

{"detail":
    [
        {
            "id":xx,...
        },
        {
            "id":xx,...
        }
    ]

}


*/
