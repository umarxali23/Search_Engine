#include <iostream>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <string>
#include <io.h>
#include <sys/stat.h>
#include <algorithm>
#include "json.hpp"
using json = nlohmann::json;


using namespace std;

//clean every word by implementing lowercase + removing punctuation
string cleanWord(const string& w) {
    string result;
    for (char c : w) {
        if (isalnum(static_cast<unsigned char>(c)))
            result += tolower(c);
    }
    return result;
}

// Process text file
void processTextFile(const string& filepath, unordered_map<string, int>& lexicon) {
    //outputting file name to ensure activity
    cout << "Reading file: " << filepath << endl;

    //open file check
    ifstream file(filepath);
    if (!file.is_open()) {
        cout << "Error opening file: " << filepath << endl;
        return;
    }


    // create a stringstream to store as one string
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string word;
    // extract every word into the stringstream
        while (ss >> word) {
            string cleaned = cleanWord(word);
            // clean the word and add to lexicon if not empty
            if (!cleaned.empty())
                lexicon[cleaned]++;
        }
    }

    file.close();
}


void readAllFiles(const string& path, unordered_map<string, int>& lexicon) {
    struct _finddata_t data;
    intptr_t handle;

    //Check if the path exists and whether it is a directory
    struct _stat info;
    if (_stat(path.c_str(), &info) != 0) {
        cout << "Cannot access: " << path << endl;
        return;
    }

    if (info.st_mode & _S_IFDIR) {
        //If it's a folder, list its contents
        string searchPath = path + "\\*";
        handle = _findfirst(searchPath.c_str(), &data);
        if (handle == -1) {
            cout << "Cannot open folder: " << path << endl;
            return;
        }
        //Loop through folder entries
        do {
            string name = data.name;
            if (name == "." || name == "..") continue; //skip current and parent directories 


            //Build full path and skip hidden/system files
            string fullPath = path + "\\" + name;

            if (data.attrib & _A_HIDDEN) continue;
            if (data.attrib & _A_SYSTEM) continue;

            if (data.attrib & _A_SUBDIR) {
                readAllFiles(fullPath, lexicon);
            }
            //Process supported files
            else {
                string ext = fullPath.substr(fullPath.find_last_of(".") + 1);
                transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                if (ext == "txt" || ext == "csv" || ext == "tsv" || ext == "log" || ext == "md")
                    processTextFile(fullPath, lexicon);
            }

        } while (_findnext(handle, &data) == 0);

        _findclose(handle);
    }
    else {
        //However, if the path is a single file
        string ext = path.substr(path.find_last_of(".") + 1);
        transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext == "txt" || ext == "csv" || ext == "tsv" || ext == "log" || ext == "md")
            processTextFile(path, lexicon);
        else
            cout << "Skipping unsupported file: " << path << endl;
    }
}

int main() {
   
    string path = "C:\\Users\\umarm\\Desktop\\DSA ESP3\\all_sources_metadata_2020-03-13.csv"; 

    unordered_map<string, int> lexicon;

    cout << "Starting...\n";

    readAllFiles(path, lexicon);

    cout << "\nFinished! Total unique words in lexicon: " << lexicon.size() << "\n";

    //Saving lexicon to lexicon.json as a JSON file

json lexJson;
lexJson["lexicon"] = json::array();

// only export words (not counts)
for (auto& p : lexicon)
    lexJson["lexicon"].push_back(p.first);

// write lexicon.json file
std::ofstream out("lexicon.json");
out << lexJson.dump(4);
out.close();

cout << "\n✓ Lexicon saved to lexicon.json\n";

    // Print first 10 words to check whether lexicon is working or not
    int count = 0;
    for (auto &p : lexicon) {
        cout << p.first << " : " << p.second << endl;
        if (++count == 10) break;
    }

    return 0;
}

