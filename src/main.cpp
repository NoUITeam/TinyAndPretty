#include<basic.h>

int main(/*int argv , char* argc[]*/){
    __START__();
    /*
    NEdb db("");
    cout<<db.getDir()<<endl;
    db.setDir("/home/jianxf/.nesrc/");
    db.setDefaultPageSize(100);
    cout<<db.getMsg()<<endl;
    db.exec("create table test (id int);");
    cout<<db.getMsg()<<endl;
    string sql = "";
    for(int i =0; i < 50; i++){
        sql = sql + "insert into test values ("+to_string(i)+");";
    }
    db.exec(sql.c_str());
    cout<<db.getMsg()<<endl;
    db.exec("describe table test;select * from test where id>10 and id<36;");
    cout<<db.getMsg()<<endl;
    cout<<db.getData()<<endl;
    db.exec("delete from test where id < 44;select * from test;drop table test;");
    cout<<db.getMsg()<<endl;
    cout<<db.getData()<<endl;
    db.close();
    */
}

/*
测试数据  @  
CREATE test(ID:INT,NAME:TEXT,SCHOOL:LONGTEXT,KEY(ID));
INSERT test(1001,Name_01,School_01);
INSERT test(1002,Name_02,School_02);
INSERT test(1003,Name_03,School_03);
INSERT test(1004,Name_04,School_04);
INSERT test(1005,Name_05,School_05);
INSERT test(1006,Name_06,School_06);
INSERT test(1007,Name_07,School_07);
PRINT test;
DELETE test(0);
REMOVE test;

*/