#include<iostream>
#include<fstream>
#include "json.h"
#include<string>
#include<cassert>
#include<vector>
#include<ctime>
#include<cstdlib>

using namespace std;

bool is_element_in_vector(vector<string> v, string e){
    vector<string>::iterator it;
    for(it = v.begin(); it != v.end(); ++it){
        if(*it == e){
            return true;
        }
    }
    return false;
}

static string  getCurrentTimeStr()
{
	time_t t = time(NULL);
	char ch[64] = {0};
	strftime(ch, sizeof(ch) - 1, "%Y-%m-%d", localtime(&t));
	return ch;
}

int get_new_id()
{
    Json::Value root;
    Json::Reader reader;
    Json::StyledWriter swriter;
    int new_id;

    ifstream ifs("id.json");

    if(!reader.parse(ifs, root)){
        //fail to open;
        throw "can't open the file 'id.json'!";
    }
    else{
        new_id = 1;
        for(int i = 0; i < root.size(); ++i){
            if(new_id == root[i].asInt()){
                new_id++;
            }
        }
    }
    root.append(new_id);

    ifs.close();
    
    ofstream ofs("id.json");
    string new_id_list = swriter.write(root);
    ofs << new_id_list;
    ofs.close();

    return new_id;
}

void print_one(int id)
{
    Json::Value root;
    Json::Reader reader;
    Json::StyledWriter swriter;

    ifstream ifs("tasks.json");

    if(!reader.parse(ifs, root)){
        cout << "can't read the file 'tasks.json'!";
        return;
    }

    for(int i = 0; i < root.size(); ++i){
        if(root[i]["--id"].asInt() == id){
            string obj_print = swriter.write(root[i]);
            cout << obj_print << endl;
            return;
        }
    }

    cout << "Id " << id << " does not exist!" << endl;
    return;
}

void print_all()
{
    Json::Value root;
    Json::Reader reader;
    Json::StyledWriter swriter;

    ifstream ifs("tasks.json");

    if(!reader.parse(ifs, root)){
        cout << "can't read the file 'tasks.json'!";
        return;
    }
    string str_print;
    for(int i = 0; i < root.size(); ++i){
        str_print = swriter.write(root[i]);
        cout << str_print << endl;
    }

    ifs.close();
    return;
}

void print(int argc, char *argv[])
{
    string command = argv[2];
    if(command == "--all"){
        print_all();
        return;
    }

    string id_str;
    bool isNum = true;

    for(int i = 2; i < argc; ++i){
        id_str = argv[i];
        for(int j = 0; j < id_str.size(); ++j){
            if(!isdigit(id_str[j])){
                isNum = false;
                break;
            }
        }

        if(isNum){
            print_one(atoi(argv[i]));
        }
        else{
            cout << id_str << " is not an id!" << endl;
        }
    }
    return;
}

void create(int argc, char *argv[])
{
    int id;
    try{
        id = get_new_id();
    }catch(const char* msg){
        cerr << msg << endl;
        return;
    }

    Json::Value root;
    Json::Reader reader;
    Json::StyledWriter swriter;

    ifstream ifs("tasks.json");
    if(!reader.parse(ifs, root)){
        //fail to open;
        throw "can't read the file 'tasks.json'!";
    }

    Json::Value new_root;

    new_root["--id"] = id;
    new_root["--creation date"] = getCurrentTimeStr();

    int index = 2;
    vector<string> valid_command = {"--title", "--description", "--status", "--deadline", "--percentage", "--priority", "--subtask"};
    while(index < argc){
        if(is_element_in_vector(valid_command, string(argv[index]))){
            new_root[argv[index]] = argv[index+1];
        }
        index += 2;
    }
    root.append(new_root);
    ifs.close();

    ofstream ofs("tasks.json");
    string write_str = swriter.write(root);
    ofs << write_str;
    ofs.close();

    return;
}

void list(int argc, char *argv[])
{
    Json::Value root;
    Json::Reader reader;
    vector<string> valid_command = {"--title", "--description", "--status", "--creation date", "--deadline", "--percentage", "--priority"};
    
    int a_ind = 2;

    bool is_verify = true;
    
    ifstream ifs("tasks.json");
    if(!reader.parse(ifs, root)){
        cout << "can't read the file 'tasks.json'!";
        return;
    }

    for(int i = 0; i < root.size(); ++i){
        is_verify = true;
        while(a_ind < argc - 1){
            //to verify if this attribute existe;
            if(!is_element_in_vector(valid_command, argv[a_ind])){
                cout << argv[a_ind] << " is not a valid attribute!" << endl;
                return;
            }

            if(root[i][argv[a_ind]] == argv[a_ind+1]){
                //this attribute verify the list requirement;
                a_ind += 2;
            }
            else{
                is_verify = false;
                break;
            }
        }
        if(is_verify){
            cout << "find one" << endl;
            print_one(root[i]["--id"].asInt());
        }
    }
    return;
}

void task_delete_one(int id)
{
    Json::Value root_id, root_task, new_root_id, new_root_task;
    Json::Reader reader;
    Json::StyledWriter swriter;
    bool isDelete = false;

    ifstream ifs("id.json");
    if(!reader.parse(ifs, root_id)){
        cout << "can't read the file 'id.json'!";
        return;
    }
    for(int j = 0; j < root_id.size(); ++j){
        if(root_id[j].asInt() != id){
            new_root_id.append(root_id[j]);
        }
        else{
            isDelete = true;
        }
    }
    ifs.close();

    ifs.open("tasks.json");
    if(!reader.parse(ifs, root_task)){
        cout << "can't read the file 'tasks.json'!";
        return;
    }
    for(int k = 0; k < root_task.size(); ++k){ 
        if(root_task[k]["--id"].asInt() != id){
            new_root_task.append(root_task[k]);
        }
    }
    ifs.close();

    string write_str_id = swriter.write(new_root_id);
    string write_str_task = swriter.write(new_root_task);

    ofstream ofs("tasks.json");
    ofs << write_str_task;
    ofs.close();

    ofs.open("id.json");
    ofs << write_str_id;
    ofs.close();

    if(isDelete){
        cout << "task id " << id << " deleted successfully!";    
    }
    else{
        cout << "didn't find task id " << id << "!";
    }
    return;
}

void task_delete_all()
{
    string empty= "[]";

    ofstream ofs("id.json");
    ofs << empty;
    ofs.close();

    ofs.open("tasks.json");
    ofs << empty;
    ofs.close();

    return;
}

void task_delete(int argc, char *argv[])
{
    string command = argv[2];
    if(command == "--all"){
        task_delete_all();
        return;
    }

    string id_str;
    bool isNum = true;

    for(int i = 2; i < argc; ++i){
        id_str = argv[i];
        for(int j = 0; j < id_str.size(); ++j){
            if(!isdigit(id_str[j])){
                isNum = false;
                break;
            }
        }

        if(isNum){
            task_delete_one(atoi(argv[i]));
        }
        else{
            cout << id_str << " is not an id!" << endl;
        }
    }
}

void change(int argc, char *argv[])
{
    Json::Value root;
    Json::Reader reader;
    Json::StyledWriter swriter;

    vector<string> valid_command = {"--title", "--description", "--status", "--creation date", "--deadline", "--percentage", "--priority"};
    string id_str = argv[2];
    int id;
    bool isNum = true;
    bool isExist = false;

    for(int j = 0; j < id_str.size(); ++j){
        if(!isdigit(id_str[j])){
            isNum = false;
            break;
        }
    }

    if(!isNum){
        cout << id_str << " is not an id!" << endl;
        return;
    }
    else{
        id = atoi(argv[2]);
    }

    ifstream ifs("tasks.json");
    if(!reader.parse(ifs, root)){
        cout << "can't read the file 'tasks.json'!";
        return;
    }
    ifs.close();
    for(int i = 0; i < root.size(); ++i){
        if(root[i]["--id"].asInt() == id && is_element_in_vector(valid_command, root[i][argv[3]].asString())){
            root[i][argv[3]] = argv[4];
            isExist = true;
        }
    }
    if(!isExist){
        cout << "Id or attribute does not exist!" << endl;
        return;
    }

    ofstream ofs("tasks.json");
    string str = swriter.write(root);
    ofs << str;
    ofs.close();

    return;
}

void help()
{
    cout << "You can use commands below:\n";
    cout << "create: you can modify attributes '--title', '--description', '--status', '--deadline', '--percentage', '--priority', '--subtask'.\n";
    cout << "list: you can enter attributes to find all tasks that match these attributes\
    ('id', '--title', '--creation date', '--status', '--deadline', '--percentage', '--priority', '--subtask').\n";
    cout << "delete: you can enter id to delete the task realated or use -all to delete all the task.\n";
    cout << "change: you can enter id, name of attributes and attributes to change them.\n";
    cout << "print: you can enter more than one id(seperated by space) or use --all to print all the task.\n";
    cout << "--help: to see all the command." <<endl;
    return;
}

int main(int argc, char *argv[])
{
    cout << argv[1] << endl;
    string command = argv[1];
    if(command == "create"){
        try{
            create(argc, argv);
        }catch(const char* msg){
            cerr << msg << endl;
        }
    }
    else if(command == "list"){
        list(argc, argv);
    }
    else if(command == "delete"){
        task_delete(argc, argv);
    }
    else if(command == "change"){
        change(argc, argv);
    }
    else if(command == "--help"){
        help();
    }
    else if(command == "print"){
        print(argc, argv);
    }
    else{
        cout << "It is not a command valid!" << endl << 
        "Use --help to see all the valid command." << endl;
    }
    return 0;
}